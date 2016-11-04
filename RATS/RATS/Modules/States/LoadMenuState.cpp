#include "stdafx.h"
#include "LoadMenuState.h"
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

LoadMenuState::LoadMenuState()
{
	dwPacketNo = 0;
}


LoadMenuState::~LoadMenuState()
{
}

void LoadMenuState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_NextState = STATE_TYPE::LOAD_MENU_STATE;

	m_unCurrentButton = 0;

	if (m_controller)
		m_controller->SetMenuKeys();
	using namespace Events;
	//Setup Input
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &LoadMenuState::OnEnter);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &LoadMenuState::OnEscape);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Up"), this, &LoadMenuState::OnArrowUp);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Down"), this, &LoadMenuState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "UpArr"), this, &LoadMenuState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownArr"), this, &LoadMenuState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &LoadMenuState::OnMouseClick);


	//if (resetSound)
	//{
	//	g_AudioSystem->PostEvent(AK::EVENTS::STOP_ALL);
	//	g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MX_MUSICLOOP_02);
	//}

	dwPacketNo = gamePad->GetState().state.dwPacketNumber;
}

void LoadMenuState::Exit()
{
	using namespace Events;

	EventManager()->UnregisterClient("Confirm", this, &LoadMenuState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &LoadMenuState::OnEscape);
	EventManager()->UnregisterClient("Up", this, &LoadMenuState::OnArrowUp);
	EventManager()->UnregisterClient("Down", this, &LoadMenuState::OnArrowDown);
	EventManager()->UnregisterClient("UpArr", this, &LoadMenuState::OnArrowUp);
	EventManager()->UnregisterClient("DownArr", this, &LoadMenuState::OnArrowDown);
	EventManager()->UnregisterClient("MouseLClick", this, &LoadMenuState::OnMouseClick);
	if (m_controller)
		m_controller->ClearMenuKeys();
}

void LoadMenuState::Initialize(Renderer* renderer)
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
	m_vButtons.push_back(2);
	m_vButtons.push_back(3);
}

void LoadMenuState::Update(float dt)
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

void LoadMenuState::Terminate()
{
	m_vButtons.clear();
}

void LoadMenuState::OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton - 1);
		g_AudioSystem->PlaySound("MENU_Hover");

}

void LoadMenuState::OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton + 1);
	g_AudioSystem->PlaySound("MENU_Hover");

}

void LoadMenuState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
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

void LoadMenuState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = STATE_TYPE::MAIN_MENU_STATE;
	g_AudioSystem->PlaySound("MENU_Back");

}

void LoadMenuState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (CheckMouseMove())
	{
		SetSelectButton();
		m_NextState = m_SelectedState;
		if (m_NextState == STATE_TYPE::MAIN_MENU_STATE)
		{
			g_AudioSystem->PlaySound("MENU_Back");

		}
		else
		{
			g_AudioSystem->PlaySound("MENU_Back");

		}
	}
}

void LoadMenuState::CreateHud(void)
{
	//Create the Slot1Button
	//Create the BGM Volume Button
	m_pSlot1Button = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "slot1Button");
	RenderComp* slot1Button = (RenderComp*)m_pSlot1Button->GetComponent("RenderComp");
	if (slot1Button != nullptr)
	{
		slot1Button->m_posX = 0.35f;
		slot1Button->m_posY = 0.07f;
		slot1Button->m_height = 0.175f;
		slot1Button->m_width = 0.3f;
	}

	//Create the Slot2Button
	m_pSlot2Button = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "slot2Button");
	RenderComp* slot2Button = (RenderComp*)m_pSlot2Button->GetComponent("RenderComp");
	if (slot2Button != nullptr)
	{
		slot2Button->m_posX = 0.35f;
		slot2Button->m_posY = 0.275f;
		slot2Button->m_height = 0.175f;
		slot2Button->m_width = 0.3f;
	}
	
	//Create the Slot3Button
	m_pSlot3Button = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "slot3Button");
	RenderComp* slot3Button = (RenderComp*)m_pSlot3Button->GetComponent("RenderComp");
	if (slot3Button != nullptr)
	{
		slot3Button->m_posX = 0.35f;
		slot3Button->m_posY = 0.48f;
		slot3Button->m_height = 0.175f;
		slot3Button->m_width = 0.3f;
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

	//Create the Select Button
	m_pSelectButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "selectButton");
	RenderComp* selectButton = (RenderComp*)m_pSelectButton->GetComponent("RenderComp");

	if (selectButton != nullptr)
	{
		selectButton->m_posX = 0.225f;
		selectButton->m_posY = 0.095f;
		selectButton->m_height = 0.1f;
		selectButton->m_width = 0.1f;
	}
}

void LoadMenuState::SetSelectButton(void)
{
	RenderComp* selectButton = (RenderComp*)m_pSelectButton->GetComponent("RenderComp");

	switch (m_unCurrentButton)
	{
	case 0:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posX = 0.225f;
			selectButton->m_posY = 0.095f;
		}

		m_SelectedState = STATE_TYPE::LOAD_MENU_STATE;

	}
		break;
	case 1:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posX = 0.225f;
			selectButton->m_posY = 0.3f;
		}

		m_SelectedState = STATE_TYPE::LOAD_MENU_STATE;

	}
		break;
	case 2:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posX = 0.225f;
			selectButton->m_posY = 0.51f;
		}

	
		m_SelectedState = STATE_TYPE::LOAD_MENU_STATE;
	}
		break;
	case 3:
	{
		if (selectButton != nullptr)
		{
			selectButton->m_posX = 0.225f;
			selectButton->m_posY = 0.8f;
		}
		m_SelectedState = STATE_TYPE::MAIN_MENU_STATE;
	}
		break;
	default:
		break;
	}
}

bool LoadMenuState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_pSlot1Button->GetComponent("RenderComp");
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
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		return true;
	}

	
	//Check Buttons - Slot2Button
	pButton = (RenderComp*)m_pSlot2Button->GetComponent("RenderComp");
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

	// Check Buttons- Slot3Button
	pButton = (RenderComp*)m_pSlot3Button->GetComponent("RenderComp");
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


	///////////////////////////////////////
	//Check Buttons - Back Button
	//////////////////////////////////////
	pButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
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
	return false;
}

bool LoadMenuState::Handle360Input()
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
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Accept");
		detectedInput = true;
	}

	return detectedInput;
}

void LoadMenuState::SetCurrentButton(int numButton)
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