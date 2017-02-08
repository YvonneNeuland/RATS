#include "stdafx.h"
#include "GamePlayState.h"
#include "../Object Manager/ObjectManager.h"
#include "../Object Manager/ObjectFactory.h"
//////
#include "../Object Manager/GameObject.h"
#include "../AI/AIManager.h"
#include "../VFX/ElectricityEffect/ElectricityEffect.h"
//////
#include "../Components/HealthComponent.h"

#include "../Audio/FFmod.h"
#include "../Input/InputController.h"
#include "../Input/ThreeSixty.h"
#include "../TinyXml/tinystr.h"
#include "../TinyXml/tinyxml.h"
#include "../Upgrade System/GameData.h"
#include <cassert>
#include <fstream>
#include "../Components/Movement/GroundFollowComponent.h"
#include "../VFX/MeshDistorter/MeshDistorter.h"
#include "../Molecules/MoleculeManager.h"
#include "../Achievements/AchManager.h"

extern	FFmod*		g_AudioSystem;
extern InputController* m_controller;
extern ThreeSixty* gamePad;
extern D3DGraphics* globalGraphicsPointer;
extern GameData* gameData;
extern CAchManager* g_SteamAchievements;

#define LATCH_TIMER 0.75f
#define NUM_SCENERY 25

GamePlayState::GamePlayState()
{
	m_UniformDist = uniform_real_distribution<float>( -1.0f );
}


GamePlayState::~GamePlayState()
{

}

void GamePlayState::Enter( BitmapFontManager* bitmapFontManager, bool resetSound )
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList( m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets() );
	m_Renderer->SetRenderSizes( m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes() );
	m_Renderer->SetRenderEmitters( m_ObjectManager.m_Molecules->GetLiveEmitters() );

	std::string& levelShape = gameData->m_levelShape;

	RenderComp* worldComp = m_pWorldObject->GetComponent<RenderComp>();
	if ( worldComp )
		worldComp->SetID( levelShape );

	worldComp = m_pInnerStar->GetComponent<RenderComp>();
	if ( worldComp )
	{
		std::string toMake;

		if ( levelShape == "Sphere" )
			toMake = "InnerStar";
		else if ( levelShape == "Torus" )
			toMake = "InnerTorus";
		else if ( levelShape == "RoundedCube" )
			toMake = "InnerCube";

		worldComp->SetID( toMake );

	}

	//ResetGame();

	m_debugListener.SetOwner( this );

	if ( m_controller )
	{
		m_controller->SetDefaultKeys();
		m_controller->SetDebugKeys();
	}

	m_ObjectManager.SetupClients();
	SetupClients();
	m_debugListener.SetFlags( DebugListenFor::PlayFlags );

	m_NextState = GAME_PLAY_STATE;
	if ( resetSound )
	{
		//g_AudioSystem->PostEvent(AK::EVENTS::STOP_ALL);
		g_AudioSystem->StopAllSounds();
		//g_AudioSystem->PlayBGM( "RATS_BGM_Normal_01" );
		g_AudioSystem->PlayRandomBGM();
		g_AudioSystem->ResetListener();
		//g_AudioSystem->PlaySound("RATS_GameOver");

		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_SILVER_CITY);
	}
	else
	{
		g_AudioSystem->ResumeAllSounds();
		g_AudioSystem->UnDampenBGM();
	}

	dwPacketNo = gamePad->GetState().state.dwPacketNumber;
	if ( m_Pylon1 == nullptr )
		CreatePylons();

	g_AudioSystem->PlaySound( "RATS_PylonIdle", m_Pylon1, false, true );
	m_Pylon1->GetChannel()->setVolume( 0.2f );

	g_AudioSystem->PlaySound( "RATS_PylonIdle", m_Pylon2, false, true );
	m_Pylon2->GetChannel()->setVolume( 0.2f );

	g_AudioSystem->PlaySound( "RATS_PylonIdle", m_Pylon3, false, true );
	m_Pylon3->GetChannel()->setVolume( 0.2f );


	m_ObjectManager.SetupWhipSound();

	m_unCurrentLevel = gameData->m_cur_level;
	m_pCurrentLevel = m_LevelManager.GetLevel( m_unCurrentLevel );
	m_szLevelShape = m_pCurrentLevel->GetLevelShape();

	m_ObjectManager.SetCamOffset();

	UpdateScore();

	m_fBeatTimer = 0.0f;
	bSongSwitch = false;

	m_ObjectManager.m_pGPS = this;

	g_AudioSystem->PlaySound("RATS_HealthLow", m_pHealthLowSound, true, true);
	m_pHealthLowSound->GetChannel()->setPaused(true);
	//m_ObjectManager.m_Molecules->BuildEmitter(*m_ObjectManager.PassBuckets()[World_Obj].begin(), 500, "InnerStar");
}

void GamePlayState::FireEmp( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (m_empTimer > 0)
	{
		return;
	}



	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	float playerEnergy = GetEnergy( m_Player );
	float playerMaxEnergy = GetMaxEnergy( m_Player );
	float percentLeft = playerEnergy / playerMaxEnergy;

	if ( percentLeft < 0.5f || numActiveEMPs >= 4 )
	{
		g_AudioSystem->PlaySound( "MENU_Error", m_Player, true );
		return;
	}

	float energyRemaining = playerEnergy - (playerMaxEnergy * 0.5f);


	HealthComponent* energyComp = (HealthComponent*)m_Player->GetComponent( "HealthComponent" );

	if ( energyComp != nullptr )
	{
		energyComp->SetEnergy( energyRemaining );
	}

	SpawnEMP( m_Player );

	m_empTimer = 0.25f;

}

void GamePlayState::SpawnEMP( GameObject* target )
{


	g_AudioSystem->PlaySound( "RATS_EMP", target, true );

	if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
	{
		gamePad->Vibrate(0, 0.6f, 0.6f);
	}


	MeshDistorter* newDist = (MeshDistorter*)m_ObjectManager.CreateObject( eOBJECT_TYPE::VFX_Distortion, XMFLOAT3( 0, 0, 0 ) );
	newDist->SetEpicenter( *target->GetTransform().GetPosition() );

	for ( unsigned int currEMP = 0; currEMP < NUM_EMPS; ++currEMP )
	{
		if ( !allPossibleEMPs[currEMP].IsActive() )
		{
			allPossibleEMPs[currEMP].Activate( target->GetTransform(), &m_ObjectManager, numActiveEMPs );
			return;
		}
	}

	//vector<list<GameObject*>>& buckets = m_ObjectManager.PassBuckets();

	//int currbucket = Enemy_Merc;
	//list<GameObject*>::iterator currEnemy;

	//while (currbucket != Player_Obj)
	//{
	//	currEnemy = buckets[currbucket].begin();

	//	while (currEnemy != buckets[currbucket].end())
	//	{
	//		(*currEnemy)->TakeDamage(50);
	//		++currEnemy;
	//	}

	//	++currbucket;
	//}

	//currbucket = eOBJECT_TYPE::EvilBullet_Simple;

	//while (currbucket != Arrow_Sigma)
	//{
	//	currEnemy = buckets[currbucket].begin();

	//	while (currEnemy != buckets[currbucket].end())
	//	{
	//		(*currEnemy)->SetDead();
	//		++currEnemy;
	//	}

	//	++currbucket;
	//}


}

void GamePlayState::Exit()
{
	gameData->m_level_timer = m_fLevelTimer;
	m_debugListener.ClearClients();
	ClearClients();
	if ( m_controller )
	{
		m_controller->ClearDefaultKeys();
		m_controller->ClearDebugKeys();
	}

	//g_AudioSystem->PostEvent(AK::EVENTS::STOP_ALL);
	m_pBitmapFontManager = nullptr;

	if ( m_Pylon1->GetChannel() )
		m_Pylon1->GetChannel()->stop();

	if ( m_Pylon2->GetChannel() )
		m_Pylon2->GetChannel()->stop();

	if ( m_Pylon3->GetChannel() )
		m_Pylon3->GetChannel()->stop();

	m_pHealthLowSound->GetChannel()->stop();

	m_ObjectManager.PauseWhipSound();
	gameData->m_cur_level = m_unCurrentLevel;

	m_Renderer->SetRenderEmitters( nullptr );
	//SaveGame();

	SteamUserStats()->StoreStats();

}

void GamePlayState::Initialize( Renderer* renderer )
{
	m_Renderer = renderer;

	//Create the AIManager and initialize it
	m_AIManager = new AIManager;
	m_AIManager->Init();

	//Initialize the ObjectManager and the ObjectFactory
	m_ObjectManager.InitializeGamePlay();
	m_ObjectFactory.InitializeGamePlay();
	m_ObjectManager.SetFactory( &m_ObjectFactory );

	m_ObjectManager.SetAIManager( m_AIManager );

	//Give the Object Manager a pointer to the AI Manager
	m_ObjectFactory.SetAIManager( m_AIManager );

	//Give the AIManager a pointer to the object Manager
	m_AIManager->SetObjManager( &m_ObjectManager );

	m_Renderer->SetCamera( m_ObjectManager.GetCamera() );

	m_fGamepadBuff = 0;
	//Load level file and get initial Level info
	//m_LevelManager.LoadLevelFile("Assets/XML/LevelXML/DebugLevel.xml");
	//m_LevelManager.LoadLevelFile("Assets/XML/LevelXML/disruptorTest.xml");
	//m_LevelManager.LoadLevelFile("Assets/XML/LevelXML/TwoLevels.xml");
	//m_LevelManager.LoadLevelFile("Assets/XML/LevelXML/LevelPrototype.xml");
	m_LevelManager.LoadLevelFile( "Assets/XML/LevelXML/RATS_Levels.xml" );

	m_pCurrentLevel = m_LevelManager.GetLevel( m_unCurrentLevel );
	m_szLevelShape = m_pCurrentLevel->GetLevelShape();
	gameData->m_num_levels = m_LevelManager.GetNumLevels();
	gameData->LoadData( "" );
	//Assets/XML/Slot1Save.xml
	//Create the player object, world object, and pylons

	m_pWorldObject = m_ObjectManager.CreateObject( eOBJECT_TYPE::World_Obj, DirectX::XMFLOAT3( 0, 0, 0 ) );

	m_pInnerStar = m_ObjectManager.CreateObject( eOBJECT_TYPE::InnerStar, DirectX::XMFLOAT3( 0, 0, 0 ), "InnerStar" );
	m_pInnerStar->GetTransform().SetScaleAll( 0.85f );
	m_pInnerStar->SetSteadyRotation( 1, 0.5f );
	//RenderComp* renderComp = (RenderComp*)m_pInnerStar->GetComponent("RenderComp");
	//renderComp->color = XMFLOAT4(2, 2, 2, 2);

	//CreatePylons();
	CreateScenery();

	m_Player = m_ObjectManager.CreateObject( eOBJECT_TYPE::Player_Obj, DirectX::XMFLOAT3( 60, 75, 0 ) );
	//m_ObjectManager.m_Molecules->BuildPosEmitter(float3(1,1,80), 150, "Shell Test");

	//Set the players ground follow component
	m_Player->GetComponent<GroundFollowComponent>()->GrabFineness() = 0.001f;

	//Spawn all the enemies in the Current Wave
	//SpawnEnemiesInWave();

	//Create the HUD
	CreateHUD();

	m_empTimer = 0;

	m_pHealthLowSound = m_ObjectManager.CreateObject(Audio_LoopHelp, POS_ORIGIN);

}

void GamePlayState::Update( float dt )
{

	//m_safetyTimer += dt;

	//if (m_safetyTimer >= 10)
	//{
	//	m_safetyTimer = 0;
	//	globalGraphicsPointer->FlushOldMemory();
	//	
	//}

	static float gunIconTimer = 0;

	gunIconTimer += dt;

	if (gunIconTimer >= 0.05f)
	{
		gunIconTimer = 0;

		string weapon = gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select]->iconRenderStr;

		RenderComp* gunIcon = (RenderComp*)m_pCurrGunIcon->GetComponent("RenderComp");

		if (weapon == "iconSpreadGun")
		{
			m_pSpreadGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
			m_pWhipGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(0.48f, 0.48f, 0.48f, 1);
			m_pMissileGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(0.48f, 0.48f, 0.48f, 1);
			if (gunIcon != nullptr)
			{
				gunIcon->m_posX = 0.02f;
			}

		}
		else if (weapon == "iconWhipGun")
		{
			m_pSpreadGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(0.48f, 0.48f, 0.48f, 1);
			m_pWhipGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
			m_pMissileGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(0.48f, 0.48f, 0.48f, 1);
			if (gunIcon != nullptr)
			{
				gunIcon->m_posX = 0.08f;
			}
		}
		else
		{
			m_pSpreadGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(0.48f, 0.48f, 0.48f, 1);
			m_pWhipGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(0.48f, 0.48f, 0.48f, 1);
			m_pMissileGunIcon->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
			if (gunIcon != nullptr)
			{
				gunIcon->m_posX = 0.14f;
			}
		}
	}




	if (m_fBeatTimer == 0.0f)
	{
		g_AudioSystem->GrabFreq();
		if (g_AudioSystem->BeatCheck())
		{
			m_fBeatTimer = 0.2f;

			//SetObjectColor(m_pInnerStar, XMFLOAT4(0.6f, 2, 2, 2));
			// TODO: pulse code goes here
			Pulse();
		}
	}
	else
	{
		m_fBeatTimer -= dt;
		if (m_fBeatTimer <= 0.0f)
		{
			m_fBeatTimer = 0.0f;

			//SetObjectColor(m_pInnerStar, XMFLOAT4(1, 1, 1, 1));
		}
			
	}

	if ( !m_bGameOver )
	{
		for ( unsigned int currEMP = 0; currEMP < 4; ++currEMP )
		{
			if ( allPossibleEMPs[currEMP].IsActive() )
				allPossibleEMPs[currEMP].Update( &m_ObjectManager, dt, m_UniformDist, m_RandEngine, numActiveEMPs );
		}

		m_ObjectManager.Update( dt );
		m_AIManager->UpdateMinions( dt );
		m_debugListener.Update( dt );
		UpdateHud( dt );

		if (m_empTimer >= 0)
		{
			m_empTimer -= dt;
		}

		if ( !m_bFillPylonMessagePlayed && m_unCurrentLevel == 0 )
		{
			PlayFillPylonMessage( dt );
		}

		if (gameData->m_bUsingGamepad)
		{
			m_fGamepadBuff += dt;
	
			// GAMEPAD STUFF
			if ( m_fGamepadBuff >= 0.1f )
			{
				if ( dwPacketNo != gamePad->GetState().state.dwPacketNumber )
				{
					dwPacketNo = gamePad->GetState().state.dwPacketNumber;
					Handle360Input();
					globalGraphicsPointer->SetCrosshairShowing( false );
					m_fGamepadBuff = 0;
				}
			}
			if (_finite(m_fGamepadBuff) == 0)
			{
				std::cout << "GAMEPAD TIMER IS GARBAGE VAL!!\n";
				m_fGamepadBuff = 0;
			}
		}
		if (gamePad->LostConnection())
		{
			m_NextState = STATE_TYPE::PAUSE_MENU_STATE;
			//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
			g_AudioSystem->PlaySound("MENU_Accept");
			g_AudioSystem->PauseAllSounds();
			g_AudioSystem->DampenBGM();
		}

		if(m_bLevelPassed)
		{
			UpdateLevelClear(dt);
		}

		// Wave is spawning in
		else if ( m_bWavesPaused )
		{
			PauseWaves( dt );
			UpdateEnvironmentalHazards( dt );
		}


		// Normal gameplay
		else
		{
			UpdateLevelInfo();
			UpdateEnvironmentalHazards( dt );
			CheckForGameLost();

			// Updates timer for multiplier, returns true if timer expired
			if ( gameData->Update( dt ) )
			{
				UpdateScore();
				// Sound & Visual effects can play here when reset happens
			}

			//if the game has been won
			//if ( m_bGameWon )
			//{
			//	m_GameWon = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "GameWon" );
			//	RenderComp* gameWon = (RenderComp*)m_GameWon->GetComponent( "RenderComp" );

			//	if ( gameWon != nullptr )
			//	{
			//		gameWon->m_posX = 0.3f;
			//		gameWon->m_posY = 0.3f;
			//		gameWon->m_height = 0.3f;
			//		gameWon->m_width = 0.4f;
			//	}

			//	m_bGameOver = m_bResetGame = true;
			//	g_AudioSystem->StopAllSounds();
			//	g_AudioSystem->StopBGM();
			//	GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
			//	g_AudioSystem->PlaySound( "RATS_GameWon", tmpAudioOneshot, true );
			//}

			//if the game has been lost
			if ( m_bGameLost )
			{
				//m_GameLost = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "GameOver" );
				//RenderComp* gameLost = (RenderComp*)m_GameLost->GetComponent( "RenderComp" );

				//if ( gameLost != nullptr )
				//{
				//	// 3,3,3,4 (original values)
				//	gameLost->m_posX = 0.4f;
				//	gameLost->m_posY = 0.1f;
				//	gameLost->m_height = 0.2f;
				//	gameLost->m_width = 0.267f;
				//}

				// If this is the first level, ACHIEVEMENT CHECK
				if (m_unCurrentLevel == 0)
				{
					if (g_SteamAchievements != nullptr)
					{
						if (g_SteamAchievements->m_pAchievements[EAchievements::ACH_DIE_FIRST_LEVEL].m_bAchieved == false)
						{
							g_SteamAchievements->m_pAchievements[EAchievements::ACH_DIE_FIRST_LEVEL].m_bAchieved = true;
							g_SteamAchievements->SetAchievement("ACH_DIE_FIRST_LEVEL");
							std::cout << "Triggering ACH_DIE_FIRST_LEVEL\n";
						}
					}
				}



				m_bWavesPaused = false;
				m_fPauseTimer = 0.0f;
				m_fPauseTimeLeft = 0.0f;

				m_bGameOver = m_bResetGame = true;
				///gameData->m_bPlayerDead = true;
				g_AudioSystem->StopAllSounds();
				g_AudioSystem->StopBGM();


				m_unCurrentWave = 0;
				//m_unCurrentLevel++;

				RenderComp* levelWinText = (RenderComp*)m_pLevelWinText->GetComponent("RenderComp");
				if (levelWinText)
				{
					levelWinText->color = XMFLOAT4(1, 1, 1, 0);
				}

				RenderComp* levelLoseText = (RenderComp*)m_pLevelLoseText->GetComponent("RenderComp");
				if (levelLoseText)
				{
					levelLoseText->color = XMFLOAT4(1, 1, 1, 0);
				}

				gameData->m_mult_timer = -0.1f;
				gameData->Update(0.10f);
				g_AudioSystem->StopAllSounds();
				g_AudioSystem->StopBGM();
				m_NextState = RESULT_STATE;
				m_bPylonsAllFull = false;
				gameData->m_bResetGame = true;



				//GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
				//g_AudioSystem->PlaySound( "RATS_GameOver", tmpAudioOneshot, true );
				//g_AudioSystem->PlaySound("RATS_GameWon");

			}
		}

		g_AudioSystem->UpdateListener( m_ObjectManager.GetCamera() );

	}

	// Game Over Screen
	else
	{
		// Let's have the game update for a few before things go kaput
		//UpdateGameOver(dt);

		if ( gamePad->GetState().state.dwPacketNumber != dwPacketNo )
		{
			if ( gamePad->GetState().state.Gamepad.wButtons )
			{
				// Changing this from Main Menu to Result State
				// And updating how result-state functions
				//m_NextState = STATE_TYPE::MAIN_MENU_STATE;
				m_NextState = STATE_TYPE::RESULT_STATE;
				
			}
		}

		gameData->m_bResetGame = true;
	}

	if ( m_pMeshDistFX )
	{
		reinterpret_cast<MeshDistorter*>(m_pMeshDistFX)->Update( dt );
	}

	if ( m_fTimeSlowHelper > 0 )
	{
		m_fTimeSlowHelper -= dt;
		if ( m_fTimeSlowHelper <= 0 )
		{
			m_fTimeSlowHelper = 0;
			//m_bTImeSlow = false;
			globalGraphicsPointer->SetScreenWarp( false );
		}
	}
}

