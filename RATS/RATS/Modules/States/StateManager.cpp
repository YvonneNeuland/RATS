#include "stdafx.h"
#include "StateManager.h"
#include "SplashScreenState.h"
#include "MainMenuState.h"
#include "GamePlayState.h"
#include "MainMenuState.h"
#include "GamePlayState.h"
#include "LoadMenuState.h"
#include "OptionsMenuState.h"
#include "PauseMenuState.h"
#include "CreditsState.h"
#include "HowToPlayState.h"
#include "SoundOptionState.h"
#include "GraphicsOptionsState.h"
#include "KeybindingsOptionsState.h"
#include "ResultState.h"
#include "UpgradeShipState.h"
#include "LevelSelectState.h"
#include "../Object Manager/CameraObject.h"
#include "../Renderer/D3DGraphics.h"
#include "../Input/InputManager.h"
#include "../Upgrade System/GameData.h"
extern D3DGraphics* globalGraphicsPointer;
extern CInputManager* m_inputMGR;
extern GameData* gameData;

StateManager::StateManager()
{

}

StateManager::~StateManager()
{
}

bool StateManager::SetState(STATE_TYPE stateType)
{

	bool bPauseSounds = true;

	m_inputMGR->ResetInput();
	globalGraphicsPointer->FadeOut();

	//Exit the current state
	m_vStateStack[m_CurrentState]->Exit();

	if (gameData->m_bResetGame)
	{
		gameData->m_bResetGame = false;
		((GamePlayState*)m_vStateStack[STATE_TYPE::GAME_PLAY_STATE])->ResetGame();
	}

	if (stateType == -1)	//PostQuitMessage(1);
		return false;//Set the current state variable to the passed in state


	if ((m_CurrentState == STATE_TYPE::PAUSE_MENU_STATE || m_CurrentState == STATE_TYPE::HOW_TO_PLAY_STATE) && stateType == STATE_TYPE::GAME_PLAY_STATE)
	{
		bPauseSounds = false;
	}

	if (m_CurrentState == STATE_TYPE::HOW_TO_PLAY_STATE)
	{
		m_CurrentState = m_LastState;
		bPauseSounds = false;
	}

	else if ( m_CurrentState == STATE_TYPE::GAME_PLAY_STATE && stateType != STATE_TYPE::PAUSE_MENU_STATE) // No longer how we do this
	{
		globalGraphicsPointer->FlushOldMemory();
		SetRenderInfosNull();
		// Call All the things!
		m_LastState = m_CurrentState;
		m_CurrentState = stateType;
	}

	else if ( m_CurrentState == STATE_TYPE::PAUSE_MENU_STATE && stateType == STATE_TYPE::MAIN_MENU_STATE)
	{
		globalGraphicsPointer->FlushOldMemory();
		SetRenderInfosNull();
		// Call All the things!
		m_LastState = m_CurrentState;
		m_CurrentState = stateType;
	}

	else
	{
		m_LastState = m_CurrentState;
		m_CurrentState = stateType;
	}
	
	m_inputMGR->ResetInput();
	

	//Enter the new state
	m_vStateStack[m_CurrentState]->Enter(m_BitmapFontManager, bPauseSounds);
	//globalGraphicsPointer->FadeIn();
	return true;
	
}

void StateManager::RemoveState()
{
	m_vStateStack[m_CurrentState]->Terminate();
	m_vStateStack[m_CurrentState]->Exit();
	m_vStateStack.pop_back();
	m_CurrentState = (STATE_TYPE)(m_CurrentState - 1);
}

const BaseState* StateManager::GetCurrentState()
{
	return m_vStateStack[m_CurrentState];
}

void StateManager::ClearStates()
{
	for (unsigned int i = 0; i < m_vStateStack.size(); )
	{
		m_vStateStack.back()->Terminate();
		//m_vStateStack.back()->Exit();
		delete m_vStateStack.back();
		m_vStateStack.pop_back();
	}
}

