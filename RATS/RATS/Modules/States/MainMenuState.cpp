#include "stdafx.h"
#include "MainMenuState.h"
#include "../Input/InputController.h"
#include "../Renderer/D3DGraphics.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../Upgrade System/GameData.h"
extern InputController* m_controller;
extern D3DGraphics* globalGraphicsPointer;
extern	FFmod*		g_AudioSystem;
extern ThreeSixty* gamePad;
extern GameData* gameData;


MainMenuState::MainMenuState()
{
	dwPacketNo = 0;
}


MainMenuState::~MainMenuState()
{
}

void MainMenuState::Enter( BitmapFontManager* bitmapFontManager, bool resetSound )
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList( m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets() );
	m_Renderer->SetRenderSizes( m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes() );

	m_unCurrentButton = 0;

	if ( m_controller )
		m_controller->SetMenuKeys();
	using namespace Events;
	//Setup Input
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Confirm" ), this, &MainMenuState::OnEnter );

	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Return" ), this, &MainMenuState::OnEscape );

	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Up" ), this, &MainMenuState::OnArrowUp );

	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Down" ), this, &MainMenuState::OnArrowDown );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "UpArr" ), this, &MainMenuState::OnArrowUp );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "DownArr" ), this, &MainMenuState::OnArrowDown );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "MouseLClick" ), this, &MainMenuState::OnMouseClick );

	m_NextState = MAIN_MENU_STATE;
	//if (resetSound)
	//{
	//	g_AudioSystem->PostEvent(AK::EVENTS::STOP_ALL);
	//	g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MX_MUSICLOOP_02);
	//}

	dwPacketNo = gamePad->GetState().state.dwPacketNumber;

	//g_AudioSystem->PostEvent(AK::EVENTS::STOP_ALL);
	//g_AudioSystem->PostEvent(AK::EVENTS::STOP_RATS_BG_MENU);
	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_RATS_BG_MENU);
	g_AudioSystem->StopAllSounds();
	g_AudioSystem->ResetListener();
	if (g_AudioSystem->IsBGMDampened())
		g_AudioSystem->UnDampenBGM();
	g_AudioSystem->PlayBGM( "RATS_BGM_Menu" );

	MessageSystem::GetInstance()->Call<void, float&, float&>( "GetMousePos", fMousePrevX, fMousePrevY );

}

void MainMenuState::Exit()
{


	using namespace Events;

	EventManager()->UnregisterClient( "Confirm", this, &MainMenuState::OnEnter );
	EventManager()->UnregisterClient( "Return", this, &MainMenuState::OnEscape );
	EventManager()->UnregisterClient( "Up", this, &MainMenuState::OnArrowUp );
	EventManager()->UnregisterClient( "Down", this, &MainMenuState::OnArrowDown );
	EventManager()->UnregisterClient( "UpArr", this, &MainMenuState::OnArrowUp );
	EventManager()->UnregisterClient( "DownArr", this, &MainMenuState::OnArrowDown );
	EventManager()->UnregisterClient( "MouseLClick", this, &MainMenuState::OnMouseClick );
	if ( m_controller )
		m_controller->ClearMenuKeys();

}

void MainMenuState::Initialize( Renderer* renderer )
{

	m_Renderer = renderer;

	//Initialize the ObjectManager and the ObjectFactory
	m_ObjectManager.Initialize();// GamePlay();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory( &m_ObjectFactory );

	//Create the menu interface
	CreateHud();

	m_vButtons.clear();
	////Set Up Buttons
	m_vButtons.push_back( 0 );
	m_vButtons.push_back( 1 );
	m_vButtons.push_back( 2 );
	m_vButtons.push_back( 3 );
	m_vButtons.push_back( 4 );

}