void GamePlayState::Terminate()
{
	m_ObjectManager.ResetAll();
	m_LevelManager.Terminate();
	m_AIManager->Shutdown();
	delete m_AIManager;
	m_AIManager = nullptr;
}

void GamePlayState::UpdateLevelInfo()
{

	int numMinionsActive = 0;
	for ( int i = 1; i < eOBJECT_TYPE::Player_Obj; i++ )
		numMinionsActive += m_ObjectManager.PassBuckets()[i].size();

	m_unNumEnemiesKilled = m_unNumEnemiesToKill - numMinionsActive;


	if ( m_unNumEnemiesToKill <= m_unNumEnemiesKilled )
	{
		m_unNumEnemiesKilled = 0;
		m_unNumEnemiesToKill = 0;

		m_bWavesPaused = true;

		if ( (m_pCurrentLevel->GetNumWaves() - 2) > m_unCurrentWave )
		{
			if ( CheckForPylonHealthFull() )
			{
				m_unCurrentWave = (m_pCurrentLevel->GetNumWaves() - 1);
			}
			else
			{
				m_unCurrentWave++;
			}
		}
		else if ( (m_pCurrentLevel->GetNumWaves() - 2) <= m_unCurrentWave && !CheckForPylonHealthFull() )
		{
			m_unCurrentWave = 0;
			//m_nEnemyMultiplier++;
		}
		else if ( CheckForPylonHealthFull() && (m_pCurrentLevel->GetNumWaves() - 1) > m_unCurrentWave )
		{
			m_unCurrentWave = (m_pCurrentLevel->GetNumWaves() - 1);

		}

		// Level Complete
		else if ( (m_LevelManager.GetNumLevels()) > m_unCurrentLevel )
		{
			if (((m_LevelManager.GetNumLevels() - 1) > m_unCurrentLevel) == false)
			{
				globalGraphicsPointer->justBeatTheGame = true;

				// We just beat the last level, ACHIEVEMENT CHECK

				if (g_SteamAchievements != nullptr)
				{
					if (g_SteamAchievements->m_pAchievements[EAchievements::ACH_BEAT_ALL_LEVELS].m_bAchieved == false)
					{
						g_SteamAchievements->m_pAchievements[EAchievements::ACH_BEAT_ALL_LEVELS].m_bAchieved = true;
						g_SteamAchievements->SetAchievement("ACH_BEAT_ALL_LEVELS");
						std::cout << "Triggering ACH_BEAT_ALL_LEVELS\n";
					}
				}
				
			}
			else if (m_unCurrentLevel == 9)
			{
				// We just beat the first boss level, achievement time
				if (g_SteamAchievements != nullptr)
				{
					if (g_SteamAchievements->m_pAchievements[EAchievements::ACH_BEAT_BOSS_ONE].m_bAchieved == false)
					{
						g_SteamAchievements->m_pAchievements[EAchievements::ACH_BEAT_BOSS_ONE].m_bAchieved = true;
						g_SteamAchievements->SetAchievement("ACH_BEAT_BOSS_ONE");
						std::cout << "Triggering ACH_BEAT_BOSS_ONE\n";
					}
				}
			}


			// BOOKMARK : Level Complete
			// If this works correctly, here is where you change the player status/behavior (if we attach an Ai zoom around thing or whatever)
			m_bLevelPassed = true;
			m_fLevelClearTimer = 10.0f;

			// Spawn Random Fireworks around player
			SpawnFireworks();


			//m_unCurrentWave = 0;
			////m_unCurrentLevel++;
			//gameData->m_mult_timer = -0.1f;
			//gameData->Update( 0.10f );
			//g_AudioSystem->StopAllSounds();
			//g_AudioSystem->StopBGM();
			////m_NextState = RESULT_STATE;
			//m_bPylonsAllFull = false;
			//gameData->m_bResetGame = true;
		}

		//else
		//{
		//	m_bGameWon = true;
		//	m_unCurrentWave = 0;
		//	//m_unCurrentLevel = 0;
		//	gameData->m_bResetGame = true;

		//}
	}
}

void GamePlayState::SpawnFireworks()
{
	// Comment for marking the file green so I know where I'm working
	for(int i=0; i<1; i++)
	{
		float x,y,z;
		int temp = rand()%35+1;
		x = (float)temp;
		y = 5.0f;
		temp = rand()%35+1;
		z = (float)temp;

		DirectX::XMFLOAT3 offset = DirectX::XMFLOAT3(x,y,z);
		MoleculeEmitter *fwork;
		fwork = 
		m_ObjectManager.m_Molecules->BuildEmitter(m_ObjectManager.GetPlayer(), 150, "FireWork");

		fwork->m_offset = offset;
	}
}

void GamePlayState::SpawnEnemiesInWave( void )
{
	m_unNumEnemiesToKill = 0;

	unsigned int numTypes = m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector().size();

	int axis = 1;

	for ( unsigned int enemyType = 0; enemyType < numTypes; enemyType++ )
	{

		int numEnemies = m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->numEnemy * m_nEnemyMultiplier;

		if ( m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->enemyType == eOBJECT_TYPE::Enemy_Koi )
		{

			m_AIManager->CreateKoiAttack( m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->numEnemy );
			m_unNumEnemiesToKill += m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->numEnemy * m_nEnemyMultiplier;
		}
		else if ( m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->enemyType == eOBJECT_TYPE::Enemy_Disruptor )
		{

			m_AIManager->CreateKoiDisrupt( m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->numEnemy );
			m_unNumEnemiesToKill += m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->numEnemy * m_nEnemyMultiplier;
		}

		else
		{

			if (m_pCurrentLevel->GetWave(m_unCurrentWave)->GetLoadedEnemyInfoVector()[enemyType]->enemyType == eOBJECT_TYPE::Enemy_Boss)
			{
				//SPAWN HEALTH BAR
				m_pBossHealthBack = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, POS_ORIGIN, "PlayerHealthBarBack", Render2D_Back);
				RenderComp* healthBack = (RenderComp*)m_pBossHealthBack->GetComponent<RenderComp>();
				if (healthBack != nullptr)
				{
					healthBack->m_posX = 0.35f;
					healthBack->m_posY = 0.84f;
					healthBack->m_height = 0.115f;
					healthBack->m_width = 0.33f;
					
				}


				m_pBossHealthFill = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, POS_ORIGIN, "PlayerHealthBarFill", Render2D_Front);
				healthBack = (RenderComp*)m_pBossHealthFill->GetComponent<RenderComp>();

				if (healthBack != nullptr)
				{
					healthBack->m_posX = 0.37f;
					healthBack->m_posY = 0.88f;
					healthBack->m_height = 0.04f;
					healthBack->m_width = 0.29f;
					fBossHealthSize = healthBack->m_width;
					healthBack->color = XMFLOAT4(1, 0, 0, 1);
				}

			}
			// TESTING NEW ENEMIES

			//if (m_pCurrentLevel->GetWave(m_unCurrentWave)->GetLoadedEnemyInfoVector()[enemyType]->enemyType == Enemy_Merc)
			//{
			//	m_unNumEnemiesToKill = 1;
			//	//for (int tmp = 0; tmp < 5; tmp++)
			//	{
			//		m_ObjectManager.CreateObject(Enemy_Boss, DirectX::XMFLOAT3(0, 0, 0));
			//	}
			//	
			//	return;
			//}


			for ( int enemy = 0; enemy < numEnemies; enemy++ )
			{
				axis = rand() % 3 + 1;

				switch ( axis )
				{
					case 1:
					{
							  GameObject* newEnemy = m_ObjectManager.CreateObject( m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->enemyType, DirectX::XMFLOAT3( 76.0f, (float)enemy + 10.0f, enemy*5.0f ) );
							  Transform trans = newEnemy->GetTransform();
					}
						break;
					case 2:
					{
							  GameObject* newEnemy = m_ObjectManager.CreateObject( m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->enemyType, DirectX::XMFLOAT3( (float)enemy + 10.0f, 76.0f, enemy*5.0f ) );
							  Transform trans = newEnemy->GetTransform();

					}
						break;
					case 3:
					{
							  GameObject* newEnemy = m_ObjectManager.CreateObject( m_pCurrentLevel->GetWave( m_unCurrentWave )->GetLoadedEnemyInfoVector()[enemyType]->enemyType, DirectX::XMFLOAT3( (float)enemy + 10.0f, enemy*5.0f, 76.0f ) );
							  Transform trans = newEnemy->GetTransform();

					}
						break;
					default:
						break;
				}

				m_unNumEnemiesToKill++;
			}
		}
	}
}

void GamePlayState::PauseWaves( float dt )
{

	std::string textToShow;

	if ( m_ObjectManager.NumEnemiesInBuckets() == 0)
	{
		GameObject* tmpOneSHot = m_ObjectManager.CreateObject( Audio_OneShot, DirectX::XMFLOAT3( 0, 0, 0 ) );
		g_AudioSystem->PlaySound( "RATS_EnemySpawn", tmpOneSHot, true );
		SpawnEnemiesInWave();

	}

	m_fPauseTimer += dt;
	m_fPauseTimeLeft = 5.0f - m_fPauseTimer;
	if ( m_fPauseTimer >= 5.0f )
	{
		m_fPauseTimer = 0.0f;
		m_bWavesPaused = false;
		m_pPauseTimeText->SetDead();
		m_pPauseTimeText = nullptr;
		m_pPauseTimeNumber->SetDead();
		m_pPauseTimeNumber = nullptr;
		m_nLastTime = 5;

	}

	else
	{
		if ( m_pPauseTimeText == nullptr )
		{

			if (m_pArrowSigma == nullptr &&
				m_pArrowLamda == nullptr &&
				m_pArrowOmega == nullptr)
			{
				if (m_pCurrentLevel->GetWave(m_pCurrentLevel->GetNumWaves() - 1)->GetLoadedEnemyInfoVector()[0]->enemyType == Enemy_Boss)
				{
					textToShow = "Boss Wave:";
				}
				else
				{
					textToShow = "Final Wave:";
				}
			}
			else
			{
				textToShow = "Next Wave:";
			}

			m_pPauseTimeText = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), textToShow, eRENDERCOMP_TYPE::Render2D_Text );
			RenderComp* pauseTimer = (RenderComp*)m_pPauseTimeText->GetComponent<RenderComp>();

			if ( pauseTimer != nullptr )
			{
				pauseTimer->m_posX = 0.19f;
				pauseTimer->m_posY = 0.25f;
				pauseTimer->m_height = 0.3f;
				pauseTimer->m_width = 0.4f;
			}
		}

		//Fade Text Red

		float timeDiff = m_fPauseTimeLeft / 5.0f;

		timeDiff = 1.0f - timeDiff;

		RenderComp* pauseTimeTextComp = (RenderComp*)m_pPauseTimeText->GetComponent<RenderComp>();

		if (pauseTimeTextComp)
		{
			pauseTimeTextComp->color = XMFLOAT4(1, 1, 1, 1);
		}

		if ( m_nLastTime > (int)m_fPauseTimeLeft )
		{
			m_nLastTime = (int)m_fPauseTimeLeft;

			if ( m_pPauseTimeNumber != nullptr )
			{
				m_pPauseTimeNumber->SetDead();
				m_pPauseTimeNumber = nullptr;
			}

			string number = "";
			number += std::to_string( m_nLastTime );
			m_pPauseTimeNumber = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), number, eRENDERCOMP_TYPE::Render2D_Text );
			RenderComp* pauseTimer = (RenderComp*)m_pPauseTimeNumber->GetComponent<RenderComp>();

			if ( pauseTimer != nullptr )
			{
				pauseTimer->m_posX = 0.72f;
				pauseTimer->m_posY = 0.25f;
				pauseTimer->m_height = 0.3f;
				pauseTimer->m_width = 0.4f;
			}
		}

		//Fade Number Red

		pauseTimeTextComp = (RenderComp*)m_pPauseTimeNumber->GetComponent<RenderComp>();

		if (pauseTimeTextComp)
		{
			pauseTimeTextComp->color = XMFLOAT4(1, 1, 0, 1);
		}
	}
}

void GamePlayState::CheckForGameLost()
{

	//if (m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Player_Obj].size() == 0)
	if ( gameData->m_bPlayerDead )
	{
		m_bGameLost = true;
	}

	/*if ( m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Test].size() == 0 )
	{
	m_bGameLost = true;
	}*/
}

void GamePlayState::PlayFillPylonMessage( float dt )
{
	if ( !m_bFillPylonMessageMade )
	{
		std::string message = "Fill the pylons to complete the level!";
		m_pFillPylonMessage = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), message, eRENDERCOMP_TYPE::Render2D_Text );

		RenderComp* fillMessage = (RenderComp*)m_pFillPylonMessage->GetComponent<RenderComp>();

		if ( fillMessage != nullptr )
		{
			fillMessage->m_posX = 0.1f;
			fillMessage->m_posY = 0.7f;
			fillMessage->m_height = 0.2f;
			fillMessage->m_width = 0.2f;

			fillMessage->color = XMFLOAT4( 1.0f, 1.0f, 0, 1.0f );
		}
		m_bFillPylonMessageMade = true;
	}

	m_fFillPylonTimer += dt;
	if ( m_fFillPylonTimer > 5.0f )
	{
		m_bFillPylonMessagePlayed = true;
		m_pFillPylonMessage->SetDead();
		m_pFillPylonMessage = nullptr;
	}
}

