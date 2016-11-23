#include "stdafx.h"
#include "KeybindingsOptionsState.h"
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

KeybindingsOptionsState::KeybindingsOptionsState()
{
	dwPacketNo = 0;
}


KeybindingsOptionsState::~KeybindingsOptionsState()
{
}

void KeybindingsOptionsState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_NextState = STATE_TYPE::KEYBINDINGS_MENU_STATE;

	m_unCurrentButton = 0;

	if (m_controller)
	{
		m_controller->SetGraphicsKeys();
	}
	using namespace Events;
	//Setup Input
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &KeybindingsOptionsState::OnEnter);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &KeybindingsOptionsState::OnEscape);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Up"), this, &KeybindingsOptionsState::OnArrowUp);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Down"), this, &KeybindingsOptionsState::OnArrowDown);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Left"), this, &KeybindingsOptionsState::OnArrowLeft);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Right"), this, &KeybindingsOptionsState::OnArrowRight);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "UpArr"), this, &KeybindingsOptionsState::OnArrowUp);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownArr"), this, &KeybindingsOptionsState::OnArrowDown);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "LeftArr"), this, &KeybindingsOptionsState::OnArrowLeft);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "RightArr"), this, &KeybindingsOptionsState::OnArrowRight);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &KeybindingsOptionsState::OnMouseClick);


	dwPacketNo = gamePad->GetState().state.dwPacketNumber;


	if (gameData->m_bUsingGamepad)
		SetInputMode(1);

}

void KeybindingsOptionsState::Exit()
{
	using namespace Events;

	EventManager()->UnregisterClient("Confirm", this, &KeybindingsOptionsState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &KeybindingsOptionsState::OnEscape);
	EventManager()->UnregisterClient("Up", this, &KeybindingsOptionsState::OnArrowUp);
	EventManager()->UnregisterClient("Down", this, &KeybindingsOptionsState::OnArrowDown);
	EventManager()->UnregisterClient("UpArr", this, &KeybindingsOptionsState::OnArrowUp);
	EventManager()->UnregisterClient("DownArr", this, &KeybindingsOptionsState::OnArrowDown);
	EventManager()->UnregisterClient("LeftArr", this, &KeybindingsOptionsState::OnArrowLeft);
	EventManager()->UnregisterClient("RightArr", this, &KeybindingsOptionsState::OnArrowRight);
	EventManager()->UnregisterClient("MouseLClick", this, &KeybindingsOptionsState::OnMouseClick);

	if (m_controller)
	{
		m_controller->ClearGraphicsKeys();
	}

	gameData->m_bUsingGamepad = (bool)m_nInputIndex;
	if (gameData->m_bUsingGamepad)
		globalGraphicsPointer->SetCrosshairShowing(false);
	else
		globalGraphicsPointer->SetCrosshairShowing(true);
}

void KeybindingsOptionsState::Initialize(Renderer* renderer)
{
	m_Renderer = renderer;
	m_ObjectManager.Initialize();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory(&m_ObjectFactory);

	CreateHud();

	//Set Up Buttons
	m_vButtons.clear();
	m_vButtons.push_back(0);
	m_vButtons.push_back(1);
}

void KeybindingsOptionsState::Update(float dt)
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
			if(Handle360Input())
			 	fGamepadTimer = 0.1f;

// 			if (gamePad->GetState().state.dwPacketNumber != dwPacketNo)
// 			{
// 				dwPacketNo = gamePad->GetState().state.dwPacketNumber;
// 				bStickHeld = false;
// 				
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
	
}

void KeybindingsOptionsState::Terminate()
{
	m_vButtons.clear();
}

//Input Functions
void KeybindingsOptionsState::OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton - 1);
	g_AudioSystem->PlaySound("MENU_Hover");

}

void KeybindingsOptionsState::OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton + 1);
	g_AudioSystem->PlaySound("MENU_Hover");

}

void KeybindingsOptionsState::OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (m_unCurrentButton == 0)
	{
		SetInputMode(m_nInputIndex - 1);
		g_AudioSystem->PlaySound("MENU_Accept");
	}
}

void KeybindingsOptionsState::OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (m_unCurrentButton == 0)
	{
		SetInputMode(m_nInputIndex + 1);
		g_AudioSystem->PlaySound("MENU_Accept");
	}
}

void KeybindingsOptionsState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (m_NextState != m_SelectedState)
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Back");

	}
	else
	{
		g_AudioSystem->PlaySound("MENU_Accept");

	}
}

void KeybindingsOptionsState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = STATE_TYPE::OPTIONS_MENU_STATE;
	g_AudioSystem->PlaySound("MENU_Back");

}

void KeybindingsOptionsState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (CheckMouseMove())
	{
		SetSelectButton();
		m_NextState = m_SelectedState;
		if (m_NextState == STATE_TYPE::OPTIONS_MENU_STATE)
		{
			g_AudioSystem->PlaySound("MENU_Back");

		}
		else
		{
			if (CheckMouseMove())
			{
				SetSelectButton();

				using namespace Events;

				MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

				RECT clientRect;
				RenderComp* pButton = (RenderComp*)m_pInputLeftArrow->GetComponent("RenderComp");
				if (!pButton)
					return;

				if (!globalGraphicsPointer)
					return;

				GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

				fMouseX = fMouseX / (clientRect.right - clientRect.left);
				fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

				if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
					fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
				{
					SetInputMode(m_nInputIndex - 1);
					g_AudioSystem->PlaySound("MENU_Accept");
				}

				pButton = (RenderComp*)m_pInputRightArrow->GetComponent("RenderComp");
				if (!pButton)
					return;

				if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
					fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
				{
					SetInputMode(m_nInputIndex + 1);
					g_AudioSystem->PlaySound("MENU_Accept");
				}
			}
		}
	}
}

