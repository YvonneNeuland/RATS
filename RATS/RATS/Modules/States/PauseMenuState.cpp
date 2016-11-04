#include "stdafx.h"
#include "PauseMenuState.h"
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

PauseMenuState::PauseMenuState()
{
	dwPacketNo = 0;
}


PauseMenuState::~PauseMenuState()
{
}

void PauseMenuState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_NextState = STATE_TYPE::PAUSE_MENU_STATE;

	if (m_controller)
		m_controller->SetMenuKeys();

	using namespace Events;

	//Setup Input
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &PauseMenuState::OnEnter);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &PauseMenuState::OnEscape);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Up"), this, &PauseMenuState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Down"), this, &PauseMenuState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "UpArr"), this, &PauseMenuState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownArr"), this, &PauseMenuState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &PauseMenuState::OnMouseClick);

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMousePrevX, fMousePrevY);

	dwPacketNo = gamePad->GetState().state.dwPacketNumber;
	gamePad->bLostCOnnection = false;
}

void PauseMenuState::Exit()
{
	using namespace Events;

	EventManager()->UnregisterClient("Confirm", this, &PauseMenuState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &PauseMenuState::OnEscape);
	EventManager()->UnregisterClient("Up", this, &PauseMenuState::OnArrowUp);
	EventManager()->UnregisterClient("Down", this, &PauseMenuState::OnArrowDown);
	EventManager()->UnregisterClient("UpArr", this, &PauseMenuState::OnArrowUp);
	EventManager()->UnregisterClient("DownArr", this, &PauseMenuState::OnArrowDown);
	EventManager()->UnregisterClient("MouseLClick", this, &PauseMenuState::OnMouseClick);
	if (m_controller)
		m_controller->ClearMenuKeys();

	if (m_NextState == MAIN_MENU_STATE)
	{
		g_AudioSystem->ResumeAllSounds();
		g_AudioSystem->StopAllSounds();
	}
}

void PauseMenuState::Initialize(Renderer* renderer)
{
	m_Renderer = renderer;
	m_ObjectManager.Initialize();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory(&m_ObjectFactory);

	CreateHud();

	////Set Up Buttons
	m_vButtons.clear();
	m_vButtons.push_back(0);
	m_vButtons.push_back(1);
	m_vButtons.push_back(2);

}

void PauseMenuState::Update(float dt)
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
			if (Handle360Input())
				fGamepadTimer = 0.1f;
// 			if (gamePad->GetState().state.dwPacketNumber != dwPacketNo)
// 			{
// 				dwPacketNo = gamePad->GetState().state.dwPacketNumber;
// 				bStickHeld = false;
// 				Handle360Input();
// 				globalGraphicsPointer->SetCrosshairShowing(false);
// 				fGamepadTimer = 0.1f;
// 			}
// 			else if (bStickHeld)
// 			{
// 				Handle360Input();
// 				fGamepadTimer = 0.2f;
// 			}
// 			else if (gamePad->GetState().normLS[1] != 0)
// 			{
// 				bStickHeld = true;
// 			}

		}
	}
	
	SetSelectButton();

	//switch out the menu navigation instructions if GamepadInstructions are showing and the mouse/keyboard are being used
	if (globalGraphicsPointer->GetCrosshairShowing() && m_bGamepadInstructionsOn)
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
	if (!globalGraphicsPointer->GetCrosshairShowing() && !m_bGamepadInstructionsOn)
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

void PauseMenuState::Terminate()
{
	m_vButtons.clear();
}