void GamePlayState::CreateHUD( void )
{
	CreateScalarMessageObjects();

	//set up the front of the player health bar		
	m_PlayerHealthFill = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PlayerHealthBarFill", Render2D_Front );
	RenderComp* healthFill = (RenderComp*)m_PlayerHealthFill->GetComponent<RenderComp>();

	if ( healthFill != nullptr )
	{
		healthFill->m_posX = 0.025f;
		healthFill->m_posY = 0.029f;
		healthFill->m_height = 0.054f;
		healthFill->m_width = 0.26f;
		m_fPlayerHealthSize = healthFill->m_width;
		healthFill->color = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );
	}

	//set up the back of the player health bar
	m_PlayerHealthBack = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PlayerHealthBarBack");
	RenderComp* healthBack = (RenderComp*)m_PlayerHealthBack->GetComponent<RenderComp>();

	if ( healthBack != nullptr )
	{
		healthBack->m_posX = 0.0095f;
		healthBack->m_posY = -0.018f;
		healthBack->m_height = 0.14f;
		healthBack->m_width = 0.29f;
	}

	//set up the front of the player energy bar
	m_PlayerEnergyFill = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PlayerEnergyFill", Render2D_Front );
	RenderComp* energyFill = (RenderComp*)m_PlayerEnergyFill->GetComponent<RenderComp>();

	if ( energyFill != nullptr )
	{
		energyFill->m_posX = 0.025f;
		energyFill->m_posY = 0.12f;
		energyFill->m_height = 0.054f;
		energyFill->m_width = 0.26f;
		m_fPlayerEnergySize = energyFill->m_width;
	}

	//set up the back of the player energy bar
	m_PlayerEnergyBack = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PlayerEnergyBarBack" );
	RenderComp* energyBack = (RenderComp*)m_PlayerEnergyBack->GetComponent<RenderComp>();

	if ( energyBack != nullptr )
	{
		energyBack->m_posX = 0.0095f;
		energyBack->m_posY = 0.072f;
		energyBack->m_height = 0.14f;
		energyBack->m_width = 0.29f;
	}

	//set up the front of the first pylon
	m_Pylon1HealthFill = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PylonEnergyFill1", Render2D_Front );
	healthFill = (RenderComp*)m_Pylon1HealthFill->GetComponent( "RenderComp" );

	if ( healthFill != nullptr )
	{
		healthFill->m_posX = 0.8118f;
		healthFill->m_posY = 0.025f;
		healthFill->m_height = 0.26f;
		healthFill->m_width = 0.025f;
		m_fPylonHealthSize = healthFill->m_height;
	}

	//set up the back of the first pylon
	m_Pylon1HealthBack = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PylonEnergyBarBack" );
	healthBack = (RenderComp*)m_Pylon1HealthBack->GetComponent( "RenderComp" );

	if ( healthBack != nullptr )
	{
		healthBack->m_posX = 0.794f;
		healthBack->m_posY = 0.01f;
		healthBack->m_height = 0.29f;
		healthBack->m_width = 0.065f;
	}

	//set up the icon for the first pylon - Sigma
	m_pPylon1Icon = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "iconPylon1" );
	RenderComp* iconPylon1 = (RenderComp*)m_pPylon1Icon->GetComponent( "RenderComp" );

	if ( iconPylon1 != nullptr )
	{
		iconPylon1->m_posX = 0.8f;
		iconPylon1->m_posY = 0.28f;
		iconPylon1->m_height = 0.05f;
		iconPylon1->m_width = 0.05f;
	}

	//set up the front of the second pylon
	m_Pylon2HealthFill = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PylonEnergyFill2", Render2D_Front );
	healthFill = (RenderComp*)m_Pylon2HealthFill->GetComponent( "RenderComp" );

	if ( healthFill != nullptr )
	{
		healthFill->m_posX = 0.8618f;
		healthFill->m_posY = 0.025f;
		healthFill->m_height = 0.26f;
		healthFill->m_width = 0.025f;
	}

	//set up the back of the second pylon
	m_Pylon2HealthBack = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PylonEnergyBarBack" );
	healthBack = (RenderComp*)m_Pylon2HealthBack->GetComponent( "RenderComp" );

	if ( healthBack != nullptr )
	{
		healthBack->m_posX = 0.844f;
		healthBack->m_posY = 0.01f;
		healthBack->m_height = 0.29f;
		healthBack->m_width = 0.065f;
	}

	//set up the icon for the second pylon - Lamda
	m_pPylon2Icon = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "iconPylon2" );
	RenderComp* iconPylon2 = (RenderComp*)m_pPylon2Icon->GetComponent( "RenderComp" );

	if ( iconPylon2 != nullptr )
	{
		iconPylon2->m_posX = 0.85f;
		iconPylon2->m_posY = 0.28f;
		iconPylon2->m_height = 0.05f;
		iconPylon2->m_width = 0.05f;
	}

	//set up the front of the third pylon
	m_Pylon3HealthFill = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PylonEnergyFill3", Render2D_Front );
	healthFill = (RenderComp*)m_Pylon3HealthFill->GetComponent( "RenderComp" );

	if ( healthFill != nullptr )
	{
		healthFill->m_posX = 0.9118f;
		healthFill->m_posY = 0.025f;
		healthFill->m_height = 0.26f;
		healthFill->m_width = 0.025f;
	}

	//set up the back of the third pylon
	m_Pylon3HealthBack = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "PylonEnergyBarBack" );
	healthBack = (RenderComp*)m_Pylon3HealthBack->GetComponent( "RenderComp" );

	if ( healthBack != nullptr )
	{
		healthBack->m_posX = 0.894f;
		healthBack->m_posY = 0.01f;
		healthBack->m_height = 0.29f;
		healthBack->m_width = 0.065f;
	}

	//set up the icon for the third pylon - Omega
	m_pPylon3Icon = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "iconPylon3" );
	RenderComp* iconPylon3 = (RenderComp*)m_pPylon3Icon->GetComponent( "RenderComp" );

	if ( iconPylon3 != nullptr )
	{
		iconPylon3->m_posX = 0.9f;
		iconPylon3->m_posY = 0.28f;
		iconPylon3->m_height = 0.05f;
		iconPylon3->m_width = 0.05f;
	}

	//set up emp icon
	m_empIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EmpIcon");
	RenderComp* empIconComp = (RenderComp*)m_empIcon->GetComponent("RenderComp");

	if (empIconComp != nullptr)
	{
		empIconComp->m_posX = 0.175f;
		empIconComp->m_posY = 0.175f;
		empIconComp->m_height = 0.1f;
		empIconComp->m_width = 0.1f;
	}

	m_empIcon2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EmpIcon");
	RenderComp* empIconComp2 = (RenderComp*)m_empIcon2->GetComponent("RenderComp");

	if (empIconComp2 != nullptr)
	{
		empIconComp2->m_posX = 0.23f;
		empIconComp2->m_posY = 0.175f;
		empIconComp2->m_height = 0.1f;
		empIconComp2->m_width = 0.1f;
	}

	//Set up the Wave Counter Text
	m_pLevelTimerText = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "Time:", eRENDERCOMP_TYPE::Render2D_Text );
	RenderComp* levelTimerText = (RenderComp*)m_pLevelTimerText->GetComponent( "RenderComp" );

	if ( levelTimerText != nullptr )
	{
		levelTimerText->m_posX = 0.73f;
		levelTimerText->m_posY = 0.9f;
		levelTimerText->m_height = 0.2f;
		levelTimerText->m_width = 0.2f;
	}

	//Set up victory
	m_pLevelWinText = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "Victory!", eRENDERCOMP_TYPE::Render2D_Text);
	RenderComp* levelWinText = (RenderComp*)m_pLevelWinText->GetComponent("RenderComp");

	if (levelWinText != nullptr)
	{
		levelWinText->m_posX = 0.325f;
		levelWinText->m_posY = 0.25f;
		levelWinText->m_height = 0.4f;
		levelWinText->m_width = 0.4f;

		levelWinText->color = XMFLOAT4(1, 1, 1, 0);
	}

	m_pLevelLoseText = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "Defeat!", eRENDERCOMP_TYPE::Render2D_Text);
	RenderComp* levelLoseText = (RenderComp*)m_pLevelLoseText->GetComponent("RenderComp");

	globalGraphicsPointer->m_loseText = levelLoseText;

	if (levelLoseText != nullptr)
	{
		levelLoseText->m_posX = 0.34f;
		levelLoseText->m_posY = 0.25f;
		levelLoseText->m_height = 0.4f;
		levelLoseText->m_width = 0.4f;

		levelLoseText->color = XMFLOAT4(1, 1, 1, 0);
	}

	//Set up the Wave Counter number
	string number = "0:00";

	m_pLevelTimerNumber = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), number, eRENDERCOMP_TYPE::Render2D_Text );

	RenderComp* levelTimerNumber = (RenderComp*)m_pLevelTimerNumber->GetComponent( "RenderComp" );

	if ( levelTimerNumber != nullptr )
	{
		levelTimerNumber->m_posX = 0.85f;
		levelTimerNumber->m_posY = 0.9f;
		levelTimerNumber->m_height = 0.2f;
		levelTimerNumber->m_width = 0.2f;
	}

	//Set up the Score Text
	m_pScoreText = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "Score:", eRENDERCOMP_TYPE::Render2D_Text );
	RenderComp* scoreText = (RenderComp*)m_pScoreText->GetComponent( "RenderComp" );

	if ( scoreText != nullptr )
	{
		scoreText->m_posX = 0.01f;
		scoreText->m_posY = 0.9f;
		scoreText->m_height = 0.2f;
		scoreText->m_width = 0.2f;
	}

	//Set up the Tally Text
	m_pTallyText = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "+", eRENDERCOMP_TYPE::Render2D_Text );
	RenderComp* tallyText = (RenderComp*)m_pTallyText->GetComponent( "RenderComp" );

	if (tallyText != nullptr)
	{
		tallyText->m_posX = 0.13f;
		tallyText->m_posY = 0.85f;
		tallyText->m_height = 0.2f;
		tallyText->m_width = 0.2f;

		tallyText->color = XMFLOAT4(0, 0, 0, 0);
	}

	// Setup tally number
	m_pTallyNumber = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "0", eRENDERCOMP_TYPE::Render2D_Text );
	RenderComp* tallyNum = (RenderComp*)m_pTallyNumber->GetComponent( "RenderComp" );

	if (tallyNum != nullptr)
	{
		tallyNum->m_posX = 0.16f;
		tallyNum->m_posY = 0.85f;
		tallyNum->m_height = 0.2f;
		tallyNum->m_width = 0.2f;

		tallyNum->color = XMFLOAT4(0, 0, 0, 0);

	}

	// Set up Gun icon
	m_pSpreadGunIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "iconSpreadGun", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* gunIcon = (RenderComp*)m_pSpreadGunIcon->GetComponent("RenderComp");

	if (gunIcon != nullptr)
	{
		gunIcon->m_posX = 0.02f;
		gunIcon->m_posY = 0.2f;
		gunIcon->m_height = 0.08f;
		gunIcon->m_width = 0.05f;
	}

	m_pWhipGunIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "iconWhipGun", eRENDERCOMP_TYPE::Render2D_Front);
	 gunIcon = (RenderComp*)m_pWhipGunIcon->GetComponent("RenderComp");

	if (gunIcon != nullptr)
	{
		gunIcon->m_posX = 0.08f;
		gunIcon->m_posY = 0.2f;
		gunIcon->m_height = 0.08f;
		gunIcon->m_width = 0.05f;
	}

	m_pMissileGunIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "iconMissileGun", eRENDERCOMP_TYPE::Render2D_Front);
	 gunIcon = (RenderComp*)m_pMissileGunIcon->GetComponent("RenderComp");

	if (gunIcon != nullptr)
	{
		gunIcon->m_posX = 0.14f;
		gunIcon->m_posY = 0.2f;
		gunIcon->m_height = 0.08f;
		gunIcon->m_width = 0.05f;
	}

	m_pCurrGunIcon = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "iconOutline", eRENDERCOMP_TYPE::Render2D_Front );
	 gunIcon = (RenderComp*)m_pCurrGunIcon->GetComponent( "RenderComp" );

	if ( gunIcon != nullptr )
	{
		gunIcon->m_posX = 0.02f;
		gunIcon->m_posY = 0.2f;
		gunIcon->m_height = 0.08f;
		gunIcon->m_width = 0.05f;
	}

	//Set up the Score Number
	number = "";
	number += std::to_string( gameData->m_score_current );
	m_pScoreNumber = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), number, eRENDERCOMP_TYPE::Render2D_Text );

	RenderComp* scoreNumber = (RenderComp*)m_pScoreNumber->GetComponent( "RenderComp" );

	if ( scoreNumber != nullptr )
	{
		scoreNumber->m_posX = 0.16f;
		scoreNumber->m_posY = 0.9f;
		scoreNumber->m_height = 0.2f;
		scoreNumber->m_width = 0.2f;
	}

	//Set up ArrowSigma	
	XMFLOAT3 playerPos = *m_Player->GetTransform().GetPosition();

	m_pArrowSigma = m_ObjectManager.CreateObject( eOBJECT_TYPE::Arrow_Sigma, playerPos, "arrowSigma", eRENDERCOMP_TYPE::Opaque );
	m_pArrowSigma->GetTransform() = m_ObjectManager.GetCamera()->GetTransform();


	//Set up Arrow Lambda
	m_pArrowLamda = m_ObjectManager.CreateObject( eOBJECT_TYPE::Arrow_Lambda, playerPos, "arrowLambda", eRENDERCOMP_TYPE::Opaque );
	m_pArrowSigma->GetTransform() = m_ObjectManager.GetCamera()->GetTransform();

	//Set up ArrowOmega
	m_pArrowOmega = m_ObjectManager.CreateObject( eOBJECT_TYPE::Arrow_Omega, playerPos, "arrowOmega", eRENDERCOMP_TYPE::Opaque );
	m_pArrowOmega->GetTransform() = m_ObjectManager.GetCamera()->GetTransform();
}

float GamePlayState::GetHealth( GameObject* obj )
{
	HealthComponent* healthComp = (HealthComponent*)obj->GetComponent( "HealthComponent" );

	if ( healthComp != nullptr )
	{
		float health = healthComp->GetHealth();
		return health;
	}
	else
	{
		return 0.0f;
	}
}

float GamePlayState::GetMaxHealth( GameObject* obj )
{
	HealthComponent* healthComp = (HealthComponent*)obj->GetComponent( "HealthComponent" );

	if ( healthComp != nullptr )
	{
		float maxHealth = healthComp->GetMaxHealth();
		return maxHealth;
	}
	else
	{
		return 1.0f;
	}
}

float GamePlayState::GetEnergy( GameObject* obj )
{
	HealthComponent* energyComp = (HealthComponent*)obj->GetComponent( "HealthComponent" );

	if ( energyComp != nullptr )
	{
		float energy = energyComp->GetEnergy();
		return energy;
	}
	else
	{
		return 0.0f;
	}
}

float GamePlayState::GetMaxEnergy( GameObject* obj )
{
	HealthComponent* energyComp = (HealthComponent*)obj->GetComponent( "HealthComponent" );

	if ( energyComp != nullptr )
	{
		float maxEnergy = energyComp->GetMaxEnergy();
		return maxEnergy;;
	}
	else
	{
		return 1.0f;
	}
}

void GamePlayState::UpdateHud( float dt )
{
	UpdatePlayerHUD( dt );
	UpdatePylon1HUD( dt );
	UpdatePylon2HUD( dt );
	UpdatePylon3HUD( dt );
	UpdateTally();
	UpdateLevelTimer( dt );
	UpdateScalarMessageHUD( dt );
	UpdateBossHud(dt);

	if (!bSongSwitch)
	{
		if (m_pArrowOmega == nullptr &&
			m_pArrowLamda == nullptr &&
			m_pArrowSigma == nullptr)
		{
			g_AudioSystem->StopBGM();
			g_AudioSystem->ResetListener();

			if (m_pCurrentLevel->GetWave(m_pCurrentLevel->GetNumWaves() - 1)->GetLoadedEnemyInfoVector()[0]->enemyType == Enemy_Boss)
			{
				g_AudioSystem->PlayBGM("RATS_BGM_Boss");
			}
			else
			{
				g_AudioSystem->PlayBGM("RATS_BGM_Intense");
			}
			

			bSongSwitch = true;

		}
	}
	

}

void GamePlayState::UpdateScore()
{
	m_unLastScore = 0;

	//Kill score 
	//m_pScoreNumber->SetDead();
	//m_pScoreNumber = nullptr;

	//Remake tally
	string number = "";
	number += std::to_string( gameData->m_score_current );
	//m_pScoreNumber = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), number, eRENDERCOMP_TYPE::Render2D_Text );

	// Update Score with added tally value
	RenderComp* scoreNumber = (RenderComp*)m_pScoreNumber->GetComponent( "RenderComp" );
	scoreNumber->m_renderInfo = nullptr;
	scoreNumber->SetID( number );


	// Reset tally back to 0 after score is updated
	scoreNumber = (RenderComp*)m_pTallyNumber->GetComponent( "RenderComp" );
	number = "0";
	scoreNumber->m_renderInfo = nullptr;
	scoreNumber->SetID( number );

	scoreNumber->color = XMFLOAT4(0, 0, 0, 0);
	scoreNumber = (RenderComp*)m_pTallyText->GetComponent("RenderComp");
	scoreNumber->color = XMFLOAT4(0, 0, 0, 0);

}

