#include "stdafx.h"
#include "OptionsMenuState.h"
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

OptionsMenuState::OptionsMenuState()
{
	dwPacketNo = 0;
}


OptionsMenuState::~OptionsMenuState()
{
}

void OptionsMenuState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_NextState = STATE_TYPE::OPTIONS_MENU_STATE;

	m_unCurrentButton = 0;

	if (m_controller)
	{
		m_controller->SetMenuKeys();
		m_controller->SetDebugKeys();
	}
	using namespace Events;
	//Setup Input
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &OptionsMenuState::OnEnter);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &OptionsMenuState::OnEscape);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Up"), this, &OptionsMenuState::OnArrowUp);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Down"), this, &OptionsMenuState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "UpArr"), this, &OptionsMenuState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownArr"), this, &OptionsMenuState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &OptionsMenuState::OnMouseClick);

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMousePrevX, fMousePrevY);

	//if (resetSound)
	//{
	//	g_AudioSystem->PostEvent(AK::EVENTS::STOP_ALL);
	//	g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MX_MUSICLOOP_02);
	//}

	m_debugListener.SetFlags(DebugListenFor::ResetAchievements);


	dwPacketNo = gamePad->GetState().state.dwPacketNumber;

}

void OptionsMenuState::Exit()
{
	using namespace Events;

	m_debugListener.ClearClients();

	EventManager()->UnregisterClient("Confirm", this, &OptionsMenuState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &OptionsMenuState::OnEscape);
	EventManager()->UnregisterClient("Up", this, &OptionsMenuState::OnArrowUp);
	EventManager()->UnregisterClient("Down", this, &OptionsMenuState::OnArrowDown);
	EventManager()->UnregisterClient("UpArr", this, &OptionsMenuState::OnArrowUp);
	EventManager()->UnregisterClient("DownArr", this, &OptionsMenuState::OnArrowDown);
	EventManager()->UnregisterClient("MouseLClick", this, &OptionsMenuState::OnMouseClick);
	if (m_controller)
	{
		m_controller->ClearMenuKeys();
		m_controller->ClearDebugKeys();
	}
}

void OptionsMenuState::Initialize(Renderer* renderer)
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
	m_vButtons.push_back(3);
	m_vButtons.push_back(4);
}