void PauseMenuState::CreateHud(void)
{
	//Create the menu background
	m_pMenuBackground = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "menuBackground", eRENDERCOMP_TYPE::Render2D_Back);
	RenderComp* menuBackground = (RenderComp*)m_pMenuBackground->GetComponent("RenderComp");
	if (menuBackground != nullptr)
	{
		menuBackground->m_posX = 0.0f;
		menuBackground->m_posY = 0.0f;
		menuBackground->m_height = 1.0f;
		menuBackground->m_width = 1.0f;
	}

	//Create the Resume Game Button
	m_pResumeGameButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "resumeGameButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* resumeGameButton = (RenderComp*)m_pResumeGameButton->GetComponent("RenderComp");
	if (resumeGameButton != nullptr)
	{
		resumeGameButton->m_posX = 0.3f;
		resumeGameButton->m_posY = 0.145f;
		resumeGameButton->m_height = 0.15f;
		resumeGameButton->m_width  = 0.3f;
	}

	m_pResumeGameButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "resumeGameButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	resumeGameButton = (RenderComp*)m_pResumeGameButtonSelected->GetComponent("RenderComp");
	if (resumeGameButton != nullptr)
	{
		resumeGameButton->m_posX = 0.3f;
		resumeGameButton->m_posY = 0.145f;
		resumeGameButton->m_height = 0.0f;
		resumeGameButton->m_width = 0.0f;
	}

	//Create the Options Button
	m_pOptionsButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "optionsButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* optionButton = (RenderComp*)m_pOptionsButton->GetComponent("RenderComp");

	if (optionButton != nullptr)
	{
		optionButton->m_posX = 0.3f;
		optionButton->m_posY = 0.32f;
		optionButton->m_height = 0.15f;
		optionButton->m_width = 0.3f;
	}

	m_pOptionsButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "optionsButtonSelect", eRENDERCOMP_TYPE::Render2D_Front);
	optionButton = (RenderComp*)m_pOptionsButtonSelected->GetComponent("RenderComp");

	if (optionButton != nullptr)
	{
		optionButton->m_posX = 0.3f;
		optionButton->m_posY = 0.32f;
		optionButton->m_height = 0.0f;
		optionButton->m_width = 0.0f;
	}

	//Create the Quit Button
	m_pQuitButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "quitButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* quitButton = (RenderComp*)m_pQuitButton->GetComponent("RenderComp");

	if (quitButton != nullptr)
	{
		quitButton->m_posX = 0.3f;
		quitButton->m_posY = 0.495f;
		quitButton->m_height = 0.15f;
		quitButton->m_width = 0.3f;
	}

	m_pQuitButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "quitButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	quitButton = (RenderComp*)m_pQuitButtonSelected->GetComponent("RenderComp");

	if (quitButton != nullptr)
	{
		quitButton->m_posX = 0.3f;
		quitButton->m_posY = 0.495f;
		quitButton->m_height = 0.0f;
		quitButton->m_width = 0.0f;
	}

	//Create the Navigation instructions
	if (globalGraphicsPointer->GetCrosshairShowing())
	{
		m_pMenuNavKeyboardInstr = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "menuNavKeyboardInstr", eRENDERCOMP_TYPE::Render2D_Front);
		RenderComp* menuNavKeyboardInstr = (RenderComp*)m_pMenuNavKeyboardInstr->GetComponent("RenderComp");

		if (menuNavKeyboardInstr != nullptr)
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

		m_pMenuNavGamepadInstr = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "menuNavGamepadInstr", eRENDERCOMP_TYPE::Render2D_Front);
		RenderComp* menuNavGamepadInstr = (RenderComp*)m_pMenuNavGamepadInstr->GetComponent("RenderComp");

		if (menuNavGamepadInstr != nullptr)
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

void PauseMenuState::SetSelectButton(void)
{

	ShrinkAllButtons();

	switch (m_unCurrentButton)
	{
	case 0:
	{		
		GrowSelectedButton(m_pResumeGameButtonSelected);
		m_SelectedState = STATE_TYPE::GAME_PLAY_STATE;
	}
		break;
	case 1:
	{		
		GrowSelectedButton(m_pOptionsButtonSelected);
		gameData->m_OptionReturnState = STATE_TYPE::PAUSE_MENU_STATE;
		m_SelectedState = STATE_TYPE::OPTIONS_MENU_STATE;
	}
		break;
	case 2:
	{
		GrowSelectedButton(m_pQuitButtonSelected);
		m_SelectedState = STATE_TYPE::MAIN_MENU_STATE;
	}
		break;
	default:
		break;
	}
}