void MainMenuState::Update( float dt )
{
	// Keybaord
	if (!gameData->m_bUsingGamepad)
	{
		fCheckTimer -= dt;
		if (fCheckTimer <= 0)
		{
			fCheckTimer = 0.1f;
			CheckMouseMove();
		}

		
	}
	
	// GAMEPAD STUFF
	else
	{
		fGamepadTimer -= dt;
		if (fGamepadTimer <= 0)
		{
			//if (gamePad->GetState().state.dwPacketNumber != dwPacketNo)
			{
				//dwPacketNo = gamePad->GetState().state.dwPacketNumber;
				//bStickHeld = false;
				if(Handle360Input())
					fGamepadTimer = 0.1f;

				globalGraphicsPointer->SetCrosshairShowing(false);
				//fStickHeldTimer = 0;
			}
// 			else if (bStickHeld)
// 			{
// 				fStickHeldTimer += dt;
// 				if (fStickHeldTimer > 0.4f)
// 					Handle360Input();
// 				fGamepadTimer = 0.1f;
// 			}
// 			else if (gamePad->GetState().normLS[1] != 0)
// 			{
// 				bStickHeld = true;
// 				//fGamepadTimer = 0.3f;
// 			}

		}
	}
	if (_finite(fGamepadTimer) == 0)
	{
		std::cout << "GAMEPAD TIMER IS GARBAGE VAL!!\n";
		fGamepadTimer = 0;
	}
	
	SetSelectButton();

	//switch out the menu navigation instructions if GamepadInstructions are showing and the mouse/keyboard are being used
	if ( globalGraphicsPointer->GetCrosshairShowing() && m_bGamepadInstructionsOn )
	{
		m_bGamepadInstructionsOn = false;
		
		RenderComp* pButton = (RenderComp*)m_pMenuNavGamepadInstr->GetComponent("RenderComp");
		if (pButton)
		{
			pButton->m_height = 0.0f;
			pButton->m_width = 0.0f;
		}
		
		pButton = (RenderComp*)m_pMenuNavKeyboardInstr->GetComponent("RenderComp");
		if (pButton)
		{
			pButton->m_height = 0.34f;
			pButton->m_width = 0.375f;
		}	
	}

	//switch out the menu navigation instructions if KeyboardInstructions are showing and the Gamepad is being used
	if ( !globalGraphicsPointer->GetCrosshairShowing() && !m_bGamepadInstructionsOn )
	{
		m_bGamepadInstructionsOn = true;
		RenderComp* pButton = (RenderComp*)m_pMenuNavKeyboardInstr->GetComponent("RenderComp");
		if (pButton)
		{
			pButton->m_height = 0.0f;
			pButton->m_width = 0.0f;
		}

		pButton = (RenderComp*)m_pMenuNavGamepadInstr->GetComponent("RenderComp");
		if (pButton)
		{
			pButton->m_height = 0.34f;
			pButton->m_width = 0.375f;
		}

	}


}

void MainMenuState::Terminate()
{
	m_vButtons.clear();
}