//Private Functions
void KeybindingsOptionsState::CreateHud(void)
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

	//Create the Input button
	m_pInputButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "inputButton");
	RenderComp* inputButton = (RenderComp*)m_pInputButton->GetComponent("RenderComp");
	if (inputButton != nullptr)
	{
		inputButton->m_posX = 0.15f;
		inputButton->m_posY = 0.275f;
		inputButton->m_height = 0.15f;
		inputButton->m_width = 0.3f;
	}

	m_pInputButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "inputButtonSelected");
	inputButton = (RenderComp*)m_pInputButtonSelected->GetComponent("RenderComp");
	if (inputButton != nullptr)
	{
		inputButton->m_posX = 0.15f;
		inputButton->m_posY = 0.275f;
		inputButton->m_height = 0.0f;
		inputButton->m_width = 0.0f;
	}

	//Draw Input Left Arrow
	m_pInputLeftArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow");
	RenderComp* inputLeftArrow = (RenderComp*)m_pInputLeftArrow->GetComponent("RenderComp");
	if (inputLeftArrow != nullptr)
	{
		inputLeftArrow->m_posX = 0.46f;
		inputLeftArrow->m_posY = 0.33f;
		inputLeftArrow->m_height = 0.05f;
		inputLeftArrow->m_width = 0.05f;
	}

	//Draw Input Right Arrow
	m_pInputRightArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow");
	RenderComp* inputRightArrow = (RenderComp*)m_pInputRightArrow->GetComponent("RenderComp");
	if (inputRightArrow != nullptr)
	{
		inputRightArrow->m_posX = 0.87f;
		inputRightArrow->m_posY = 0.33f;
		inputRightArrow->m_height = 0.05f;
		inputRightArrow->m_width = 0.05f;
	}

	//Draw Input mode
	
	m_pInput = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), keyboardMode, eRENDERCOMP_TYPE::Render2D_Text);
	RenderComp* inputMode = (RenderComp*)m_pInput->GetComponent("RenderComp");

	if (inputMode != nullptr)
	{
		inputMode->m_posX = 0.525f;
		inputMode->m_posY = 0.315f;
		inputMode->m_height = 0.3f;
		inputMode->m_width = 0.3f;

		inputMode->color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	}

	//Create the Back Button
	m_pBackButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButton");
	RenderComp* backButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
	if (backButton != nullptr)
	{
		backButton->m_posX = 0.35f;
		backButton->m_posY = 0.81f;
		backButton->m_height = 0.125f;
		backButton->m_width = 0.3f;
	}

	m_pBackButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButtonSelected");
	 backButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
	if (backButton != nullptr)
	{
		backButton->m_posX = 0.35f;
		backButton->m_posY = 0.81f;
		backButton->m_height = 0.125f;
		backButton->m_width = 0.3f;
	}

}

//Input Helper Functions
void KeybindingsOptionsState::SetSelectButton(void)
{
	
	ShrinkAllButtons();
	switch (m_unCurrentButton)
	{

	case 0:
	{
		GrowSelectedButton(m_pInputButtonSelected);
		m_SelectedState = STATE_TYPE::KEYBINDINGS_MENU_STATE;

	}
		break;
	case 1:
	{
		GrowSelectedButton(m_pBackButtonSelected);
		m_SelectedState = STATE_TYPE::OPTIONS_MENU_STATE;

	}
		break;
	default:
		break;
	}
}

bool KeybindingsOptionsState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (!globalGraphicsPointer)
		return false;

	GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

	fMouseX = fMouseX / (clientRect.right - clientRect.left);
	fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

	// Check Buttons - Slot1Button


	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 1)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 1;
		return true;
	}

	pButton = (RenderComp*)m_pInputLeftArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;		
		return true;
	}

	pButton = (RenderComp*)m_pInputRightArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		return true;
	}

	pButton = (RenderComp*)m_pInputButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		return true;
	}
	return false;
}

bool KeybindingsOptionsState::Handle360Input()
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

	if (tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bHeld)
	{
		if (m_unCurrentButton == 0)
		{
			SetInputMode(m_nInputIndex - 1);
			g_AudioSystem->PlaySound("MENU_Accept");
		}
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bHeld)
	{
		if (m_unCurrentButton == 0)
		{
			SetInputMode(m_nInputIndex + 1);
			g_AudioSystem->PlaySound("MENU_Accept");
		}
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Accept");
		detectedInput = true;
	}

	return detectedInput;
}

void KeybindingsOptionsState::SetCurrentButton(int numButton)
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

void KeybindingsOptionsState::ShrinkAllButtons()
{
	RenderComp* pButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pInputButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}
}

void KeybindingsOptionsState::GrowSelectedButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.15f;
		pButton->m_width = 0.3f;
	}
}

void KeybindingsOptionsState::SetInputMode(int index)
{
	if (index < 0)
	{
		m_nInputIndex = 1;
	}
	else if (index > 1)
	{
		m_nInputIndex = 0;
	}
	else
	{
		m_nInputIndex = index;
	}

	if (m_nInputIndex == 0)
	{
		ChangeStringText(m_pInput, keyboardMode);
	}
	else
	{
		ChangeStringText(m_pInput, gamepadMode);
	}
}

void KeybindingsOptionsState::ChangeStringText(GameObject* obj, string newString)
{
	RenderComp* messageComp = (RenderComp*)obj->GetComponent("RenderComp");
	if (messageComp)
	{
		messageComp->SetID(newString);
		messageComp->m_renderInfo = nullptr;
	}
}