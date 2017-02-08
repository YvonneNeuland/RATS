#include "stdafx.h"
#include "Game.h"
#include "Wwise files/Event.h"

#include "Wwise files/EventManager.h"
#include "Wwise files/Transform.h"
#include "Dependencies/Paramesium/Include/steam_api.h"
#include "Modules/Input/InputManager.h"
#include "Modules/Input/MessageSystem.h"
#include "Modules/Input/InputController.h"
//#include "Modules/Input/ObjectRegistry.h"
//#include "Wwise files/AudioSystemWwise.h"
#include "Modules/Achievements/AchManager.h"
#include "Modules/Input/ThreeSixty.h"
#include "Modules/Audio/FFmod.h"
// Grants Testing Includes
//#include "Modules/Object Manager/ObjectFactory.h"
//#include "Modules/Upgrade System/GameData.h"

extern ThreeSixty* gamePad;
extern FFmod*	g_AudioSystem;
extern InputController* m_controller;
extern CInputManager* m_inputMGR;
extern XTime* m_Xtime;
extern GameData *gameData;
extern CAchManager* g_SteamAchievements;

Game::Game()
{
}

Game::~Game()
{
	//Shutdown();
}

void Game::Initialize(HINSTANCE hInstance)
{
	using namespace Events;
	m_Xtime = new XTime;
	m_Xtime->Restart();
	// Start up Steam Services
	// - Apparenly we need to init Steam before the renderer, otherwise 
	// - the DLL won't attach to the process properly.  

	if (!SteamAPI_Init())
	{

		// 3 Reasons why this wouldn't work
		// A. Steam Client Isn't Running
		// B. Steam Client cant determine the AppID.  Make sure Steam_appid.txt is in the Dir.
		// C. App isn't running under same User context as Client, including Admin priviliges

		std::cout << "SteamAPI_Init() Failed!\n";

	}
	// Steam is up, let's check for the user
	else
	{
		if (SteamUser() != 0)
		{
			if (!SteamUser()->BLoggedOn())
			{
				std::cout << "Steam User not logged in\n";

			}
			// So we found the user, and we're logged in, now let's get them achievements loaded
			else
			{
				g_SteamAchievements = new CAchManager(g_Achievements, 9, g_Stats, 6);
			}
		}
	}

	


	// Now that we tried to run steam, we can init everything else.
	
	

	CEventManager::CreateInstance();
	MessageSystem::CreateInstance();



	m_inputMGR = new CInputManager;
	m_controller = new InputController;//= m_objects->CreateModule("Controller");
	
	
	m_inputMGR->Init();
	m_controller->Init();
	//m_controller->SetMenuKeys();
	//m_controller->SetDefaultKeys();
	//m_input->
	// TESTING THE OTHER MESSAGE SYS
	// Press
	
	//MessageSystem::GetInstance()->RegisterMessage<
	//ms->RegisterMessage<CInputManager, EVENTID, const unsigned char, EVENTID>("BindKeyPressEvent", this, &CInputManager::BindKeyPressEvent);
	

	m_Renderer = new Renderer;
	m_Renderer->Initialize(hInstance);

	gamePad = new ThreeSixty();
	gameData = new GameData();
	gameData->Initialize();
	gamePad->Initialize();

//  	g_AudioSystem = new AudioSystemWwise;
//  	g_AudioSystem->Initialize();
//  	g_AudioSystem->SetBasePath(L"./Dependencies/Wwise/SoundBanks/");
//  	g_AudioSystem->LoadSoundBank(L"Init.bnk");
//  	g_AudioSystem->LoadSoundBank(L"SoundBank.bnk");
//  	g_AudioSystem->SetWorldScale(4);
	g_AudioSystem = new FFmod;
	g_AudioSystem->Init();
	LoadSounds();

	m_StateManager = new StateManager;
// 	g_AudioSystem->SetRTCPValue(AK::GAME_PARAMETERS::MX_VOLUME, 50);
// 	g_AudioSystem->SetRTCPValue(AK::GAME_PARAMETERS::FX_VOLUME, 60);
	m_StateManager->Initialize(m_Renderer);

	//g_AudioSystem->SetChannelVolume(2, 20);
	
	//Transform newTrns;
	//
	//newTrns.SetPosition(DirectX::XMFLOAT3(0, 0, 0));
	//newTrns.SetLocalMatrix(DirectX::XMFLOAT4X4());
	//g_AudioSystem.RegisterListener(&newTrns, "Main Camera");
	////g_AudioSystem.RegisterEntity(&m_Entity2, "Main Entity");
	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MX_MUSICLOOP_01);
	////g_AudioSystem.PostEvent(AK::EVENTS::PLAY_FX_3D_CAR_LOOP, &m_Entity2);