void MainMenuState::CreateHud( void )
{

	// Text for Whip Speed
	m_gameName = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "RatsAlpha", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* gameComp = (RenderComp*)m_gameName->GetComponent("RenderComp");

	if (gameComp != nullptr)
	{
		gameComp->m_posX = 0.6f;
		gameComp->m_posY = 0.15f;
		gameComp->m_height = 0.4f;
		gameComp->m_width = 0.4f;

		gameComp->color = XMFLOAT4(1, 1, 1, 1);
	}

	//Create the menu background
	m_pMenuBackground = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "menuBackground", eRENDERCOMP_TYPE::Render2D_Back );
	RenderComp* menuBackground = (RenderComp*)m_pMenuBackground->GetComponent( "RenderComp" );
	if ( menuBackground != nullptr )
	{
		menuBackground->m_posX = 0.0f;
		menuBackground->m_posY = 0.0f;
		menuBackground->m_height = 1.0f;
		menuBackground->m_width = 1.0f;
	}

	//Create the New Game Button
	m_NewGameButton = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "newGameButton", eRENDERCOMP_TYPE::Render2D_Front );
	RenderComp* newGameButton = (RenderComp*)m_NewGameButton->GetComponent( "RenderComp" );
	if (newGameButton != nullptr)
	{


		newGameButton->m_posX = 0.266f;
		newGameButton->m_posY = 0.245f;
		newGameButton->m_height = 0.175f;
		newGameButton->m_width = 0.325f;
	}

	m_NewGameButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "newGameButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	newGameButton = (RenderComp*)m_NewGameButtonSelected->GetComponent("RenderComp");
	if (newGameButton != nullptr)
	{
		newGameButton->m_posX = 0.266f;
		newGameButton->m_posY = 0.245f;
		newGameButton->m_height = 0.0f;
		newGameButton->m_width = 0.0f;
	}

	//Create the Load Game Button
	m_LoadGameButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "loadGameButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* loadGameButton = (RenderComp*)m_LoadGameButton->GetComponent("RenderComp");
	if (loadGameButton != nullptr)
	{

		loadGameButton->m_posX = 0.266f;
		loadGameButton->m_posY = 0.08f;
		loadGameButton->m_height = 0.175f;
		loadGameButton->m_width = 0.325f;
	}

	m_LoadGameButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "loadGameButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	loadGameButton = (RenderComp*)m_LoadGameButtonSelected->GetComponent("RenderComp");
	if (loadGameButton != nullptr)
	{
		loadGameButton->m_posX = 0.266f;
		loadGameButton->m_posY = 0.08f;
		loadGameButton->m_height = 0.175f;
		loadGameButton->m_width = 0.325f;

	}

	//Create the How to Play Button
	m_HowToPlayButton = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "howToPlayButton", eRENDERCOMP_TYPE::Render2D_Front );
	RenderComp* howToPlayButton = (RenderComp*)m_HowToPlayButton->GetComponent( "RenderComp" );
	if ( howToPlayButton != nullptr )
	{
		howToPlayButton->m_posX = 0.266f;
		howToPlayButton->m_posY = 0.42f;
		howToPlayButton->m_height = 0.175f;
		howToPlayButton->m_width = 0.325f;
	}

	m_HowToPlayButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "howToPlayButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	howToPlayButton = (RenderComp*)m_HowToPlayButtonSelected->GetComponent("RenderComp");
	if (howToPlayButton != nullptr)
	{
		howToPlayButton->m_posX	= 0.266f;
		howToPlayButton->m_posY = 0.42f;
		howToPlayButton->m_height = 0.0f;
		howToPlayButton->m_width = 0.0f;
	}

	//Create the Options Button
	m_OptionsButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "optionsButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* optionsButton = (RenderComp*)m_OptionsButton->GetComponent("RenderComp");
	if (optionsButton != nullptr)
	{
		optionsButton->m_posX = 0.266f;
		optionsButton->m_posY = 0.595f;
		optionsButton->m_height = 0.175f;
		optionsButton->m_width = 0.325f;
	}

	m_OptionsButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "optionsButtonSelect", eRENDERCOMP_TYPE::Render2D_Front);
	optionsButton = (RenderComp*)m_OptionsButtonSelected->GetComponent("RenderComp");
	if (optionsButton != nullptr)
	{
		optionsButton->m_posX = 0.266f;
		optionsButton->m_posY = 0.595f;
		optionsButton->m_height = 0.0f;
		optionsButton->m_width = 0.0f;
	}
	

	//Create the Quit Button
	m_QuitButton = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "quitButton", eRENDERCOMP_TYPE::Render2D_Front );
	RenderComp* quitButton = (RenderComp*)m_QuitButton->GetComponent( "RenderComp" );

	if ( quitButton != nullptr )
	{
		quitButton->m_posX = 0.266f;
		quitButton->m_posY = 0.77f;
		quitButton->m_height = 0.175f;
		quitButton->m_width = 0.325f;
	}

	m_QuitButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "quitButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	quitButton = (RenderComp*)m_QuitButtonSelected->GetComponent("RenderComp");

	if (quitButton != nullptr)
	{
		quitButton->m_posX = 0.266f;
		quitButton->m_posY = 0.77f;
		quitButton->m_height = 0.0f;
		quitButton->m_width = 0.0f;
	}
	//Create the Select Button
	m_SelectButton = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "selectButton", Render2D_Front );
	RenderComp* selectButton = (RenderComp*)m_SelectButton->GetComponent( "RenderComp" );

	if ( selectButton != nullptr )
	{
		selectButton->m_posX = 0.225f;
		selectButton->m_posY = 0.15f;
		selectButton->m_height = 0.1f;
		selectButton->m_width = 0.1f;
		selectButton->color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 0.0f );
	}

	//White square behind navigation

	m_whiteSquare = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "WhiteSquare", eRENDERCOMP_TYPE::Render2D_Middle);
	RenderComp* whiteSq = (RenderComp*)m_whiteSquare->GetComponent("RenderComp");

	if (whiteSq != nullptr)
	{
		whiteSq->m_posX = 0.625f;
		whiteSq->m_posY = 0.63f;
		whiteSq->m_height = 0.34f;
		whiteSq->m_width = 0.375f;

		whiteSq->color = XMFLOAT4(1, 1, 1, 0.0f);
	}


	//Create the Navigation instructions
	if ( globalGraphicsPointer->GetCrosshairShowing() )
	{
		m_pMenuNavKeyboardInstr = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "menuNavKeyboardInstr", eRENDERCOMP_TYPE::Render2D_Front );
		RenderComp* menuNavKeyboardInstr = (RenderComp*)m_pMenuNavKeyboardInstr->GetComponent( "RenderComp" );

		if ( menuNavKeyboardInstr != nullptr )
		{
			menuNavKeyboardInstr->m_posX = 0.625f;
			menuNavKeyboardInstr->m_posY = 0.63f;
			menuNavKeyboardInstr->m_height = 0.34f;
			menuNavKeyboardInstr->m_width = 0.375f;

			menuNavKeyboardInstr->color = XMFLOAT4(1, 2.0f, 2.0f, 1);

		}

		m_pMenuNavGamepadInstr = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "menuNavGamepadInstr", eRENDERCOMP_TYPE::Render2D_Front);
		RenderComp* menuNavGamepadInstr = (RenderComp*)m_pMenuNavGamepadInstr->GetComponent("RenderComp");

		if (menuNavGamepadInstr != nullptr)
		{
			menuNavGamepadInstr->m_posX = 0.625f;
			menuNavGamepadInstr->m_posY = 0.63f;
			menuNavGamepadInstr->m_height = 0.0f;
			menuNavGamepadInstr->m_width = 0.0f;

			menuNavGamepadInstr->color = XMFLOAT4(1, 2.0f, 2.0f, 1);

		}		
	}
	else
	{
		m_pMenuNavKeyboardInstr = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "menuNavKeyboardInstr", eRENDERCOMP_TYPE::Render2D_Front);
		RenderComp* menuNavKeyboardInstr = (RenderComp*)m_pMenuNavKeyboardInstr->GetComponent("RenderComp");

		if (menuNavKeyboardInstr != nullptr)
		{
			menuNavKeyboardInstr->m_posX = 0.625f;
			menuNavKeyboardInstr->m_posY = 0.63f;
			menuNavKeyboardInstr->m_height = 0.0f;
			menuNavKeyboardInstr->m_width = 0.0f;

			menuNavKeyboardInstr->color = XMFLOAT4(1, 2.0f, 2.0f, 1);

		}

		m_pMenuNavGamepadInstr = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "menuNavGamepadInstr", eRENDERCOMP_TYPE::Render2D_Front );
		RenderComp* menuNavGamepadInstr = (RenderComp*)m_pMenuNavGamepadInstr->GetComponent( "RenderComp" );

		if ( menuNavGamepadInstr != nullptr )
		{
			menuNavGamepadInstr->m_posX = 0.625f;
			menuNavGamepadInstr->m_posY = 0.63f;
			menuNavGamepadInstr->m_height = 0.34f;
			menuNavGamepadInstr->m_width = 0.375f;

			menuNavGamepadInstr->color = XMFLOAT4(1, 2.0f, 2.0f, 1);

		}

		m_bGamepadInstructionsOn = true;
	}

}