void OptionsMenuState::Update(float dt)
{
	// Keyboard
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
	if (_finite(fGamepadTimer) == 0)
	{
		std::cout << "GAMEPAD TIMER IS GARBAGE VAL!!\n";
		fGamepadTimer = 0;
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

void OptionsMenuState::Terminate()
{
	m_vButtons.clear();
}

void OptionsMenuState::CreateHud(void)
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

	//Create the Sound Options Button
	m_SoundOptionsButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "soundOptionsButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* soundOptionsButton = (RenderComp*)m_SoundOptionsButton->GetComponent("RenderComp");
	if (soundOptionsButton != nullptr)
	{
		soundOptionsButton->m_posX = 0.266f;
		soundOptionsButton->m_posY = 0.08f;
		soundOptionsButton->m_height = 0.175f;
		soundOptionsButton->m_width = 0.325f;
	}

	m_SoundOptionsButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "soundOptionsButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	soundOptionsButton = (RenderComp*)m_SoundOptionsButtonSelected->GetComponent("RenderComp");
	if (soundOptionsButton != nullptr)
	{
		soundOptionsButton->m_posX = 0.266f;
		soundOptionsButton->m_posY = 0.08f;
		soundOptionsButton->m_height = 0.0f;
		soundOptionsButton->m_width = 0.0f;		
	}

	//Create the Graphics Options Button
	m_GraphicsOptionsButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "graphicsOptionsButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* graphicsOptionsButton = (RenderComp*)m_GraphicsOptionsButton->GetComponent("RenderComp");
	if (graphicsOptionsButton != nullptr)
	{
		graphicsOptionsButton->m_posX = 0.266f;
		graphicsOptionsButton->m_posY = 0.245f;
		graphicsOptionsButton->m_height = 0.175f;
		graphicsOptionsButton->m_width = 0.325f;
	}
	
	m_GraphicsOptionsButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "graphicsOptionsButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	graphicsOptionsButton = (RenderComp*)m_GraphicsOptionsButtonSelected->GetComponent("RenderComp");
	if (graphicsOptionsButton != nullptr)
	{
		graphicsOptionsButton->m_posX = 0.266f;
		graphicsOptionsButton->m_posY = 0.245f;
		graphicsOptionsButton->m_height = 0.0f;
		graphicsOptionsButton->m_width = 0.0f;
	}

	//Create the Keybindings Options Button
	m_KeybindingsOptionsButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "inputButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* keybindingsOptionsButton = (RenderComp*)m_KeybindingsOptionsButton->GetComponent("RenderComp");
	if (keybindingsOptionsButton != nullptr)
	{
		keybindingsOptionsButton->m_posX = 0.266f;
		keybindingsOptionsButton->m_posY = 0.42f;
		keybindingsOptionsButton->m_height = 0.175f;
		keybindingsOptionsButton->m_width = 0.325f;
	}

	m_KeybindingsOptionsButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "inputButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	keybindingsOptionsButton = (RenderComp*)m_KeybindingsOptionsButtonSelected->GetComponent("RenderComp");
	if (keybindingsOptionsButton != nullptr)
	{
		keybindingsOptionsButton->m_posX = 0.266f;
		keybindingsOptionsButton->m_posY = 0.42f;
		keybindingsOptionsButton->m_height = 0.0f;
		keybindingsOptionsButton->m_width = 0.0f;
		//keybindingsOptionsButton->color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	//Create the Game Mode Options Button
	m_pCreditsButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "creditsButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* creditsButton = (RenderComp*)m_pCreditsButton->GetComponent("RenderComp");
	if (creditsButton != nullptr)
	{
		creditsButton->m_posX = 0.266f;
		creditsButton->m_posY = 0.595f;
		creditsButton->m_height = 0.175f;
		creditsButton->m_width = 0.325f;
	}

	m_pCreditsSelectedButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "creditsButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	creditsButton = (RenderComp*)m_pCreditsSelectedButton->GetComponent("RenderComp");
	if (creditsButton != nullptr)
	{
		creditsButton->m_posX = 0.266f;
		creditsButton->m_posY = 0.595f;
		creditsButton->m_height = 0.0f;
		creditsButton->m_width = 0.0f;		
	}

	//Create the Back to Main Menu button
	m_BackToMainMenuButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* backToMainMenuButton = (RenderComp*)m_BackToMainMenuButton->GetComponent("RenderComp");

	if (backToMainMenuButton != nullptr)
	{
		backToMainMenuButton->m_posX = 0.266f;
		backToMainMenuButton->m_posY = 0.77f;
		backToMainMenuButton->m_height = 0.175f;
		backToMainMenuButton->m_width = 0.325f;
	}

	m_BackToMainMenuButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	backToMainMenuButton = (RenderComp*)m_BackToMainMenuButtonSelected->GetComponent("RenderComp");

	if (backToMainMenuButton != nullptr)
	{
		backToMainMenuButton->m_posX = 0.266f;
		backToMainMenuButton->m_posY = 0.77f;
		backToMainMenuButton->m_height = 0.0f;
		backToMainMenuButton->m_width = 0.0f;
	}

	//Create the Select Button
	m_SelectButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "selectButton", Render2D_Front);
	RenderComp* selectButton = (RenderComp*)m_SelectButton->GetComponent("RenderComp");

	if (selectButton != nullptr)
	{
		selectButton->m_posX = 0.225f;
		selectButton->m_posY = 0.095f;
		selectButton->m_height = 0.1f;
		selectButton->m_width = 0.1f;
		selectButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
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
			menuNavKeyboardInstr->color = XMFLOAT4(1, 2.0f, 2.0f, 1);

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

void OptionsMenuState::OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton - 1);
	g_AudioSystem->PlaySound("MENU_Hover");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void OptionsMenuState::OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton + 1);
	g_AudioSystem->PlaySound("MENU_Hover");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void OptionsMenuState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (m_SelectedState == STATE_TYPE::MAIN_MENU_STATE || m_SelectedState == STATE_TYPE::PAUSE_MENU_STATE )
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Back");

	}
	else
	{
		m_NextState = m_SelectedState;

		if (m_unCurrentButton == 0 || m_unCurrentButton == 1 || m_unCurrentButton == 2 || m_unCurrentButton == 3)
		{
			g_AudioSystem->PlaySound("MENU_Accept");
		}
		else
		{
			g_AudioSystem->PlaySound("MENU_Error");
		}
	}
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void OptionsMenuState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = gameData->m_OptionReturnState;
	if (m_NextState == STATE_TYPE::MAIN_MENU_STATE || m_NextState == STATE_TYPE::PAUSE_MENU_STATE)
	{

		g_AudioSystem->PlaySound("MENU_Back");

	}

	//m_NextState = STATE_TYPE::MAIN_MENU_STATE;
	//g_AudioSystem->PlaySound("MENU_Back");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void OptionsMenuState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (CheckMouseMove() || globalGraphicsPointer->GetCrosshairShowing())
	{
		SetSelectButton();
		m_NextState = m_SelectedState;
		if (m_NextState == STATE_TYPE::MAIN_MENU_STATE || m_NextState == STATE_TYPE::PAUSE_MENU_STATE)
		{
		
			g_AudioSystem->PlaySound("MENU_Back");

		}
		else
		{
			if (m_unCurrentButton == 0 || m_unCurrentButton == 1 || m_unCurrentButton == 2 || m_unCurrentButton == 3)
			{
				g_AudioSystem->PlaySound("MENU_Accept");
			}
			else
			{
				g_AudioSystem->PlaySound("MENU_Error");
			}
		}
	}
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void OptionsMenuState::SetSelectButton(void)
{
	ShrinkAllButtons();

	RenderComp* selectButton = (RenderComp*)m_SelectButton->GetComponent("RenderComp");

	switch (m_unCurrentButton)
	{
	case 0:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posY = 0.095f;
		}

		GrowSelectedButton(m_SoundOptionsButtonSelected);

		m_SelectedState = STATE_TYPE::SOUND_MENU_STATE;
	}
		break;
	case 1:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posY = 0.27f;
		}

		GrowSelectedButton(m_GraphicsOptionsButtonSelected);

		m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;
		//m_SelectedState = STATE_TYPE::OPTIONS_MENU_STATE;
	}
		break;
	case 2:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posY = 0.44f;
		}

		GrowSelectedButton(m_KeybindingsOptionsButtonSelected);
		m_SelectedState = STATE_TYPE::KEYBINDINGS_MENU_STATE;
	}
		break;
	case 3:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posY = 0.62f;
		}

		GrowSelectedButton(m_pCreditsSelectedButton);

		m_SelectedState = STATE_TYPE::CREDITS_STATE;
	}
		break;
	case 4:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posY = 0.79f;
		}

		GrowSelectedButton(m_BackToMainMenuButtonSelected);
		m_SelectedState = gameData->m_OptionReturnState;
	}
		break;
	default:
		break;
	}
}