void GamePlayState::UpdateTally()
{
	//Update Score
	if ( gameData->m_tally_score > m_unLastScore )
	{
		m_unLastScore = *(unsigned int*)&gameData->m_tally_score;

		//Remake tally
		string number = "";
		number += std::to_string( gameData->m_tally_score );
		//m_pTallyNumber = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), number, eRENDERCOMP_TYPE::Render2D_Text );

		RenderComp* scoreNumber = (RenderComp*)m_pTallyNumber->GetComponent( "RenderComp" );
		scoreNumber->SetID( number );
		scoreNumber->m_renderInfo = nullptr;

		scoreNumber->color = XMFLOAT4(1, 1, 0, 1);
		scoreNumber = (RenderComp*)m_pTallyText->GetComponent("RenderComp");
		scoreNumber->color = XMFLOAT4(1, 1, 0, 1);

	}
}

void GamePlayState::UpdatePlayerHUD( float dt )
{
	//Update Player Health BarSize
	float playerHealth = GetHealth( m_Player );
	float playerMaxHealth = GetMaxHealth( m_Player );
	float percentLeft = playerHealth / playerMaxHealth;

	globalGraphicsPointer->SetPSPlayerHealth( percentLeft );

	RenderComp* healthFill = (RenderComp*)m_PlayerHealthFill->GetComponent( "RenderComp" );
	if ( healthFill != nullptr )
	{
		float size = (m_fPlayerHealthSize *  percentLeft);

		healthFill->m_width = size;
	}

	//Update the player health bar color and play alarm if necessary
	if ( percentLeft < 0.33f && m_fLastPlayerHealth > 0.33f )
	{
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_RATS_HEALTHLOW, &m_ObjectManager.GetCamera()->GetTransform());
		//g_AudioSystem->PlaySound( "RATS_HealthLow", m_ObjectManager.GetCamera(), true, true );
		m_pHealthLowSound->GetChannel()->setPaused(false);
		//turn health bar red
		healthFill->color = XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f );
		m_f4Color = healthFill->color;
	}

	if ( (percentLeft > 0.33 && m_fLastPlayerHealth < 0.33f) || (percentLeft < 0.66f && m_fLastPlayerHealth > 0.66f) )
	{
		//turn health bar yellow
		healthFill->color = XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f );
		m_f4Color = healthFill->color;
	}

	if ( percentLeft > 0.66f && m_fLastPlayerHealth < 0.66f )
	{
		//turn health fill green
		healthFill->color = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );
		m_f4Color = healthFill->color;
	}

	//stop alarm if health improved above 0.33
	if ( percentLeft > 0.33f && m_fLastPlayerHealth < 0.33f )
	{
		//g_AudioSystem->PostEvent(AK::EVENTS::STOP_RATS_HEALTHLOW, &m_ObjectManager.GetCamera()->GetTransform());
		m_pHealthLowSound->GetChannel()->setPaused(true);
	}

	//flash player health bar white for 100ms when player gets hit
	if ( percentLeft < m_fLastPlayerHealth && !m_bPlayerHealthFlash )
	{
		m_bPlayerHealthFlash = true;
		healthFill->color = XMFLOAT4( 2.0f, 2.0f, 2.0f, 2.0f );
	}

	if ( m_bPlayerHealthFlash )
	{
		if ( m_fFlashTimerPlayer < 0.1f )
		{
			m_fFlashTimerPlayer += dt;
		}
		else
		{
			m_fFlashTimerPlayer = 0.0f;
			m_bPlayerHealthFlash = false;
			healthFill->color = m_f4Color;
		}
	}

	m_fLastPlayerHealth = percentLeft;

	//Update Player Energy
	float playerEnergy = GetEnergy( m_Player );
	float playerMaxEnergy = GetMaxEnergy( m_Player );
	percentLeft = playerEnergy / playerMaxEnergy;

	if (percentLeft >= 0.5f)
	{
		RenderComp* emp1 = (RenderComp*)m_empIcon->GetComponent("RenderComp");
		emp1->m_width = 0.1f;
		emp1->m_height = 0.1f;
	}
	else
	{
		RenderComp* emp1 = (RenderComp*)m_empIcon->GetComponent("RenderComp");
		emp1->m_width = 0;
		emp1->m_height = 0;
	}

	if (percentLeft >= 1.0f)
	{
		RenderComp* emp2 = (RenderComp*)m_empIcon2->GetComponent("RenderComp");
		emp2->m_width = 0.1f;
		emp2->m_height = 0.1f;
	}
	else
	{
		RenderComp* emp2 = (RenderComp*)m_empIcon2->GetComponent("RenderComp");
		emp2->m_width = 0;
		emp2->m_height = 0;
	}

	RenderComp* energyFill = (RenderComp*)m_PlayerEnergyFill->GetComponent( "RenderComp" );

	if ( energyFill != nullptr )
	{
		float size = (m_fPlayerEnergySize * percentLeft);

		energyFill->m_width = size;
	}
}

void GamePlayState::UpdatePylon1HUD( float dt )
{
	if ( m_ObjectManager.GetPlayer() == nullptr )
		return;

	if (m_pArrowLamda == nullptr)
		return;

	HealthComponent* playerStats = reinterpret_cast<HealthComponent*>(m_ObjectManager.GetPlayer()->GetComponent( "HealthComponent" ));
	HealthComponent* pylonStats = reinterpret_cast<HealthComponent*>(m_Pylon1->GetComponent( "HealthComponent" ));

	if ( m_Pylon1HealthFill != nullptr )
	{
		//Update Pylon1 Health Bar
		float pylon1Health = GetHealth( m_Pylon1 );
		float pylon1MaxHealth = GetMaxHealth( m_Pylon1 );
		float percentLeft = pylon1Health / pylon1MaxHealth;
		m_fPylon1HealthPercentLeft = percentLeft;

		if ( percentLeft <= 0.1f ) //pylon disabled
		{
			//change the color of the arrow, pylon, electricity, and pylon health bar
			SetObjectColor(m_pArrowLamda, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
			SetObjectColor( m_Pylon1HealthFill, XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) );
			auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].begin();

			for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].end(); iter++ )
			{
				SetObjectColor(*iter, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
			}

			Color col = { 1.0f, 0.0f, 0.0f, 1.0f };
			m_pLightning1->GrabColor() = col;
			m_pLightning2->GrabColor() = col;
		}
		else if ( percentLeft >= 1.0f && !m_bPylon1Full ) //pylon full
		{
			if ( m_Pylon1HealthFill )
			{
				//change the color of the arrow, pylon, electricity, and pylon health bar				
				m_pArrowLamda->SetDead();
				m_pArrowLamda = nullptr;
				SetObjectColor( m_Pylon1HealthFill, XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) );
				auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].begin();

				for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].end(); iter++ )
				{
					(*iter)->SetDead();
				}


				m_pLightning1->SetDead();
				m_pLightning2->SetDead();

				//initiate mesh distort
				SpawnEMP( m_Pylon1 );

				SetObjectInvincible( m_Pylon1, true );

				m_bPylon1Full = true;
				gameData->m_pylons_alive--;

				m_Pylon1->GetChannel()->stop();

				m_AIManager->HandlePylonDeath( m_Pylon1 );

				m_fTimeSlowHelper = 2;
				//m_bTImeSlow = true;
				//g_AudioSystem->PlaySound("RATS_Bass");

				GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
				g_AudioSystem->PlaySound( "RATS_PylonDeath", tmpAudioOneshot, true );
				if ( m_ObjectManager.PassBuckets()[Pylon_Test].size() != 1 )
					globalGraphicsPointer->SetScreenWarp( true );

				gameData->m_PlayerShip.speedScaler = 1.6f;
			}
		}
		else
		{
			//set electricity back to yellow
			if ( !m_bPylon1Full )
			{

				Color col = { 1.0f, 1.0f, 0.0f, 1.0f };
				m_pLightning1->GrabColor() = col;
				m_pLightning2->GrabColor() = col;
			}

			if ( percentLeft < 0.33f && !m_bArrowLamdaRedGlow )
			{
				m_bArrowLamdaRedGlow = true;

				SetObjectColor(m_pArrowLamda, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
				SetObjectColor( m_Pylon1HealthFill, XMFLOAT4( 1.0f, 0.65f, 0.0f, 1.0f ) );
				gameData->m_PlayerShip.speedScaler = 0.7f;
			}

			if ( percentLeft > 0.33f && m_bArrowLamdaRedGlow )
			{
				m_bArrowLamdaRedGlow = false;

				SetObjectColor( m_pArrowLamda, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
				SetObjectColor( m_Pylon1HealthFill, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
				gameData->m_PlayerShip.speedScaler = 1;
			}

			if ( percentLeft > 0.65f && percentLeft < 1 )
			{
				gameData->m_PlayerShip.speedScaler = 1.3f;
				SetObjectColor( m_Pylon1HealthFill, XMFLOAT4( 0.7f, 0.7f, 1.3f, 2.0f ) );
			}

			if ( GetEnergy( m_Pylon1 ) != 0 && m_fLatch1Timer == 0 )
			{
				m_fLatch1Timer = LATCH_TIMER;
				m_pLightning1->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
				m_pLightning2->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
			}

			//Flash health bar when the pylon is hit
			if ( percentLeft < m_fLastHealth1Percent && !m_bFlashPylon1Health )
			{
				m_bFlashPylon1Health = true;
				globalGraphicsPointer->SetPylonWhite( 1, m_bFlashPylon1Health );

				//flash the arrow when the pylon is hit
				SetObjectColor( m_pArrowLamda, XMFLOAT4( 2.0f, 2.0f, 2.0f, 2.0f ) );
				m_pArrowLamda->GetTransform().SetScaleAll( 1.5f );

				auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].begin();

				for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].end(); iter++ )
				{
					SetObjectColor(*iter, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
				}
			}

			m_fLastHealth1Percent = percentLeft;

			if ( m_bFlashPylon1Health )
			{
				if ( m_fFlashTimer1 < 0.1f )
				{
					m_fFlashTimer1 += dt;
				}
				else
				{
					m_fFlashTimer1 = 0.0f;
					m_bFlashPylon1Health = false;
					m_pArrowLamda->GetTransform().SetScaleAll( 1.0f );
					globalGraphicsPointer->SetPylonWhite( 1, m_bFlashPylon1Health );

					if ( percentLeft > 0.33f )
					{
						SetObjectColor( m_pArrowLamda, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
					}
					else
					{
						SetObjectColor(m_pArrowLamda, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
					}


					auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].begin();

					for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Lambda].end(); iter++ )
					{
						SetObjectColor( *iter, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
					}
				}
			}

			//set the size of the health bar
			RenderComp* healthFill = (RenderComp*)m_Pylon1HealthFill->GetComponent( "RenderComp" );
			if ( healthFill != nullptr )
			{
				float size = (m_fPylonHealthSize *  percentLeft);

				healthFill->m_height = size;
			}
		}
	}

	// timer stuff

	if ( m_fLatch1Timer > 0 )
	{
		m_fLatch1Timer -= dt;

		if ( GetEnergy( m_Pylon1 ) > 0 )
		{
			float ratioVal = (GetEnergy( m_Pylon1 ) * (dt / LATCH_TIMER));
			pylonStats->SetHealth( pylonStats->GetHealth() + ratioVal );
			playerStats->SetEnergy( playerStats->GetEnergy() - ratioVal );
		}

		if ( m_fLatch1Timer <= 0 )
		{
			m_fLatch1Timer = 0;
			m_pLightning1->ResetTarget();
			m_pLightning2->ResetTarget();
			pylonStats->SetEnergy( 0 );

		}
		else
		{
			m_pLightning1->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
			m_pLightning2->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
		}
	}

	//Update Arrow lambda
	if ( m_bPylonLamdaAlive && m_pArrowLamda )
	{
		XMFLOAT3 playerPos = *m_Player->GetTransform().GetPosition();
		XMFLOAT3 pylonPos = *m_Pylon1->GetTransform().GetPosition();
		m_pArrowLamda->GetTransform() = m_ObjectManager.GetCamera()->GetTransform();
		m_pArrowLamda->GetTransform().SetPosition( playerPos );

		XMVECTOR vecBetween = XMLoadFloat3( &pylonPos ) - XMLoadFloat3( &playerPos );

		XMFLOAT3 newZ = *m_Player->GetTransform().GetYAxis();
		XMVECTOR newZV = XMLoadFloat3( &newZ );
		XMVECTOR newXV = XMVector3Cross( vecBetween, newZV );
		XMVECTOR newYV = XMVector3Cross( newXV, newZV );
		newXV = XMVector3Normalize( newXV );
		newYV = XMVector3Normalize( newYV );

		XMFLOAT3 finalX, finalY;
		XMStoreFloat3( &finalX, newXV );
		XMStoreFloat3( &finalY, newYV );

		m_pArrowLamda->GetTransform().SetXAxis( finalX );
		m_pArrowLamda->GetTransform().SetYAxis( finalY );
		m_pArrowLamda->GetTransform().SetZAxis( newZ );

		XMFLOAT4X4 localMatrix = m_pArrowLamda->GetTransform().GetLocalMatrix();
		XMFLOAT4X4 translationMatrix = XMFLOAT4X4( 1.0f, 0.0f, 0.0f, 0.0f,
												   0.0f, 1.0f, 0.0f, 0.0f,
												   0.0f, 0.0f, 1.0f, 0.0f,
												   0.0f, -m_fArrowOffset, 0.0f, 1.0f );

		XMMATRIX translation = XMLoadFloat4x4( &translationMatrix );
		XMMATRIX rotation = XMLoadFloat4x4( &localMatrix );

		XMMATRIX newLocal = XMMatrixMultiply( translation, rotation );

		XMStoreFloat4x4( &localMatrix, newLocal );
		m_pArrowLamda->GetTransform().SetLocalMatrix( localMatrix );
	}
}