void MainMenuState::SetCurrentButton( int numButton )
{
	if ( numButton > (int)(m_vButtons.size() - 1) )
	{
		m_unCurrentButton = 0;
	}
	else if ( numButton < 0 )
	{
		m_unCurrentButton = m_vButtons.size() - 1;
	}
	else
	{
		m_unCurrentButton = numButton;
	}
}

void MainMenuState::SetSelectButton( void )
{
	ShrinkAllButtons();

	RenderComp* selectButton = (RenderComp*)m_SelectButton->GetComponent( "RenderComp" );

	switch ( m_unCurrentButton )
	{
		case 0:
		{

			GrowSelectedButton(m_LoadGameButtonSelected);

			m_SelectedState = STATE_TYPE::LEVEL_SELECT_STATE;
		}
			break;
		case 1:
		{

			// New Game Button
			if (selectButton != nullptr)
			{
				selectButton->m_posY = 0.095f;
			}

			GrowSelectedButton(m_NewGameButtonSelected);
			m_SelectedState = LEVEL_SELECT_STATE;
			// Since this is a new game, reset level and upgrade things

			//m_SelectedState = STATE_TYPE::GAME_PLAY_STATE;

				  
		}
			break;
		case 2:
		{
				  if ( selectButton != nullptr )
				  {
					  selectButton->m_posY = 0.44f;
				  }

				  GrowSelectedButton(m_HowToPlayButtonSelected);

				  m_SelectedState = STATE_TYPE::HOW_TO_PLAY_STATE;
		}
			break;
		case 3:
		{ 
			GrowSelectedButton(m_OptionsButtonSelected);
			gameData->m_OptionReturnState = STATE_TYPE::MAIN_MENU_STATE;
			m_SelectedState = STATE_TYPE::OPTIONS_MENU_STATE;


		}
			break;
		case 4:
		{
				  if ( selectButton != nullptr )
				  {
					  selectButton->m_posY = 0.79f;
				  }

				  GrowSelectedButton(m_QuitButtonSelected);

				  m_SelectedState = STATE_TYPE::EXIT_GAME;
		}
			break;
		default:
			break;
	}
}