bool OptionsMenuState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

	if (fMouseX == fMousePrevX && fMouseY == fMousePrevY)
		return false;

	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_SoundOptionsButton->GetComponent("RenderComp");
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

	// Check Buttons - Sound Options


	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		return true;
	}

	//Check Buttons Graphics Options
	pButton = (RenderComp*)m_GraphicsOptionsButton->GetComponent("RenderComp");
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

	// Check Buttons- Keybindings
	pButton = (RenderComp*)m_KeybindingsOptionsButton->GetComponent("RenderComp");
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

	//Check Buttons- Game Mode
	pButton = (RenderComp*)m_pCreditsButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 3)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 3;
		return true;
	}

	// Check Buttons- Back to Main Menu
	pButton = (RenderComp*)m_BackToMainMenuButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 4)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 4;
		return true;
	}


	return false;
}

bool OptionsMenuState::Handle360Input()
{
	GamePad tmpPad = gamePad->GetState();
	bool detectedInput = false;

	if (tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bHeld ||
		tmpPad.buttons[buttonList::DPAD_UP] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::DPAD_UP] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton - 1);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bHeld ||
		tmpPad.buttons[buttonList::DPAD_DOWN] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::DPAD_DOWN] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton + 1);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Accept");
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::B] == buttonStatus::bPress)
	{
		if (m_unCurrentButton != 4)
		{
			m_unCurrentButton = 4;
			g_AudioSystem->PlaySound("MENU_Back");
			//SetSelectButton();
		}
			
// 		else if (m_unCurrentButton == 4)
// 		{
// 			g_AudioSystem->PlaySound("MENU_Back");
// 			m_NextState = m_SelectedState;
// 		}

		detectedInput = true;
	}
	return detectedInput;
}

void OptionsMenuState::SetCurrentButton(int numButton)
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

void OptionsMenuState::ShrinkAllButtons()
{
	RenderComp* pButton = (RenderComp*)m_SoundOptionsButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_GraphicsOptionsButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_KeybindingsOptionsButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pCreditsSelectedButton->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_BackToMainMenuButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}
}

void OptionsMenuState::GrowSelectedButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.175f;
		pButton->m_width = 0.325f;
	}
}