void GamePlayState::UpdatePylon2HUD( float dt )
{

	if ( m_ObjectManager.GetPlayer() == nullptr )
		return;

	if (m_pArrowSigma == nullptr)
		return;

	HealthComponent* pylonStats = reinterpret_cast<HealthComponent*>(m_Pylon2->GetComponent( "HealthComponent" ));
	HealthComponent* playerStats = reinterpret_cast<HealthComponent*>(m_ObjectManager.GetPlayer()->GetComponent( "HealthComponent" ));


	if ( m_Pylon2HealthFill != nullptr )
	{
		//Update Pylon2 Health Bar
		float pylon2Health = GetHealth( m_Pylon2 );
		float pylon2MaxHealth = GetMaxHealth( m_Pylon2 );
		float percentLeft = pylon2Health / pylon2MaxHealth;

		m_fPylon2HealthPercentLeft = percentLeft;

		if ( percentLeft <= 0.1f )	//pylon disabled
		{
			//change the color of the arrow, pylon, electricity, and pylon health bar
			SetObjectColor(m_pArrowSigma, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
			SetObjectColor( m_Pylon2HealthFill, XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) );
			auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].begin();

			for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].end(); iter++ )
			{
				SetObjectColor( *iter, XMFLOAT4( 1.0f, 0.0f, 0.0, 1.0f ) );
			}

			Color col = { 1.0f, 0.0f, 0.0f, 1.0f };
			m_pLightning3->GrabColor() = col;
			m_pLightning4->GrabColor() = col;
		}
		else if ( percentLeft >= 1.0f && !m_bPylon2Full )		//pylon full
		{

			if ( m_Pylon2HealthFill )
			{
				//change the color of the arrow, pylon, electricity, and pylon health bar
				SetObjectColor( m_pArrowSigma, XMFLOAT4( 0.5f, 0.0f, 1.5f, 0.0f ) );
				m_pArrowSigma->SetDead();
				m_pArrowSigma = nullptr;
				SetObjectColor( m_Pylon2HealthFill, XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) );
				auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].begin();

				for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].end(); iter++ )
				{
					(*iter)->SetDead();
				}


				m_pLightning3->SetDead();
				m_pLightning4->SetDead();

				//initiate mesh distort
				SpawnEMP( m_Pylon2 );



				m_bPylon2Full = true;

				SetObjectInvincible( m_Pylon2, true );

				m_Pylon2->GetChannel()->stop();

				m_AIManager->HandlePylonDeath( m_Pylon2 );

				m_fTimeSlowHelper = 2;
				//m_bTImeSlow = true;
				//g_AudioSystem->PlaySound("RATS_Bass");
				GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
				g_AudioSystem->PlaySound( "RATS_PylonDeath", tmpAudioOneshot, true );
				if ( m_ObjectManager.PassBuckets()[Pylon_Test].size() != 1 )
					globalGraphicsPointer->SetScreenWarp( true );

				gameData->m_PlayerShip.dmgScalar = 2;

			}
		}
		else
		{
			//Set Electricity back to yellow
			if ( !m_bPylon2Full )
			{
				Color col = { 1.0f, 1.0f, 0.0f, 1.0f };
				m_pLightning3->GrabColor() = col;
				m_pLightning4->GrabColor() = col;


				//Check for low pylon health to turn on/off glow
				if ( percentLeft < 0.33f && !m_bArrowSigmaRedGlow )
				{
					m_bArrowSigmaRedGlow = true;
					SetObjectColor(m_pArrowSigma, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
					SetObjectColor( m_Pylon2HealthFill, XMFLOAT4( 1.0f, 0.65f, 0.0f, 1.0f ) );
					gameData->m_PlayerShip.dmgScalar = 0.7f;
				}

				if ( percentLeft > 0.33f && m_bArrowSigmaRedGlow )
				{
					m_bArrowSigmaRedGlow = false;
					SetObjectColor( m_pArrowSigma, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
					SetObjectColor( m_Pylon2HealthFill, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
					gameData->m_PlayerShip.dmgScalar = 1;
				}

				if ( percentLeft > 0.65f && percentLeft < 1 )
				{
					gameData->m_PlayerShip.dmgScalar = 1.5f;
					SetObjectColor( m_Pylon2HealthFill, XMFLOAT4( 0.7f, 0.7f, 1.3f, 2.0f ) );

				}

				if ( GetEnergy( m_Pylon2 ) != 0 && m_fLatch2Timer == 0 )
				{
					m_fLatch2Timer = LATCH_TIMER;
					m_pLightning3->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
					m_pLightning4->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
				}

				//flash pylon health bar when the pylon gets hit
				if ( percentLeft < m_fLastHealth2Percent && !m_bFlashPylon2Health )
				{
					m_bFlashPylon2Health = true;
					globalGraphicsPointer->SetPylonWhite( 2, m_bFlashPylon2Health );

					//flash arrow when the pylon gets hit
					SetObjectColor( m_pArrowSigma, XMFLOAT4( 2.0f, 2.0f, 2.0f, 2.0f ) );
					m_pArrowSigma->GetTransform().SetScaleAll( 1.5f );

					auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].begin();

					for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].end(); iter++ )
					{
						SetObjectColor(*iter, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
					}

				}

				m_fLastHealth2Percent = percentLeft;

				if ( m_bFlashPylon2Health )
				{
					if ( m_fFlashTimer2 < 0.1f )
					{
						m_fFlashTimer2 += dt;
					}
					else
					{
						m_fFlashTimer2 = 0.0f;
						m_bFlashPylon2Health = false;
						globalGraphicsPointer->SetPylonWhite( 2, m_bFlashPylon2Health );
						m_pArrowSigma->GetTransform().SetScaleAll( 1.0f );

						if ( percentLeft > 0.33f )
						{
							SetObjectColor( m_pArrowSigma, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
						}
						else
						{
							SetObjectColor(m_pArrowSigma, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
						}


						auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].begin();

						for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Sigma].end(); iter++ )
						{
							SetObjectColor( *iter, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
						}
					}
				}

				RenderComp* healthFill = (RenderComp*)m_Pylon2HealthFill->GetComponent( "RenderComp" );

				if ( healthFill != nullptr )
				{
					float size = (m_fPylonHealthSize *  percentLeft);

					healthFill->m_height = size;
				}
			}
		}
	}

	// timer stuff

	if ( m_fLatch2Timer > 0 )
	{
		m_fLatch2Timer -= dt;

		if ( GetEnergy( m_Pylon2 ) > 0 )
		{
			float ratioVal = (GetEnergy( m_Pylon2 ) * (dt / LATCH_TIMER));
			pylonStats->SetHealth( pylonStats->GetHealth() + ratioVal );
			playerStats->SetEnergy( playerStats->GetEnergy() - ratioVal );
		}

		if ( m_fLatch2Timer <= 0 )
		{
			m_fLatch2Timer = 0;
			m_pLightning3->ResetTarget();
			m_pLightning4->ResetTarget();
			pylonStats->SetEnergy( 0 );
		}
		else
		{
			m_pLightning3->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
			m_pLightning4->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
		}
	}

	//Update Pylon Arrow Sigma
	if ( m_bPylonSigmaAlive && m_pArrowSigma )
	{
		XMFLOAT3 playerPos = *m_Player->GetTransform().GetPosition();
		XMFLOAT3 pylonPos = *m_Pylon2->GetTransform().GetPosition();
		m_pArrowSigma->GetTransform() = m_ObjectManager.GetCamera()->GetTransform();
		m_pArrowSigma->GetTransform().SetPosition( playerPos );

		XMVECTOR vecBetween = XMLoadFloat3( &pylonPos ) - XMLoadFloat3( &playerPos );

		XMFLOAT3 newZ = *m_Player->GetTransform().GetYAxis();
		XMVECTOR newZV = XMLoadFloat3( &newZ );
		XMVECTOR newXV = XMVector3Cross( vecBetween, newZV );
		XMVECTOR newYV = XMVector3Cross( newXV, newZV );
		newXV = XMVector3Normalize( newXV );
		newYV = XMVector3Normalize( newYV );

		XMFLOAT3 finalX, finalY;
		XMStoreFloat3( &finalX, newXV );
		XMStoreFloat3( &finalY, newYV );

		m_pArrowSigma->GetTransform().SetXAxis( finalX );
		m_pArrowSigma->GetTransform().SetYAxis( finalY );
		m_pArrowSigma->GetTransform().SetZAxis( newZ );

		XMFLOAT4X4 localMatrix = m_pArrowSigma->GetTransform().GetLocalMatrix();
		XMFLOAT4X4 translationMatrix = XMFLOAT4X4( 1.0f, 0.0f, 0.0f, 0.0f,
												   0.0f, 1.0f, 0.0f, 0.0f,
												   0.0f, 0.0f, 1.0f, 0.0f,
												   0.0f, -m_fArrowOffset, 0.0f, 1.0f );

		XMMATRIX translation = XMLoadFloat4x4( &translationMatrix );
		XMMATRIX rotation = XMLoadFloat4x4( &localMatrix );

		XMMATRIX newLocal = XMMatrixMultiply( translation, rotation );

		XMStoreFloat4x4( &localMatrix, newLocal );
		m_pArrowSigma->GetTransform().SetLocalMatrix( localMatrix );
	}
}

void GamePlayState::UpdatePylon3HUD( float dt )
{

	if ( m_ObjectManager.GetPlayer() == nullptr )
		return;

	if (m_pArrowOmega == nullptr)
		return;

	HealthComponent* pylonStats = reinterpret_cast<HealthComponent*>(m_Pylon3->GetComponent( "HealthComponent" ));
	HealthComponent* playerStats = reinterpret_cast<HealthComponent*>(m_ObjectManager.GetPlayer()->GetComponent( "HealthComponent" ));

	if ( m_Pylon3HealthFill != nullptr )
	{
		//Update Pylon3 Health Bar
		float pylon3Health = GetHealth( m_Pylon3 );
		float pylon3MaxHealth = GetMaxHealth( m_Pylon3 );
		float percentLeft = pylon3Health / pylon3MaxHealth;

		m_fPylon3HealthPercentLeft = percentLeft;

		if ( percentLeft <= 0.1f )		//pylon disabled
		{
			//change the color of the arrow, pylon, electricity, and pylon health bar
			SetObjectColor(m_pArrowOmega, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
			SetObjectColor( m_Pylon3HealthFill, XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) );
			auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].begin();

			for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].end(); iter++ )
			{
				SetObjectColor( *iter, XMFLOAT4( 0.5f, 0.5f, 0.5f, 1.0f ) );
			}

			Color col = { 1.0f, 0.0f, 0.0f, 1.0f };
			m_pLightning5->GrabColor() = col;
			m_pLightning6->GrabColor() = col;
		}
		else if ( percentLeft >= 1.0f && !m_bPylon3Full )		//pylon full
		{
			if ( m_Pylon3HealthFill )
			{
				//change the color of the arrow, pylon, electricity, and pylon health bar				
				m_pArrowOmega->SetDead();
				m_pArrowOmega = nullptr;
				SetObjectColor( m_Pylon3HealthFill, XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) );
				auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].begin();

				for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].end(); iter++ )
				{
					(*iter)->SetDead();

				}


				m_pLightning5->SetDead();
				m_pLightning6->SetDead();

				//Trigger mesh distort
				SpawnEMP( m_Pylon3 );


				//Set pylon to full/invincible
				m_bPylon3Full = true;
				SetObjectInvincible( m_Pylon3, true );

				gameData->m_pylons_alive--;
				m_AIManager->HandlePylonDeath( m_Pylon3 );

				m_Pylon3->GetChannel()->stop();

				m_fTimeSlowHelper = 2;
				//m_bTImeSlow = true;
				//g_AudioSystem->PlaySound("RATS_Bass");
				GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
				g_AudioSystem->PlaySound( "RATS_PylonDeath", tmpAudioOneshot, true );
				if ( m_ObjectManager.PassBuckets()[Pylon_Test].size() != 1 )
					globalGraphicsPointer->SetScreenWarp( true );

				gameData->m_PlayerShip.resilienceScalar = 0.35f;
			}
		}
		else
		{
			//Set Electricity back to yellow
			if ( !m_bPylon3Full )
			{
				Color col = { 1.0f, 1.0f, 0.0f, 1.0f };
				m_pLightning5->GrabColor() = col;
				m_pLightning6->GrabColor() = col;



				//Check for low pylon health to turn on/off glow
				if ( percentLeft < 0.33f && !m_bArrowOmegaRedGlow )
				{
					m_bArrowOmegaRedGlow = true;
					SetObjectColor( m_pArrowOmega, XMFLOAT4( 5.0f, 0.0f, 0.0f, 5.0f ) );
					SetObjectColor( m_Pylon3HealthFill, XMFLOAT4( 1.0f, 0.65f, 0.0f, 1.0f ) );
					gameData->m_PlayerShip.resilienceScalar = 1.5f;
				}

				if ( percentLeft > 0.33f && m_bArrowOmegaRedGlow )
				{
					m_bArrowOmegaRedGlow = false;
					SetObjectColor( m_pArrowOmega, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
					SetObjectColor( m_Pylon3HealthFill, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
					gameData->m_PlayerShip.resilienceScalar = 1;
				}

				if ( percentLeft > 0.65f && percentLeft < 1 )
				{
					gameData->m_PlayerShip.resilienceScalar = 0.7f;
					SetObjectColor( m_Pylon3HealthFill, XMFLOAT4( 0.7f, 0.7f, 1.3f, 2.0f ) );
				}

				if ( GetEnergy( m_Pylon3 ) != 0 && m_fLatch3Timer == 0 )
				{
					m_fLatch3Timer = LATCH_TIMER;
					m_pLightning5->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
					m_pLightning6->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
				}

				//flash pylon health bar when the pylon gets hit
				if ( percentLeft < m_fLastHealth3Percent && !m_bFlashPylon3Health )
				{
					m_bFlashPylon3Health = true;
					globalGraphicsPointer->SetPylonWhite( 3, m_bFlashPylon3Health );
					m_pArrowOmega->GetTransform().SetScaleAll( 1.5f );
					//flash arrow when the pylon gets hit
					SetObjectColor( m_pArrowOmega, XMFLOAT4( 2.0f, 2.0f, 2.0f, 2.0f ) );

					auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].begin();

					for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].end(); iter++ )
					{
						SetObjectColor(*iter, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
					}
				}

				m_fLastHealth3Percent = percentLeft;

				if ( m_bFlashPylon3Health )
				{
					if ( m_fFlashTimer3 < 0.1f )
					{
						m_fFlashTimer3 += dt;
					}
					else
					{
						m_fFlashTimer3 = 0.0f;
						m_bFlashPylon3Health = false;
						globalGraphicsPointer->SetPylonWhite( 3, m_bFlashPylon3Health );
						m_pArrowOmega->GetTransform().SetScaleAll( 1.0f );

						if ( percentLeft > 0.33f )
						{
							SetObjectColor( m_pArrowOmega, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
						}
						else
						{
							SetObjectColor(m_pArrowOmega, XMFLOAT4(0.5f, 0.5f, 0.5f, 1));
						}


						auto iter = m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].begin();

						for ( ; iter != m_ObjectManager.PassBuckets()[eOBJECT_TYPE::Pylon_Omega].end(); iter++ )
						{
							SetObjectColor(*iter, XMFLOAT4(1, 1, 1, 1));
						}
					}
				}

				RenderComp* healthFill = (RenderComp*)m_Pylon3HealthFill->GetComponent( "RenderComp" );
				if ( healthFill != nullptr )
				{
					float size = (m_fPylonHealthSize *  percentLeft);

					healthFill->m_height = size;
				}
			}
		}
	}

	// timer stuff

	if ( m_fLatch3Timer > 0 )
	{
		m_fLatch3Timer -= dt;
		// fraction of total val by time elapsed and total time
		if ( GetEnergy( m_Pylon3 ) > 0 )
		{
			float ratioVal = (GetEnergy( m_Pylon3 ) * (dt / LATCH_TIMER));
			pylonStats->SetHealth( pylonStats->GetHealth() + ratioVal );
			playerStats->SetEnergy( playerStats->GetEnergy() - ratioVal );
		}

		if ( m_fLatch3Timer <= 0 )
		{
			m_fLatch3Timer = 0;
			m_pLightning5->ResetTarget();
			m_pLightning6->ResetTarget();
			pylonStats->SetEnergy( 0 );
		}
		else
		{
			m_pLightning5->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
			m_pLightning6->GrabTarget() = *m_ObjectManager.GetPlayer()->GetTransform().GetPosition();
		}
	}

	if ( m_bPylonOmegaAlive && m_pArrowOmega )
	{
		XMFLOAT3 playerPos = *m_Player->GetTransform().GetPosition();
		XMFLOAT3 pylonPos = *m_Pylon3->GetTransform().GetPosition();
		m_pArrowOmega->GetTransform() = m_ObjectManager.GetCamera()->GetTransform();
		m_pArrowOmega->GetTransform().SetPosition( playerPos );

		XMVECTOR vecBetween = XMLoadFloat3( &pylonPos ) - XMLoadFloat3( &playerPos );

		XMFLOAT3 newZ = *m_Player->GetTransform().GetYAxis();
		XMVECTOR newZV = XMLoadFloat3( &newZ );
		XMVECTOR newXV = XMVector3Cross( vecBetween, newZV );
		XMVECTOR newYV = XMVector3Cross( newXV, newZV );
		newXV = XMVector3Normalize( newXV );
		newYV = XMVector3Normalize( newYV );

		XMFLOAT3 finalX, finalY;
		XMStoreFloat3( &finalX, newXV );
		XMStoreFloat3( &finalY, newYV );

		m_pArrowOmega->GetTransform().SetXAxis( finalX );
		m_pArrowOmega->GetTransform().SetYAxis( finalY );
		m_pArrowOmega->GetTransform().SetZAxis( newZ );

		XMFLOAT4X4 localMatrix = m_pArrowOmega->GetTransform().GetLocalMatrix();
		XMFLOAT4X4 translationMatrix = XMFLOAT4X4( 1.0f, 0.0f, 0.0f, 0.0f,
												   0.0f, 1.0f, 0.0f, 0.0f,
												   0.0f, 0.0f, 1.0f, 0.0f,
												   0.0f, -m_fArrowOffset, 0.0f, 1.0f );

		XMMATRIX translation = XMLoadFloat4x4( &translationMatrix );
		XMMATRIX rotation = XMLoadFloat4x4( &localMatrix );

		XMMATRIX newLocal = XMMatrixMultiply( translation, rotation );

		XMStoreFloat4x4( &localMatrix, newLocal );
		m_pArrowOmega->GetTransform().SetLocalMatrix( localMatrix );
	}
}