void StateManager::Initialize(Renderer* renderer)
{
	//Set the renderer pointer
	m_Renderer = renderer;

	//create all the states, initialize them, and add them to the state stack
	BaseState* gamePlayState		= new GamePlayState;			//0
	BaseState* mainMenuState		= new MainMenuState;			//1
	BaseState* optionsMenuState		= new OptionsMenuState;			//2
	BaseState* howToPlayState		= new HowToPlayState;			//3
	BaseState* loadMenuState		= new LoadMenuState;			//4
	BaseState* soundMenuState		= new SoundOptionState;			//5
	BaseState* graphicsMenuState	= new GraphicsOptionsState;		//6
	BaseState* keybindingsMenuState = new KeybindingsOptionsState;	//7
	BaseState* upgradeShipState		= new UpgradeShipState;			//8
	BaseState* pauseMenuState		= new PauseMenuState;			//9
	BaseState* resultState			= new ResultState;				//10
	BaseState* levelSelectState		= new LevelSelectState;			//11
	BaseState* splashMenuState		= new SplashScreenState;		//12
	BaseState* creditsState			= new CreditsState;				//13
	/*		
	BaseState* pauseMenuState	= new PauseMenuState;
	BaseState* creditsState		= new CreditsState;*/

	gamePlayState->Initialize(m_Renderer);			//0
	mainMenuState->Initialize(m_Renderer);			//1
	optionsMenuState->Initialize(m_Renderer);		//2
	howToPlayState->Initialize(m_Renderer);			//3
	loadMenuState->Initialize(m_Renderer);			//4
	soundMenuState->Initialize(m_Renderer);			//5
	graphicsMenuState->Initialize(m_Renderer);		//6
	keybindingsMenuState->Initialize(m_Renderer);	//7
	upgradeShipState->Initialize(m_Renderer);		//8
	pauseMenuState->Initialize(m_Renderer);			//9
	resultState->Initialize(m_Renderer);			//10
	levelSelectState->Initialize(m_Renderer);		//11
	splashMenuState->Initialize(m_Renderer);		//12
	creditsState->Initialize(m_Renderer);			//13

	/*pauseMenuState->Initialize(m_Renderer);
	creditsState->Initialize(m_Renderer);*/

	m_vStateStack.push_back(gamePlayState);				//0
	m_vStateStack.push_back(mainMenuState);				//1
	m_vStateStack.push_back(optionsMenuState);			//2
	m_vStateStack.push_back(howToPlayState);			//3
	m_vStateStack.push_back(loadMenuState);				//4
	m_vStateStack.push_back(soundMenuState);			//5
	m_vStateStack.push_back(graphicsMenuState);			//6
	m_vStateStack.push_back(keybindingsMenuState);		//7
	m_vStateStack.push_back(upgradeShipState);			//8
	m_vStateStack.push_back(pauseMenuState);			//9
	m_vStateStack.push_back(resultState);				//10
	m_vStateStack.push_back(levelSelectState);			//11
	m_vStateStack.push_back(splashMenuState);			//12
	m_vStateStack.push_back(creditsState);
	gameData->LoadData("Assets/XML/Slot1Save.xml");

	/*m_vStateStack.push_back(pauseMenuState);
	m_vStateStack.push_back(creditsState);*/

	//m_CurrentState = STATE_TYPE::RESULT_STATE;

	//Setup the BitmapFontManager
	m_BitmapFontManager = new BitmapFontManager;

	//Load in the BitmapFonts xml files
	m_BitmapFontManager->LoadFont("Audiowide", "Assets/XML/FontXML/Audiowide.xml");

	globalGraphicsPointer->SetBitmapFont(m_BitmapFontManager->GetFont("Audiowide"));

	//Enter the current state
	m_vStateStack[m_CurrentState]->Enter(m_BitmapFontManager);

}

void StateManager::Terminate()
{	
	if (m_BitmapFontManager)
	{
		m_BitmapFontManager->Terminate();
		delete m_BitmapFontManager;
		m_BitmapFontManager = 0;
	}
	
	ClearStates();

}

bool StateManager::Update(float dt)
{
	m_vStateStack[m_CurrentState]->Update(dt);

	if (m_CurrentState != m_vStateStack[m_CurrentState]->m_NextState)
	{
		if (!SetState(m_vStateStack[m_CurrentState]->m_NextState))
			return false;
			
	}

	if (m_bTimeSlow == false)
	{
		if (m_vStateStack[m_CurrentState]->m_bTImeSlow)
			m_bTimeSlow = true;
	}
	else
	{
		if (m_vStateStack[m_CurrentState]->m_bTImeSlow == false)
			m_bTimeSlow = false;
	}
	

	return true;
}

void StateManager::SetRenderInfosNull()
{
	for(unsigned int i=0; i<m_vStateStack.size(); i++)
	{
		m_vStateStack[i]->m_ObjectFactory.RenderInfoHelper();
	}
}