void PauseMenuState::OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton - 1);
	g_AudioSystem->PlaySound("MENU_Hover");
	globalGraphicsPointer->SetCrosshairShowing(true);

}

void PauseMenuState::OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton + 1);
	g_AudioSystem->PlaySound("MENU_Hover");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void PauseMenuState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (m_SelectedState == MAIN_MENU_STATE)
	{
		gameData->m_bResetGame = true;
	}


	if (m_SelectedState == STATE_TYPE::GAME_PLAY_STATE)
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Back");
		

	}
	else
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Accept");
		


	}
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void PauseMenuState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = STATE_TYPE::GAME_PLAY_STATE;
	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_BACK);
	g_AudioSystem->PlaySound("MENU_Back");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void PauseMenuState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	
	if (gameData->m_bUsingGamepad)
		return;

	if (CheckMouseMove() || globalGraphicsPointer->GetCrosshairShowing())
	{
		SetSelectButton();
		m_NextState = m_SelectedState;

		if (m_SelectedState == MAIN_MENU_STATE)
		{
			gameData->m_bResetGame = true;
		}

		if (m_NextState != STATE_TYPE::MAIN_MENU_STATE)
		{
			//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
			g_AudioSystem->PlaySound("MENU_Accept");
			

		}
		else
		{
			g_AudioSystem->PlaySound("MENU_Back");
			

		}
	}
}

bool PauseMenuState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

	if (fMouseX == fMousePrevX && fMouseY == fMousePrevY)
		return false;

	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_pResumeGameButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (!globalGraphicsPointer)
		return false;

	globalGraphicsPointer->SetCrosshairShowing(true);
	fMousePrevX = fMouseX;
	fMousePrevY = fMouseY;

	GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

	fMouseX = fMouseX / (clientRect.right - clientRect.left);
	fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

	// Check Buttons - Resume Game
	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		return true;
	}

	//Check Buttons - Options Game Button
	pButton = (RenderComp*)m_pOptionsButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 1)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 1;
		return true;
	}

	//Check Buttons - Quit Game Button
	pButton = (RenderComp*)m_pQuitButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 2)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 2;
		return true;
	}
	return false;
}

bool PauseMenuState::Handle360Input()
{
	GamePad tmpPad = gamePad->GetState();
	bool detectedInput = false;

	if (tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton - 1);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton + 1);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{	

		if (m_SelectedState == MAIN_MENU_STATE)
		{
			gameData->m_bResetGame = true;
		}

		if (m_SelectedState == STATE_TYPE::GAME_PLAY_STATE)
		{
			m_NextState = m_SelectedState;
			g_AudioSystem->PlaySound("MENU_Back");

		}
		else
		{
			m_NextState = m_SelectedState;
			g_AudioSystem->PlaySound("MENU_Accept");

		}

		detectedInput = true;
	}

	return detectedInput;
}

void PauseMenuState::SetCurrentButton(int numButton)
{
	if (numButton > (int)(m_vButtons.size() - 1))
	{
		m_unCurrentButton = 0;
	}
	else if (numButton < 0)
	{
		m_unCurrentButton = m_vButtons.size() - 1;
	}
	else
	{
		m_unCurrentButton = numButton;
	}
}

void PauseMenuState::ResetButtonColors()
{
	RenderComp* pButton = (RenderComp*)m_pResumeGameButton->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	pButton = (RenderComp*)m_pQuitButton->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void PauseMenuState::ShrinkAllButtons()
{
	RenderComp* pButton = (RenderComp*)m_pResumeGameButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pQuitButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pOptionsButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}
}

void PauseMenuState::GrowSelectedButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.15f;
		pButton->m_width = 0.3f;
	}
}