void GamePlayState::ResetGame()
{
	//reset ALL game variables
	//m_unCurrentLevel = 0;
	//gameData->ResetScore();

	globalGraphicsPointer->m_loseText = nullptr;
	globalGraphicsPointer->renderPlayer = true;


	globalGraphicsPointer->m_fireworkPositions.clear();

	m_fOldDamageScalar = 1.0f;
	m_fOldSpeedScalar = 1.0f;
	m_fOldResilienceScalar = 1.0f;
	m_fScalarMessageTimer = 0.0f;
	m_bScalarMessageChanged = false;
	m_pStatScalarMessage = nullptr;

	m_empTimer = 0;

	m_fLevelTimer = 0.0f;

	m_fHazardTimer = 0.0f;
	m_bHazardsSpawned = false;
	m_bHazardWarningPlayed = false;
	m_vEnvironmentalHazards.clear();

	m_bPylon1Full = false;
	m_bPylon2Full = false;
	m_bPylon3Full = false;

	m_bPylonsAllFull = false;
	m_nEnemyMultiplier = 1;

	m_unCurrentWave = 0;
	m_unNumEnemiesToKill = 0;
	m_unNumEnemiesKilled = 0;
	m_unLastScore = 0;

	m_pBossHealthBack = nullptr;
	m_pBossHealthFill = nullptr;

	m_fPauseTimer = 0.0f;
	m_bWavesPaused = true;
	m_bGameWon = false;
	m_bGameLost = false;
	m_bLevelPassed = false;

	m_bGameOver = false;
	m_bResetGame = false;

	m_Pylon1 = nullptr;
	m_Pylon2 = nullptr;
	m_Pylon3 = nullptr;
	m_Player = nullptr;

	m_PlayerHealthBack = nullptr;
	m_PlayerHealthFill = nullptr;
	m_fPlayerHealthSize = 0.0;

	m_PlayerEnergyBack = nullptr;
	m_PlayerEnergyFill = nullptr;
	m_fPlayerEnergySize = 0.0f;

	m_Pylon1HealthBack = nullptr;
	m_Pylon2HealthBack = nullptr;
	m_Pylon3HealthBack = nullptr;
	m_Pylon1HealthFill = nullptr;
	m_Pylon2HealthFill = nullptr;
	m_Pylon3HealthFill = nullptr;
	m_fPylonHealthSize = 0.0f;

	m_pArrowSigma = nullptr;
	m_pArrowLamda = nullptr;
	m_pArrowOmega = nullptr;

	m_bPylonOmegaAlive = true;
	m_bPylonLamdaAlive = true;
	m_bPylonSigmaAlive = true;

	m_pLevelTimerText = nullptr;
	m_pLevelTimerNumber = nullptr;

	m_pLevelWinText = nullptr;
	m_pLevelLoseText = nullptr;

	m_pPauseTimeText = nullptr;
	m_pPauseTimeNumber = nullptr;

	m_pScoreText = nullptr;
	m_pScoreNumber = nullptr;

	m_GameWon = nullptr;
	m_GameLost = nullptr;

	m_fFlashTimer1 = 0.0f;
	m_fFlashTimer2 = 0.0f;
	m_fFlashTimer3 = 0.0f;
	m_fLastHealth1Percent = 1.0f;
	m_fLastHealth2Percent = 1.0f;
	m_fLastHealth3Percent = 1.0f;
	m_bFlashPylon1Health = false;
	m_bFlashPylon2Health = false;
	m_bFlashPylon3Health = false;

	m_pLightning1 = nullptr;		//pylon1 a.k.a. Lambda	-> Orange
	m_pLightning2 = nullptr;
	m_pLightning3 = nullptr;		//pylon2 a.k.a. Sigma	-> Purple
	m_pLightning4 = nullptr;
	m_pLightning5 = nullptr;		//pylon3 a.k.a. Omega	-> Green
	m_pLightning6 = nullptr;

	m_fLatch1Timer = 0;
	m_fLatch2Timer = 0;
	m_fLatch3Timer = 0;

	m_nLastTime = 5;

	globalGraphicsPointer->SetPlayerWhite( false );
	globalGraphicsPointer->SetPylonWhite( 1, false );
	globalGraphicsPointer->SetPylonWhite( 2, false );
	globalGraphicsPointer->SetPylonWhite( 3, false );

	gameData->m_PlayerShip.dmgScalar = 1;
	gameData->m_PlayerShip.resilienceScalar = 1;
	gameData->m_PlayerShip.speedScaler = 1;
	//gameData->m_bPlayerDead = false;

	m_ObjectManager.ResetAll();
	m_fGamepadBuff = 0;
	//m_Renderer->SetCamera(m_ObjectManager.GetCamera());

	// Set the level shape
	m_szLevelShape = m_pCurrentLevel->GetLevelShape();

	// Create the player object, world object, and pylons
	m_pWorldObject = m_ObjectManager.CreateObject( eOBJECT_TYPE::World_Obj, DirectX::XMFLOAT3( 0, 0, 0 ) );
	CreatePylons();
	m_Player = m_ObjectManager.CreateObject( eOBJECT_TYPE::Player_Obj, DirectX::XMFLOAT3( 25, 75, 0 ) );
	//m_ObjectManager.m_Molecules->BuildEmitter(m_Player, 200, "SmokeTrail");

	// Pretty internal star thing
	m_pInnerStar = m_ObjectManager.CreateObject( eOBJECT_TYPE::InnerStar, DirectX::XMFLOAT3( 0, 0, 0 ) );
	m_pInnerStar->GetTransform().SetScaleAll( 0.85f );
	m_pInnerStar->SetSteadyRotation( 1, 0.5f );

	//Set the players ground follow component
	m_Player->GetComponent<GroundFollowComponent>()->GrabFineness() = 0.001f;

	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_SILVER_CITY);
	//m_ObjectManager.m_Molecules->ResetEmitters();	// Called from ObjectManager when ResetAll takes place
	CreateScenery();
	//Spawn all the enemies in the Current Wave
	//SpawnEnemiesInWave();
	//m_pMeshDistFX = m_ObjectManager.CreateObject(VFX_Distortion, DirectX::XMFLOAT3(0, 0, 0));
	// Revert score values
	//gameData->ResetScore();
	globalGraphicsPointer->SetPlayerShieldShowing( false );
	globalGraphicsPointer->SetScreenWarp( false );
	//Create the HUD
	CreateHUD();

	m_fTimeSlowHelper = 0;

	for ( int i = 0; i < NUM_EMPS; i++ )
	{
		if(allPossibleEMPs[i].IsActive())
			allPossibleEMPs[i].Deactivate( &m_ObjectManager, numActiveEMPs );
	}
}

void GamePlayState::PauseGame( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = STATE_TYPE::HOW_TO_PLAY_STATE;
	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
	g_AudioSystem->PlaySound( "MENU_Accept" );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void GamePlayState::ReturnToMainMenu( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;

	if ( m_bGameOver )
	{
		m_NextState = STATE_TYPE::RESULT_STATE;
	}
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void GamePlayState::ExitGameToMainMenu( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;
	//m_bResetGame = true;
	if ( m_bGameOver )
	{
		m_NextState = STATE_TYPE::MAIN_MENU_STATE;

	}
	else
	{
		m_NextState = STATE_TYPE::PAUSE_MENU_STATE;
	}

	globalGraphicsPointer->SetCrosshairShowing( true );
	g_AudioSystem->PauseAllSounds();
	g_AudioSystem->DampenBGM();
}

void GamePlayState::SetupClients()
{
	using namespace Events;
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "HTP" ), this, &GamePlayState::PauseGame );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "GameOverReturn" ), this, &GamePlayState::ReturnToMainMenu );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "ExitUnsafe" ), this, &GamePlayState::ExitGameToMainMenu );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "SwapSwap" ), this, &GamePlayState::SwitchWeapon );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "EMP" ), this, &GamePlayState::FireEmp );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Weapon1" ), this, &GamePlayState::SetWeapon1 );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Weapon2" ), this, &GamePlayState::SetWeapon2 );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Weapon3" ), this, &GamePlayState::SetWeapon3 );

}

void GamePlayState::ClearClients()
{
	using namespace Events;
	EventManager()->RegisterClient( "HTP", this, &GamePlayState::PauseGame );
	EventManager()->RegisterClient( "GameOverReturn", this, &GamePlayState::ReturnToMainMenu );
	EventManager()->RegisterClient( "ExitUnsafe", this, &GamePlayState::ExitGameToMainMenu );
	EventManager()->RegisterClient( "SwapSwap", this, &GamePlayState::SwitchWeapon );
	EventManager()->RegisterClient( "EMP", this, &GamePlayState::FireEmp );
	EventManager()->RegisterClient( "Weapon1", this, &GamePlayState::SetWeapon1 );
	EventManager()->RegisterClient( "Weapon2", this, &GamePlayState::SetWeapon2 );
	EventManager()->RegisterClient( "Weapon3", this, &GamePlayState::SetWeapon3 );


}

void GamePlayState::Handle360Input()
{

	Events::CGeneralEventArgs2<unsigned char, float> tmpArgs( 0, -1.0f );
	GamePad tmpPad = gamePad->GetState();
	if ( tmpPad.buttons[buttonList::BACK] == buttonStatus::bPress )
	{
		m_NextState = STATE_TYPE::HOW_TO_PLAY_STATE;
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
		g_AudioSystem->PlaySound( "MENU_Accept" );
		g_AudioSystem->PauseAllSounds();
		g_AudioSystem->DampenBGM();

	}
	if ( tmpPad.buttons[buttonList::START] == buttonStatus::bPress )
	{
		m_NextState = STATE_TYPE::PAUSE_MENU_STATE;
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
		g_AudioSystem->PlaySound( "MENU_Accept" );
		g_AudioSystem->PauseAllSounds();
		g_AudioSystem->DampenBGM();

	}
	if ( tmpPad.buttons[buttonList::X] == buttonStatus::bPress )
	{
		SwitchWeapon( tmpArgs );

	}
	if ( tmpPad.buttons[buttonList::LEFT_BUMPER] == buttonStatus::bPress ||
		tmpPad.normTriggers[0] > 0.5f)
	{
		FireEmp( tmpArgs );
	}

	if ( tmpPad.buttons[buttonList::DPAD_LEFT] == buttonStatus::bPress )
	{
		SetWeapon1( tmpArgs );
	}

	if ( tmpPad.buttons[buttonList::DPAD_UP] == buttonStatus::bPress )
	{
		SetWeapon2( tmpArgs );
	}

	if ( tmpPad.buttons[buttonList::DPAD_RIGHT] == buttonStatus::bPress )
	{
		SetWeapon3( tmpArgs );
	}

	//prevButtons = tmpPad.state.Gamepad.wButtons;
}



void GamePlayState::SwitchWeapon( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;
	gameData->m_PlayerShip.SwitchWeapon();

	string weapon = gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select]->iconRenderStr;

	RenderComp* gunIcon = (RenderComp*)m_pCurrGunIcon->GetComponent("RenderComp");

	

	if (weapon == "iconSpreadGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.02f;
		}

	}
	else if (weapon == "iconWhipGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.08f;
		}
	}
	else
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.14f;
		}
	}

	//reinterpret_cast<RenderComp*>(m_pCurrGunIcon->GetComponent( "RenderComp" ))->SetID( gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select]->iconRenderStr );
	GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
	g_AudioSystem->PlaySound( "RATS_WeaponSwap", tmpAudioOneshot, true );
}

void GamePlayState::CreatePylons()
{
	float pillarOffset = 12;
	float lightningHeightOffset = 4;

	m_Pylon1 = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Test, DirectX::XMFLOAT3( 0, 0, 0 ) );
	m_Pylon2 = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Test, DirectX::XMFLOAT3( 0, 0, 0 ) );
	m_Pylon3 = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Test, DirectX::XMFLOAT3( 0, 0, 0 ) );



	m_Pylon1->SetSecondaryType( Pylon_Lambda );
	m_Pylon2->SetSecondaryType( Pylon_Sigma );
	m_Pylon3->SetSecondaryType( Pylon_Omega );

	m_fLatch1Timer = 0;
	m_fLatch2Timer = 0;
	m_fLatch3Timer = 0;

	if ( gameData->m_levelShape == "Torus" )
		pillarOffset = SetPylonPosTorus();
	else if (gameData->m_levelShape == "RoundedCube")
		pillarOffset = SetPylonPosCube();
	else
		pillarOffset = SetPylonPosSphere();


	GameObject* tmpPillar;
	DirectX::XMFLOAT3 tmpPos;
	DirectX::XMVECTOR vecHelp;

	// LAMBDA LEFT
	tmpPillar = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Lambda, *m_Pylon1->GetTransform().GetPosition() );
	tmpPillar->GetTransform() = m_Pylon1->GetTransform();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetPosition() );
	vecHelp -= DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( &tmpPos, vecHelp );
	tmpPillar->GetTransform().SetPosition( tmpPos );

	// LAMBDA RIGHT
	tmpPillar = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Lambda, *m_Pylon1->GetTransform().GetPosition() );
	tmpPillar->GetTransform() = m_Pylon1->GetTransform();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetPosition() );
	vecHelp += DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( &tmpPos, vecHelp );
	tmpPillar->GetTransform().SetPosition( tmpPos );


	// SIGMA LEFT
	tmpPillar = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Sigma, *m_Pylon2->GetTransform().GetPosition() );
	tmpPillar->GetTransform() = m_Pylon2->GetTransform();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetPosition() );
	vecHelp -= DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( &tmpPos, vecHelp );
	tmpPillar->GetTransform().SetPosition( tmpPos );

	// SIGMA RIGHT
	tmpPillar = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Sigma, *m_Pylon2->GetTransform().GetPosition() );
	tmpPillar->GetTransform() = m_Pylon2->GetTransform();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetPosition() );
	vecHelp += DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( &tmpPos, vecHelp );
	tmpPillar->GetTransform().SetPosition( tmpPos );

	// OMEGA LEFT
	tmpPillar = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Omega, *m_Pylon3->GetTransform().GetPosition() );
	tmpPillar->GetTransform() = m_Pylon3->GetTransform();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetPosition() );
	vecHelp -= DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( &tmpPos, vecHelp );
	tmpPillar->GetTransform().SetPosition( tmpPos );

	// OMEGA RIGHT
	tmpPillar = m_ObjectManager.CreateObject( eOBJECT_TYPE::Pylon_Omega, *m_Pylon3->GetTransform().GetPosition() );
	tmpPillar->GetTransform() = m_Pylon3->GetTransform();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetPosition() );
	vecHelp += DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( &tmpPos, vecHelp );
	tmpPillar->GetTransform().SetPosition( tmpPos );




	//Make pylon1 lightning	
	m_pLightning1 = (ElectricityEffect*)(m_ObjectManager.CreateObject( eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3( 0, 0, 0 ) ));

	Position* positions = m_pLightning1->GetPositions();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetPosition() );
	vecHelp += DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetYAxis() ) * lightningHeightOffset;
	vecHelp += DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( positions, vecHelp );
	vecHelp -= DirectX::XMLoadFloat3( m_Pylon1->GetTransform().GetXAxis() ) * pillarOffset * 2;
	DirectX::XMStoreFloat3(/*positions + m_pLightning1->GetNumSegments()*/&m_pLightning1->GrabTarget(), vecHelp );
	m_pLightning1->SetOrigLastPos(/*positions[m_pLightning1->GetNumSegments()]*/m_pLightning1->GrabTarget() );

	//Orange
	Color col = { 1.0f, 1, 0.0f, 1.0f };
	m_pLightning1->GrabColor() = col;

	m_pLightning2 = (ElectricityEffect*)(m_ObjectManager.CreateObject( eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3( 0, 0, 0 ) ));

	Position* positions2 = m_pLightning2->GetPositions();
	positions2[0] = m_pLightning1->GrabTarget();//positions[m_pLightning1->GetNumSegments()];
	/*positions2[m_pLightning2->GetNumSegments()]*/m_pLightning2->GrabTarget() = positions[0];
	m_pLightning2->GrabColor() = col;
	m_pLightning2->SetOrigLastPos(/*positions2[m_pLightning2->GetNumSegments()]*/m_pLightning2->GrabTarget() );


	//Make pylon2 lightning	
	m_pLightning3 = (ElectricityEffect*)(m_ObjectManager.CreateObject( eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3( 0, 0, 0 ) ));


	positions = m_pLightning3->GetPositions();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetPosition() );
	vecHelp += DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetYAxis() ) * lightningHeightOffset;
	vecHelp += DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( positions, vecHelp );
	vecHelp -= DirectX::XMLoadFloat3( m_Pylon2->GetTransform().GetXAxis() ) * pillarOffset * 2;
	DirectX::XMStoreFloat3(/*positions + m_pLightning3->GetNumSegments()*/&m_pLightning3->GrabTarget(), vecHelp );
	m_pLightning3->SetOrigLastPos(/*positions[m_pLightning3->GetNumSegments()]*/m_pLightning3->GrabTarget() );


	col = { 1, 1, 0.0f, 1.0f };
	m_pLightning3->GrabColor() = col;

	m_pLightning4 = (ElectricityEffect*)(m_ObjectManager.CreateObject( eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3( 0, 0, 0 ) ));

	positions2 = m_pLightning4->GetPositions();
	positions2[0] = m_pLightning3->GrabTarget();//positions[m_pLightning3->GetNumSegments()];
	/*positions2[m_pLightning4->GetNumSegments()]*/m_pLightning4->GrabTarget() = positions[0];
	m_pLightning4->GrabColor() = col;
	m_pLightning4->SetOrigLastPos(/*positions2[m_pLightning4->GetNumSegments()]*/m_pLightning4->GrabTarget() );


	//make pylon3 lightning
	m_pLightning5 = (ElectricityEffect*)(m_ObjectManager.CreateObject( eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3( 0, 0, 0 ) ));

	positions = m_pLightning5->GetPositions();
	vecHelp = DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetPosition() );
	vecHelp += DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetYAxis() ) * lightningHeightOffset;
	vecHelp += DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetXAxis() ) * pillarOffset;
	DirectX::XMStoreFloat3( positions, vecHelp );
	vecHelp -= DirectX::XMLoadFloat3( m_Pylon3->GetTransform().GetXAxis() ) * pillarOffset * 2;
	DirectX::XMStoreFloat3(/*positions + m_pLightning5->GetNumSegments()*/&m_pLightning5->GrabTarget(), vecHelp );
	m_pLightning5->SetOrigLastPos(/*positions[m_pLightning5->GetNumSegments()]*/m_pLightning5->GrabTarget() );


	col = { 1.0f, 1.0f, 0.0f, 1.0f };
	m_pLightning5->GrabColor() = col;

	m_pLightning6 = (ElectricityEffect*)(m_ObjectManager.CreateObject( eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3( 0, 0, 0 ) ));

	positions2 = m_pLightning6->GetPositions();
	positions2[0] = /*positions[m_pLightning5->GetNumSegments()]*/m_pLightning5->GrabTarget();
	/*positions2[m_pLightning6->GetNumSegments()]*/m_pLightning6->GrabTarget() = positions[0];
	m_pLightning6->GrabColor() = col;
	m_pLightning6->SetOrigLastPos(/*positions2[m_pLightning6->GetNumSegments()]*/m_pLightning6->GrabTarget() );



}