#if 0
	// Testing Grant things
	// ***************************************
	


	// ***************************************
#endif
	
}

bool Game::Run(MSG uMsg)
{
	m_Xtime->Signal();
	float dt = (float)m_Xtime->Delta();
	if (m_StateManager->IsTimeSlow())
		dt *= 0.3f;

	Events::EventManager()->ProcessEvents();
	m_Renderer->Frame(dt);
	m_inputMGR->Update(dt);
	gamePad->Update(dt);
	if (!m_StateManager->Update(dt))
		return false;
	g_AudioSystem->Update(dt);

	if (SteamAPI_IsSteamRunning())
		SteamAPI_RunCallbacks();

	return true;
}

void Game::Shutdown()
{
	m_Renderer->Shutdown();
	m_StateManager->Terminate();
	delete m_Renderer;

	g_AudioSystem->Shutdown();
	delete g_AudioSystem;
	m_controller->ShutDown();
	m_inputMGR->ShutDown();
	delete gamePad;
	delete m_controller;
	delete m_inputMGR;
	delete m_StateManager;
	delete m_Xtime;
	delete gameData;
	Events::EventManager()->Shutdown();
	Events::CEventManager::DestroyInstance();
	Events::MessageSystem::DestroyInstance();
	SteamAPI_Shutdown();
	if (g_SteamAchievements)
		delete g_SteamAchievements;
	
}


// TESTING THE STARTUP PROJ EVENT AND MSG SYSTEMS
void Game::Test(const Events::CGeneralEventArgs<float>& args)
{
	std::cout << "Test Successful!!\n";
}

void Game::LoadSounds()
{
	g_AudioSystem->LoadSound("MENU_Accept", FMOD_3D);
	g_AudioSystem->LoadSound("MENU_Back", FMOD_3D);
	g_AudioSystem->LoadSound("MENU_Hover", FMOD_3D);
	g_AudioSystem->LoadSound("MENU_Error", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_Bass", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_Dash", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_EMP", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_EnemyDeath", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_EnemyHit", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_EnemySpawn", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_EvilBullet", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_HealthLow", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadSound("RATS_MissileCharge", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_MissileLoop", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadSound("RATS_MissileRush", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_MissilePlayer", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_Pickup", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_PlayerDeath", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_PlayerHit", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_PylonCharging", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_PylonDeath", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_PylonIdle", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadSound("RATS_SpreadBulletFire", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_WeaponSwap", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_WaveTransition", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_Whip", FMOD_3D | FMOD_LOOP_NORMAL);

	g_AudioSystem->LoadStream("RATS_BGM_Normal_01", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadStream("RATS_BGM_Normal_02", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadStream("RATS_BGM_Normal_03", FMOD_3D | FMOD_LOOP_NORMAL);
	//g_AudioSystem->LoadStream("RATS_BGM_Normal_04", FMOD_3D | FMOD_LOOP_NORMAL);

	g_AudioSystem->LoadStream("RATS_BGM_Intense", FMOD_3D | FMOD_LOOP_NORMAL);
	//g_AudioSystem->LoadStream("RATS_BGM_Intense_03", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadStream("RATS_BGM_Menu", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadStream("RATS_BGM_Results", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadStream("RATS_BGM_Boss", FMOD_3D | FMOD_LOOP_NORMAL);
	g_AudioSystem->LoadStream("RATS_BGM_Credits", FMOD_3D | FMOD_LOOP_NORMAL);

	g_AudioSystem->LoadSound("RATS_GameWon", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_GameOver", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_MineAlert", FMOD_3D);
	g_AudioSystem->LoadSound("RATS_DisruptorLeech", FMOD_3D );

}