void MainMenuState::OnArrowUp( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton( m_unCurrentButton - 1 );
	g_AudioSystem->PlaySound( "MENU_Hover" );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void MainMenuState::OnArrowDown( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton( m_unCurrentButton + 1 );
	g_AudioSystem->PlaySound( "MENU_Hover" );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void MainMenuState::OnEnter( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = m_SelectedState;
	if ( m_unCurrentButton == 1 )	// new game
	{
		gameData->ResetGame();
		gameData->m_cur_level = 0;
	}

	if ( m_unCurrentButton == 4 )
	{
		gameData->SaveGame();
	}


	g_AudioSystem->PlaySound( "MENU_Accept" );

	globalGraphicsPointer->SetCrosshairShowing( true );

}

void MainMenuState::OnEscape( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;

	m_unCurrentButton = m_vButtons.size() - 1;
	g_AudioSystem->PlaySound( "MENU_Hover" );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

bool MainMenuState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>( "GetMousePos", fMouseX, fMouseY );

	if ( fMouseX == fMousePrevX && fMouseY == fMousePrevY )
		return false;



	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_NewGameButton->GetComponent( "RenderComp" );
	if ( !pButton )
		return false;

	if ( !globalGraphicsPointer )
		return false;

	globalGraphicsPointer->SetCrosshairShowing( true );
	fMousePrevX = fMouseX;
	fMousePrevY = fMouseY;

	GetClientRect( globalGraphicsPointer->GetWindow(), &clientRect );

	fMouseX = fMouseX / (clientRect.right - clientRect.left);
	fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

	// Check Buttons - NewGame


	if ( fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		 fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height )
	{
		if ( m_unCurrentButton != 1 )
			g_AudioSystem->PlaySound( "MENU_Hover" );

		m_unCurrentButton = 1;
		return true;
	}

	//Check Buttons Options
	pButton = (RenderComp*)m_OptionsButton->GetComponent( "RenderComp" );
	if ( !pButton )
		return false;

	if ( fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		 fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height )
	{
		if ( m_unCurrentButton != 3 )
			g_AudioSystem->PlaySound( "MENU_Hover" );

		m_unCurrentButton = 3;
		return true;
	}

	// Check Buttons- HTP
	pButton = (RenderComp*)m_HowToPlayButton->GetComponent( "RenderComp" );
	if ( !pButton )
		return false;

	if ( fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		 fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height )
	{
		if ( m_unCurrentButton != 2 )
			g_AudioSystem->PlaySound( "MENU_Hover" );

		m_unCurrentButton = 2;
		return true;
	}

	//Check Buttons- Load Game
	pButton = (RenderComp*)m_LoadGameButton->GetComponent( "RenderComp" );
	if ( !pButton )
		return false;

	if ( fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		 fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height )
	{
		if ( m_unCurrentButton != 0 )
			g_AudioSystem->PlaySound( "MENU_Hover" );

		m_unCurrentButton = 0;
		return true;
	}

	// Check Buttons- Exit
	pButton = (RenderComp*)m_QuitButton->GetComponent( "RenderComp" );
	if ( !pButton )
		return false;

	if ( fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		 fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height )
	{
		if ( m_unCurrentButton != 4 )
			g_AudioSystem->PlaySound( "MENU_Hover" );

		m_unCurrentButton = 4;
		return true;
	}



	return false;
}

void MainMenuState::OnMouseClick( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if (gameData->m_bUsingGamepad)
		return;

	if ( CheckMouseMove() || globalGraphicsPointer->GetCrosshairShowing() )
	{
		SetSelectButton();
		m_NextState = m_SelectedState;
		if ( m_unCurrentButton == 1 )
		{
			gameData->ResetGame();
			gameData->m_cur_level = 0;
		}

		if ( m_unCurrentButton == 4 )
		{
			gameData->SaveGame();
		}

		
		g_AudioSystem->PlaySound( "MENU_Accept" );
		
	}

}

bool MainMenuState::Handle360Input()
{

	GamePad tmpPad = gamePad->GetState();
	bool detectedInput = false;

	//if (tmpPad.normMag > 0.75f)
	
		if (tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bPress || 
			tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bHeld  ||
			tmpPad.buttons[buttonList::DPAD_UP]		 == buttonStatus::bPress ||
			tmpPad.buttons[buttonList::DPAD_UP] == buttonStatus::bHeld )
		{
			SetCurrentButton(m_unCurrentButton - 1);
			g_AudioSystem->PlaySound("MENU_Hover");
			detectedInput = true;
		}

		if (tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bPress ||
			tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bHeld  ||
			tmpPad.buttons[buttonList::DPAD_DOWN]	   == buttonStatus::bPress ||
			tmpPad.buttons[buttonList::DPAD_DOWN]	   == buttonStatus::bHeld)
		{
			SetCurrentButton(m_unCurrentButton + 1);
			g_AudioSystem->PlaySound("MENU_Hover");
			detectedInput = true;
		}
	

	

	if ( tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		 tmpPad.buttons[buttonList::START] == buttonStatus::bPress )
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound( "MENU_Accept" );

		if ( m_unCurrentButton == 1 )
		{
			gameData->ResetGame();
			gameData->m_cur_level = 0;
		}

		if ( m_unCurrentButton == 4 )
		{
			gameData->SaveGame();
		}

		detectedInput = true;

	}

	if (tmpPad.buttons[buttonList::B] == buttonStatus::bPress)
	{
		if (m_unCurrentButton != 4)
		{
			m_unCurrentButton = 4;
			g_AudioSystem->PlaySound("MENU_Back");

		}


		detectedInput = true;
	}

	return detectedInput;

}

void MainMenuState::ShrinkAllButtons()
{
	RenderComp* pButton = (RenderComp*)m_NewGameButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_HowToPlayButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_QuitButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_LoadGameButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_OptionsButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}
}

void MainMenuState::GrowSelectedButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.175f;
		pButton->m_width = 0.325f;
	}
}