void GamePlayState::CreateScenery()
{

	static int nRatio = 0;

	float dist, randScaleRot, orbitSpeed;

	std::string tmpRenderINfo;

	float colX, colY, colZ;

	XMFLOAT3 newPos, orbitAxis;
	XMVECTOR toCenter;

	GameObject* tmpObj;

	for ( int i = 0; i < 10; ++i ) m_UniformDist( m_RandEngine );

	for ( int i = 0; i < NUM_SCENERY; ++i )
	{
		do
		{
			newPos = XMFLOAT3( m_UniformDist( m_RandEngine ), m_UniformDist( m_RandEngine ), m_UniformDist( m_RandEngine ) );
		} while ( XMVector3Length( XMLoadFloat3( &newPos ) ).m128_f32[0] <= 0 );
		XMStoreFloat3( &newPos, XMVector3Normalize( XMLoadFloat3( &newPos ) ) );

		if ( nRatio > 3 )
		{
			tmpRenderINfo = GrabRandomPlanetInfo();

			dist = (((m_UniformDist( m_RandEngine ) * 0.5f) + 1.0f) * 75.0f) + 585;

			newPos.x *= dist;
			newPos.y *= dist;
			newPos.z *= dist;

			nRatio = 0;
		}
		else
		{
			tmpRenderINfo = GrabRandomAsteroidInfo();

			dist = (rand() % 2 ? 685 : 385) + (((m_UniformDist( m_RandEngine ) * 0.5f) + 1) * 100) * nRatio;
			newPos.x *= dist;
			newPos.y *= dist;
			newPos.z *= dist;

			++nRatio;
		}

		tmpObj = m_ObjectManager.CreateObject( Scenery_Obj, newPos, tmpRenderINfo );

		randScaleRot = (m_UniformDist( m_RandEngine ) * 0.25f) + 0.75f;
		orbitSpeed = 0;
		if ( nRatio == 0 )
		{
			tmpObj->GetTransform().SetScaleAll( randScaleRot * 0.7f );
			tmpObj->SetSteadyRotation( rand() % 3, randScaleRot * 0.1f );
			orbitSpeed = 0.001f * (m_UniformDist( m_RandEngine ) + 1.01f);
		}
		else
		{
			tmpObj->GetTransform().SetScaleAll( randScaleRot * 0.4f );
			tmpObj->SetSteadyRotation( rand() % 3, randScaleRot * 0.2f );
			orbitSpeed = 0.005f * (m_UniformDist( m_RandEngine ) + 1.01f);
		}


		toCenter = XMLoadFloat3( tmpObj->GetTransform().GetPosition() );
		XMStoreFloat3( &orbitAxis, XMVector3Normalize( XMVector3Cross( toCenter, XMVector3Cross( toCenter, XMLoadFloat3( &XMFLOAT3( 0, 1, 0 ) ) ) ) ) );

		tmpObj->SetSteadyOrbit( DirectX::XMFLOAT3( 0, 0, 0 ), orbitAxis, XM_PI * orbitSpeed );


		colX = (((m_UniformDist( m_RandEngine ) * 0.5f) + 1) * 6) * 0.1f + 0.8f;
		colY = (((m_UniformDist( m_RandEngine ) * 0.5f) + 1) * 6) * 0.1f + 0.8f;
		colZ = (((m_UniformDist( m_RandEngine ) * 0.5f) + 1) * 6) * 0.1f + 0.8f;
		reinterpret_cast<RenderComp*>(tmpObj->GetComponent( "RenderComp" ))->color = DirectX::XMFLOAT4( colX, colY, colZ, 1 );

	}
}

std::string GamePlayState::GrabRandomAsteroidInfo()
{
	int randHelp = rand() % 5;

	// 	randHelp += 2;
	// 	if (randHelp == NUM_SCENERY)
	// 	{
	// 		randHelp = 0;
	// 	}
	// 	if (randHelp > NUM_SCENERY)
	// 	{
	// 		randHelp = 1;
	// 	}

	switch ( randHelp )
	{
		case 0:
			return "Asteroid01_Plain";
			break;

		case 1:
			return "Asteroid01_Cyan";
			break;

		case 2:
			return "Asteroid01_Red";
			break;

		case 3:
			return "Asteroid02_Ice";
			break;

		case 4:
			return "Asteroid02_Rock";
			break;


	}

	return "";
}

std::string GamePlayState::GrabRandomPlanetInfo()
{
	int randHelp = rand() % 10;


	switch ( randHelp )
	{

		case 0:
			return "Planet01";
			break;

		case 1:
			return "Planet02";
			break;

		case 2:
			return "Planet03_Pink";
			break;

		case 3:
			return "Planet03_Road";
			break;

		case 4:
			return "Planet03_Black";
			break;

		case 5:
			return "Planet04_Gas";
			break;

		case 6:
			return "Planet04_Grass";
			break;

		case 7:
			return "Planet04_Fire";
			break;

		case 8:
			return "Planet04_Ice";
			break;

		case 9:
			return "Planet04_Sepia";
			break;

	}

	return "";
}



void GamePlayState::CreateTornado( Transform trans )
{
	GameObject* tornado = m_ObjectManager.CreateObject( eOBJECT_TYPE::Env_Tornado, *trans.GetPosition(), "Tornado" );
	tornado->GetTransform() = trans;
	m_vEnvironmentalHazards.push_back( tornado );

	m_ObjectManager.m_Molecules->BuildEmitter( tornado, 200, "Tornado" );
}

void GamePlayState::CreateProminence( Transform trans )
{
	GameObject* prominence = m_ObjectManager.CreateObject( eOBJECT_TYPE::Env_Prominence, *trans.GetPosition(), "Prominence" );
	prominence->GetTransform() = trans;
	m_ObjectManager.m_Molecules->BuildEmitter(prominence, 200, "ProminenceMolecule");
	m_vEnvironmentalHazards.push_back(prominence);
}

void GamePlayState::CreateSingularity( Transform trans )
{
	GameObject* singularity = m_ObjectManager.CreateObject( eOBJECT_TYPE::Env_Singularity, *trans.GetPosition(), "Singularity" );
	singularity->GetTransform() = trans;
	m_vEnvironmentalHazards.push_back( singularity );
	MoleculeEmitter *single =
		m_ObjectManager.m_Molecules->BuildEmitter( singularity, 250, "Singularity Effect" );
	single->m_targetPosition = *singularity->GetTransform().GetPosition();
}


bool GamePlayState::CheckForPylonHealthFull()
{
	if ( m_fPylon1HealthPercentLeft >= 1.0f && m_fPylon2HealthPercentLeft >= 1.0f && m_fPylon3HealthPercentLeft >= 1.0f )
	{
		m_bPylonsAllFull = true;
	}
	else
	{
		m_bPylonsAllFull = false;
	}

	return m_bPylonsAllFull;
}

void GamePlayState::UpdateEnvironmentalHazards( float dt )
{
	int numToMake = 0;
	if (m_pCurrentLevel->GetNumLevel() + 1 < 5)
	{
		numToMake = m_pCurrentLevel->GetNumLevel() + 1;
	}
	else
	{
		numToMake = 5;
	}
	
	if ( m_vEnvironmentalHazards.size() < ((UINT)numToMake) )
	{
		m_fHazardTimer += dt;

		if ( !m_bHazardWarningPlayed )
		{
			if ( m_fHazardTimer > 3.5f )
			{
				if ( !m_bHazardWarningSpawned )
				{
					m_tEnvHazTransform = m_Player->GetTransform();
					m_tEnvHazTransform.SetLocalMatrix( TranslateTargetBack( 20.0f, m_tEnvHazTransform ) );
					m_ObjectManager.m_Molecules->BuildPosEmitter( *m_tEnvHazTransform.GetPosition(), 100, "Hazard Warning" );
					m_bHazardWarningSpawned = true;
				}
				else
				{
					m_bHazardWarningPlayed = true;
					m_bHazardWarningSpawned = false;
				}
			}

		}
		else if ( m_fHazardTimer > 5.0f && !m_bHazardsSpawned && m_bHazardWarningPlayed )
		{
			m_bHazardsSpawned = true;

			int hazardType = m_vEnvironmentalHazards.size();

			switch ( hazardType )
			{
				case 0:
				{
						  CreateTornado( m_tEnvHazTransform );
				}
					break;
				case 1:
				{
					CreateTornado(m_tEnvHazTransform);						 
				}
					break;
				case 2:
				{
					CreateTornado(m_tEnvHazTransform);
				}
					break;
				case 3:
				{
					CreateSingularity(m_tEnvHazTransform);
				}
					break;
				case 4:
				{
					CreateProminence(m_tEnvHazTransform);
				}
					break;
				default:
					break;
			}

			m_fHazardTimer = 0.0f;
		}
		else if ( m_fHazardTimer > 5.0f && m_bHazardsSpawned && m_bHazardWarningPlayed )
		{
			m_bHazardsSpawned = false;

			//for (unsigned int i = 0; i < m_vEnvironmentalHazards.size(); i++)
			//{
			//	m_vEnvironmentalHazards[i]->SetDead();
			//}

			//m_vEnvironmentalHazards.clear();
			m_fHazardTimer = 0.0f;
			m_bHazardWarningPlayed = false;
		}
	}
}

void GamePlayState::SetObjectColor( GameObject* obj, XMFLOAT4 color )
{
	RenderComp* renderComp = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( renderComp )
	{
		renderComp->color = color;
	}
}

void GamePlayState::SetObjectInvincible( GameObject* obj, bool invincible )
{
	HealthComponent* healthComp = (HealthComponent*)obj->GetComponent( "HealthComponent" );
	if ( healthComp != nullptr )
	{
		healthComp->SetInvincibility( invincible );
	}
}

XMFLOAT4X4 GamePlayState::TranslateTargetBack( float offset, Transform trans )
{

	XMFLOAT4X4 localMatrix = trans.GetLocalMatrix();
	XMFLOAT4X4 translationMatrix = XMFLOAT4X4( 1.0f, 0.0f, 0.0f, 0.0f,
											   0.0f, 1.0f, 0.0f, 0.0f,
											   0.0f, 0.0f, 1.0f, 0.0f,
											   offset, 0.0f, offset, 1.0f );

	XMMATRIX translation = XMLoadFloat4x4( &translationMatrix );
	XMMATRIX rotation = XMLoadFloat4x4( &localMatrix );

	XMMATRIX newLocal = XMMatrixMultiply( translation, rotation );

	XMStoreFloat4x4( &localMatrix, newLocal );
	return localMatrix;
}

void GamePlayState::UpdateLevelTimer( float dt )
{
	if (m_bLevelPassed)
	{
		return;
	}

	m_fLevelTimer += dt;
	string number = "";
	if ( m_fLevelTimer < 60.0f )
	{
		int seconds = (int)m_fLevelTimer;

		if ( m_fLevelTimer < 10.0f )
		{
			number += "0:0";
			number += std::to_string( seconds );
		}
		else
		{
			number += "0:";
			number += std::to_string( seconds );

		}
	}
	else
	{
		int minutes = (int)(m_fLevelTimer / 60.0f);
		int seconds = (int)(m_fLevelTimer - (minutes * 60.0f));
		number += std::to_string( minutes );
		number += ":";

		if ( seconds < 10 )
		{
			number += "0";
		}

		number += std::to_string( seconds );
	}

	if (number != m_oldTimerNumber)
	{
		m_oldTimerNumber = number;

		RenderComp* levelTimer = (RenderComp*)m_pLevelTimerNumber->GetComponent("RenderComp");
		if (levelTimer)
		{
			levelTimer->SetID(number);
			levelTimer->m_renderInfo = nullptr;
		}
	}


}

float GamePlayState::SetPylonPosSphere()
{

	m_Pylon2->GetTransform().SetPosition( DirectX::XMFLOAT3( 0, 75, 0 ) );

	//Rotate Pylons to sit correctly on the sphere
	m_Pylon1->GetTransform().RotateBy( 2, 120, 1, true );
	//m_Pylon1->GetTransform().RotateBy(0, 90, 1, true);
	DirectX::XMFLOAT3 posHelp = *m_Pylon1->GetTransform().GetYAxis();
	posHelp.x *= 75;
	posHelp.y *= 75;
	posHelp.z *= 75;
	m_Pylon1->GetTransform().SetPosition( posHelp );

	m_Pylon3->GetTransform().RotateBy( 2, -120, 1, true );
	//m_Pylon3->GetTransform().RotateBy(0, -90, 1, true);
	posHelp = *m_Pylon3->GetTransform().GetYAxis();
	posHelp.x *= 75;
	posHelp.y *= 75;
	posHelp.z *= 75;
	m_Pylon3->GetTransform().SetPosition( posHelp );

	return 12;
}

float GamePlayState::SetPylonPosTorus()
{

	m_Pylon2->GetTransform().RotateBy( 1, 120, 1, true );
	m_Pylon3->GetTransform().RotateBy( 1, 240, 1, true );

	m_Pylon1->GetTransform().RotateBy( 0, -90, 1, true );
	m_Pylon2->GetTransform().RotateBy( 0, -90, 1, true );
	m_Pylon3->GetTransform().RotateBy( 0, -90, 1, true );

	DirectX::XMFLOAT3 posHelp = *m_Pylon1->GetTransform().GetYAxis();
	posHelp.x *= 160;
	posHelp.y *= 160;
	posHelp.z *= 160;
	m_Pylon1->GetTransform().SetPosition( posHelp );

	posHelp = *m_Pylon2->GetTransform().GetYAxis();
	posHelp.x *= 160;
	posHelp.y *= 160;
	posHelp.z *= 160;
	m_Pylon2->GetTransform().SetPosition( posHelp );

	posHelp = *m_Pylon3->GetTransform().GetYAxis();
	posHelp.x *= 160;
	posHelp.y *= 160;
	posHelp.z *= 160;
	m_Pylon3->GetTransform().SetPosition( posHelp );

	m_Pylon1->GetTransform().RotateBy( 1, -90, 1, true );
	m_Pylon2->GetTransform().RotateBy( 1, -90, 1, true );
	m_Pylon3->GetTransform().RotateBy( 1, -90, 1, true );


	return 15;
}


float GamePlayState::SetPylonPosCube()
{
	m_Pylon2->GetTransform().SetPosition(DirectX::XMFLOAT3(0, 75, 0));


	m_Pylon1->GetTransform().RotateBy(2, 90, 1, true);
	//m_Pylon1->GetTransform().RotateBy(0, 90, 1, true);
	DirectX::XMFLOAT3 posHelp = *m_Pylon1->GetTransform().GetYAxis();
	posHelp.x *= 75;
	posHelp.y *= 85;
	posHelp.z *= 75;
	m_Pylon1->GetTransform().SetPosition(posHelp);

	m_Pylon3->GetTransform().RotateBy(0, -90, 1, true);
	//m_Pylon3->GetTransform().RotateBy(0, -90, 1, true);
	posHelp = *m_Pylon3->GetTransform().GetYAxis();
	posHelp.x *= 85;
	posHelp.y *= 75;
	posHelp.z *= 75;
	m_Pylon3->GetTransform().SetPosition(posHelp);


	return 12;
}


SCALAR_TYPE GamePlayState::StatScalarChanged()
{
	if ( gameData->m_PlayerShip.speedScaler != m_fOldSpeedScalar )
	{
		return SCALAR_TYPE::SPEED;
	}

	if ( gameData->m_PlayerShip.resilienceScalar != m_fOldResilienceScalar )
	{

		return SCALAR_TYPE::RESILIENCE;
	}

	if ( gameData->m_PlayerShip.dmgScalar != m_fOldDamageScalar )
	{
		return SCALAR_TYPE::DAMAGE;
	}

	return SCALAR_TYPE::NONE;

}

void GamePlayState::UpdateScalarMessageHUD( float dt )
{
	if ( StatScalarChanged() != SCALAR_TYPE::NONE )
	{
		m_bScalarMessageChanged = true;
	}

	if ( m_bScalarMessageChanged )
	{
		string message = "";
		switch ( StatScalarChanged() )
		{
			case NONE:
				break;
			case SPEED:
			{
						  if ( gameData->m_PlayerShip.speedScaler > m_fOldSpeedScalar )
						  {
							  message += "Speed up to ";
							  int percent = (int)(gameData->m_PlayerShip.speedScaler * 100);
							  message += to_string( percent );
							  message += "%";
						  }
						  else
						  {
							  message += "Speed down to ";
							  int percent = (int)(gameData->m_PlayerShip.speedScaler * 100);
							  message += to_string( percent );
							  message += "%";
						  }

						  m_fOldSpeedScalar = gameData->m_PlayerShip.speedScaler;

						  RenderComp* messageComp = (RenderComp*)m_pStatScalarMessage->GetComponent( "RenderComp" );
						  if ( messageComp )
						  {
							  messageComp->SetID( message );
							  SetObjectColor( m_pStatScalarMessage, XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) );
							  messageComp->m_renderInfo = nullptr;
						  }
			}
				break;
			case RESILIENCE:
			{
							   if ( gameData->m_PlayerShip.resilienceScalar > m_fOldResilienceScalar )
							   {
								   message += "Resilience down to ";
								   int percent = (int)((100 - (gameData->m_PlayerShip.resilienceScalar * 100)) + 100);
								   message += to_string( percent );
								   message += "%";
							   }
							   else
							   {
								   message += "Resilience up to ";
								   int percent = (int)((100 - (gameData->m_PlayerShip.resilienceScalar * 100)) + 100);
								   message += to_string( percent );
								   message += "%";
							   }

							   m_fOldResilienceScalar = gameData->m_PlayerShip.resilienceScalar;

							   RenderComp* messageComp = (RenderComp*)m_pStatScalarMessage->GetComponent( "RenderComp" );
							   if ( messageComp )
							   {
								   messageComp->SetID( message );
								   SetObjectColor( m_pStatScalarMessage, XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) );
								   messageComp->m_renderInfo = nullptr;
							   }
			}
				break;
			case DAMAGE:
			{
						   if ( gameData->m_PlayerShip.dmgScalar > m_fOldDamageScalar )
						   {
							   message += "Damage up to ";
							   int percent = (int)(gameData->m_PlayerShip.dmgScalar * 100);
							   message += to_string( percent );
							   message += "!";
						   }
						   else
						   {
							   message += "Damage down to ";
							   int percent = (int)(gameData->m_PlayerShip.dmgScalar * 100);

							   message += to_string( percent );
							   message += "%";
						   }

						   m_fOldDamageScalar = gameData->m_PlayerShip.dmgScalar;

						   RenderComp* messageComp = (RenderComp*)m_pStatScalarMessage->GetComponent( "RenderComp" );
						   if ( messageComp )
						   {
							   messageComp->SetID( message );
							   SetObjectColor( m_pStatScalarMessage, XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) );
							   messageComp->m_renderInfo = nullptr;
						   }
			}
				break;	
			default:
				break;
		}

		m_bScalarMessageChanged = false;
		m_fScalarMessageTimer = 0.0f;
	}

	m_fScalarMessageTimer += dt;

	if ( m_fScalarMessageTimer >= 3.0f )
	{
		string message = "";
		RenderComp* messageComp = (RenderComp*)m_pStatScalarMessage->GetComponent( "RenderComp" );
		if ( messageComp )
		{
			messageComp->SetID( message );
			SetObjectColor( m_pStatScalarMessage, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
			messageComp->m_renderInfo = nullptr;
		}
		m_fScalarMessageTimer = 0.0f;
	}

}

void GamePlayState::CreateScalarMessageObjects( void )
{
	string message = "";

	m_pStatScalarMessage = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), message, eRENDERCOMP_TYPE::Render2D_Text );

	RenderComp* scalarMessage = (RenderComp*)m_pStatScalarMessage->GetComponent( "RenderComp" );

	if ( scalarMessage != nullptr )
	{
		scalarMessage->m_posX = 0.4f;
		scalarMessage->m_posY = 0.025f;
		scalarMessage->m_height = 0.2f;
		scalarMessage->m_width = 0.125f;
	}

	m_fOldDamageScalar = gameData->m_PlayerShip.dmgScalar;
	m_fOldResilienceScalar = gameData->m_PlayerShip.resilienceScalar;
	m_fOldSpeedScalar = gameData->m_PlayerShip.speedScaler;
}

void GamePlayState::UpdateGameOver( float dt )
{
	// Change this timer to a different float
	// Need to modify ObjectManager to keep player object in bucket (camera depends on it)
	m_fPauseTimer += dt;
	if ( m_fPauseTimer > m_fPauseTimeLeft )
	{
		// Real Game Over
	}

	else
	{
		m_ObjectManager.Update( dt );			// Still update objects for the moment
	}
}

void GamePlayState::UpdateLevelClear(float dt)
{
	m_fLevelClearTimer -= dt;
	
	if(m_fLevelClearTimer < 0.0f)
	{
		// 
		m_unCurrentWave = 0;
		//m_unCurrentLevel++;

		RenderComp* levelWinText = (RenderComp*)m_pLevelWinText->GetComponent("RenderComp");
		if (levelWinText)
		{
			levelWinText->color = XMFLOAT4(1, 1, 1, 0);
		}

		gameData->m_mult_timer = -0.1f;
		gameData->Update( 0.10f );
		g_AudioSystem->StopAllSounds();
		g_AudioSystem->StopBGM();
		m_NextState = RESULT_STATE;
		m_bPylonsAllFull = false;
		gameData->m_bResetGame = true;

	}

	else if (m_fLevelClearTimer > 2.0f)
	{
		// Change this for spawning all emitters, timer them to pop out every 2 ish seconds(?)
		
		//m_ObjectManager.Update(dt);

		RenderComp* levelWinText = (RenderComp*)m_pLevelWinText->GetComponent("RenderComp");
		if (levelWinText)
		{
			levelWinText->color = XMFLOAT4(1, 1, 1, 1);
		}

		static float shotTimer = 0;
		shotTimer += dt;

		if (shotTimer >= 0.2f)
		{
			shotTimer = 0;

			int expIndex = rand() % globalGraphicsPointer->m_fireworkPositions.size();

			XMFLOAT3 expPos = globalGraphicsPointer->m_fireworkPositions[expIndex];

			m_Player->GetComponent<HealthComponent>()->SetMaxHealth(10000000);


			GameObject* tmpAudio = m_ObjectManager.CreateObject(Audio_OneShot, expPos);
			g_AudioSystem->PlaySound("RATS_EnemyDeath", tmpAudio);

			int colorIndex = rand() % 7;

			if (colorIndex == 0)
			{
				MoleculeEmitter* explosion = m_ObjectManager.m_Molecules->BuildPosEmitter(expPos, 400, "Purple Fire Explosion");

			}
			else if (colorIndex == 1)
			{
				MoleculeEmitter* explosion = m_ObjectManager.m_Molecules->BuildPosEmitter(expPos, 400, "Cyan Fire Explosion");

			}
			else if (colorIndex == 2)
			{
				MoleculeEmitter* explosion = m_ObjectManager.m_Molecules->BuildPosEmitter(expPos, 400, "Red Fire Explosion");

			}
			else if (colorIndex == 3)
			{
				MoleculeEmitter* explosion = m_ObjectManager.m_Molecules->BuildPosEmitter(expPos, 400, "Orange Orange Fire Explosion");

			}
			else if (colorIndex == 4)
			{
				MoleculeEmitter* explosion = m_ObjectManager.m_Molecules->BuildPosEmitter(expPos, 400, "Yellow Fire Explosion");

			}
			else if (colorIndex == 5)
			{
				MoleculeEmitter* explosion = m_ObjectManager.m_Molecules->BuildPosEmitter(expPos, 400, "Pink Fire Explosion");

			}
			else if (colorIndex == 6)
			{
				MoleculeEmitter* explosion = m_ObjectManager.m_Molecules->BuildPosEmitter(expPos, 400, "Green Fire Explosion");

			}

		}
	}
}

void GamePlayState::SetWeapon1( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	gameData->m_PlayerShip.SwitchWeapon( 0 );
	string weapon = gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select]->iconRenderStr;

	RenderComp* gunIcon = (RenderComp*)m_pCurrGunIcon->GetComponent("RenderComp");



	if (weapon == "iconSpreadGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.02f;
		}
	}
	else if (weapon == "iconWhipGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.08f;
		}
	}
	else
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.14f;
		}
	}
	GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
	g_AudioSystem->PlaySound( "RATS_WeaponSwap", tmpAudioOneshot, true );
}

void GamePlayState::SetWeapon2( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	gameData->m_PlayerShip.SwitchWeapon( 1 );
	string weapon = gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select]->iconRenderStr;

	RenderComp* gunIcon = (RenderComp*)m_pCurrGunIcon->GetComponent("RenderComp");



	if (weapon == "iconSpreadGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.02f;
		}
	}
	else if (weapon == "iconWhipGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.08f;
		}
	}
	else
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.14f;
		}
	}
	GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
	g_AudioSystem->PlaySound( "RATS_WeaponSwap", tmpAudioOneshot, true );
}

void GamePlayState::SetWeapon3( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	gameData->m_PlayerShip.SwitchWeapon( 2 );
	string weapon = gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select]->iconRenderStr;

	RenderComp* gunIcon = (RenderComp*)m_pCurrGunIcon->GetComponent("RenderComp");



	if (weapon == "iconSpreadGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.02f;
		}
	}
	else if (weapon == "iconWhipGun")
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.08f;
		}
	}
	else
	{
		if (gunIcon != nullptr)
		{
			gunIcon->m_posX = 0.14f;
		}
	}
	GameObject* tmpAudioOneshot = m_ObjectManager.CreateObject( Audio_OneShot, POS_ORIGIN );
	g_AudioSystem->PlaySound( "RATS_WeaponSwap", tmpAudioOneshot, true );
}

void GamePlayState::Pulse()
{
	MoleculeEmitter* tmpEmitter;
	std::list<GameObject*>::iterator tmpIter;


	// LAMBDA - BLUE

	if (m_pArrowLamda == nullptr)
	{
		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(m_ObjectManager.GetPlayer(), 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(0.3f, 0.6f, 1, 1);
	}
	else
	{
		tmpIter = m_ObjectManager.PassBuckets()[Pylon_Lambda].begin();

		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(*tmpIter, 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(0.3f, 0.6f, 1, 1);

		tmpIter++;

		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(*tmpIter, 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(0.3f, 0.6f, 1, 1);
	}

	//////////////////////////////////////////////////////////////////////////

	// Sigma - Red

	if (m_pArrowSigma == nullptr)
	{
		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(m_ObjectManager.GetPlayer(), 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(1, 0.3f, 0.3f, 1);
	}
	else
	{
		tmpIter = m_ObjectManager.PassBuckets()[Pylon_Sigma].begin();

		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(*tmpIter, 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(1, 0.3f, 0.3f, 1);

		tmpIter++;

		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(*tmpIter, 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(1, 0.3f, 0.3f, 1);
	}

	//////////////////////////////////////////////////////////////////////////

	// OMEGA - GREEN

	if (m_pArrowOmega == nullptr)
	{
		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(m_ObjectManager.GetPlayer(), 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(0.3f, 1, 0.2f, 1);
	}
	else
	{
		tmpIter = m_ObjectManager.PassBuckets()[Pylon_Omega].begin();

		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(*tmpIter, 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(0.3f, 1, 0.2f, 1);

		tmpIter++;

		tmpEmitter = m_ObjectManager.m_Molecules->BuildEmitter(*tmpIter, 100, "Pulse");
		tmpEmitter->m_endColor = tmpEmitter->m_startColor = DirectX::XMFLOAT4(0.3f, 1, 0.2f, 1);
	}

	//////////////////////////////////////////////////////////////////////////

}

void GamePlayState::UpdateBossHud(float dt)
{
	if (m_pBossHealthFill == nullptr)
		return;

	if (m_ObjectManager.PassBuckets()[Enemy_Boss].size() == 0)
	{

		SetObjectColor(m_pBossHealthFill, XMFLOAT4(0, 0, 0, 0));
		SetObjectColor(m_pBossHealthBack, XMFLOAT4(0, 0, 0, 0));


		return;
	}
		

	GameObject* bossObj = *m_ObjectManager.PassBuckets()[Enemy_Boss].begin();

	

	HealthComponent* bossHealth = bossObj->GetComponent<HealthComponent>();

	float percent = bossHealth->GetHealth() / bossHealth->GetMaxHealth();

	if (percent > 1)
		percent = 1;
	if (percent < 0)
		percent = 0;

	RenderComp* bossRender = m_pBossHealthFill->GetComponent<RenderComp>();

	bossRender->m_width = fBossHealthSize * percent;

	if (bossObj->GetFlag(eSTATUS_FLAGS::Spwaning))
		return;

	if (bossHealthBarColor.z == 0 && !bossHealth->GetInvincibility())
	{
		bossRender->color = bossHealthBarColor = XMFLOAT4(1, 0, 0.8f, 1);
	}
	else if (bossHealthBarColor.z == 0.8f && bossHealth->GetInvincibility())
	{
		bossRender->color = bossHealthBarColor = XMFLOAT4(1, 0, 0, 1);
	}

}

void GamePlayState::EMP::Activate( Transform& epicenterTrans, ObjectManager* objMngr, int& toIncr )
{
	TransformAxes[0] = *epicenterTrans.GetXAxis();
	TransformAxes[1] = *epicenterTrans.GetYAxis();
	TransformAxes[2] = *epicenterTrans.GetZAxis();

	m_pos = *epicenterTrans.GetPosition();

	Color col = { 1, 1, 0, 1 };
	for ( unsigned int currElec = 0; currElec < NUM_EMP_ELECS; ++currElec )
	{
		allElecs[currElec] = (ElectricityEffect*)objMngr->CreateObject( eOBJECT_TYPE::VFX_Lightning, *epicenterTrans.GetPosition() );
		allElecs[currElec]->SetMaxToWait( 0.01f );
		allElecs[currElec]->GetPositions()[0] = m_pos;
		allElecs[currElec]->GrabColor() = col;
	}

	m_bActive = true;
	m_fcurrRadius = 0;
	++toIncr;
}

void GamePlayState::EMP::Update( ObjectManager* objMngr, float dt, uniform_real_distribution<float>& URDist, default_random_engine& RandEng, int& numEMPsActive )
{
	if ( m_fcurrRadius >= m_fMaxRadius )
	{
		Deactivate( objMngr, numEMPsActive );
		return;
	}

	m_fcurrRadius += m_fRadiusIncrSpeed * dt;

	int elecsLeft = NUM_EMP_ELECS - 1;
	int currbucket;
	list<GameObject*>::iterator currEnemy;
	std::vector<list<GameObject*>>& buckets = objMngr->PassBuckets();

	Color lockedOn = { 1, 0, 1, 1 };
	Color notLocked = { 1, 1, 0, 1 };

	XMFLOAT3 currEnemyPos;
	XMVECTOR toEnemyPos;

	currbucket = EvilBullet_Simple;
	while ( currbucket != Arrow_Sigma )
	{
		currEnemy = buckets[currbucket].begin();

		while ( currEnemy != buckets[currbucket].end() )
		{
			currEnemyPos = *((*currEnemy)->GetTransform().GetPosition());
			toEnemyPos = XMLoadFloat3( &currEnemyPos ) - XMLoadFloat3( &m_pos );

			if ( XMVector3LengthSq( toEnemyPos ).m128_f32[0] < m_fcurrRadius * m_fcurrRadius )
			{
				allElecs[elecsLeft]->GrabColor() = lockedOn;
				allElecs[elecsLeft]->GrabTarget() = currEnemyPos;
				(*currEnemy)->SetDead();

				--elecsLeft;

				if ( elecsLeft < 0 ) return;
			}
			++currEnemy;
		}

		++currbucket;
	}

	currbucket = Enemy_Merc;
	while ( currbucket != Player_Obj )
	{
		currEnemy = buckets[currbucket].begin();

		while ( currEnemy != buckets[currbucket].end() )
		{
			currEnemyPos = *((*currEnemy)->GetTransform().GetPosition());
			toEnemyPos = XMLoadFloat3( &currEnemyPos ) - XMLoadFloat3( &m_pos );

			if ( XMVector3LengthSq( toEnemyPos ).m128_f32[0] < m_fcurrRadius * m_fcurrRadius )
			{
				allElecs[elecsLeft]->GrabColor() = lockedOn;
				allElecs[elecsLeft]->GrabTarget() = currEnemyPos;
				(*currEnemy)->TakeDamage( 500 * dt );

				--elecsLeft;

				if ( elecsLeft < 0 ) return;
			}

			++currEnemy;
		}

		++currbucket;
	}

	XMVECTOR usedX = XMLoadFloat3( &TransformAxes[0] ), usedY = XMLoadFloat3( &TransformAxes[1] ), usedZ = XMLoadFloat3( &TransformAxes[2] );
	while ( elecsLeft >= 0 )
	{
		XMStoreFloat3( &currEnemyPos,
					   (XMVector3Normalize( (usedX * URDist( RandEng )) + (usedY * ((URDist( RandEng ) * 0.5f) + 1) * 0.25f) + (usedZ * URDist( RandEng )) ) * m_fcurrRadius) + XMLoadFloat3( &m_pos ) );
		allElecs[elecsLeft]->GrabColor() = notLocked;
		allElecs[elecsLeft]->GrabTarget() = currEnemyPos;

		--elecsLeft;
	}

}

void GamePlayState::EMP::Deactivate( ObjectManager* objMngr, int& toDecr )
{
	for ( unsigned int currElec = 0; currElec < NUM_EMP_ELECS; ++currElec )
	{
		allElecs[currElec]->SetDead( true );
		allElecs[currElec] = nullptr;
	}

	m_bActive = false;
	--toDecr;
}