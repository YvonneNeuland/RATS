#include "stdafx.h"
#include "HowToPlayState.h"
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

HowToPlayState::HowToPlayState()
{
}


HowToPlayState::~HowToPlayState()
{
}

void HowToPlayState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());
	m_NextState = STATE_TYPE::HOW_TO_PLAY_STATE;

	m_unCurrentButton = 0;

	if (m_controller)
		m_controller->SetHTPKeys();

	//Set up input
	using namespace Events;

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &HowToPlayState::OnEnter);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &HowToPlayState::OnEscape);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Left"), this, &HowToPlayState::OnArrowLeft);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Right"), this, &HowToPlayState::OnArrowRight);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "LeftArr"), this, &HowToPlayState::OnArrowLeft);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "RightArr"), this, &HowToPlayState::OnArrowRight);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &HowToPlayState::OnMouseClick);

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMousePrevX, fMousePrevY);
	dwPacketNo = gamePad->GetState().state.dwPacketNumber;
}

void HowToPlayState::Exit()
{
	//Set up input
	using namespace Events;
	
	EventManager()->UnregisterClient("Confirm", this, &HowToPlayState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &HowToPlayState::OnEscape);
	EventManager()->UnregisterClient("LeftArr", this, &HowToPlayState::OnArrowLeft);
	EventManager()->UnregisterClient("RightArr", this, &HowToPlayState::OnArrowRight);
	EventManager()->UnregisterClient("Left", this, &HowToPlayState::OnArrowLeft);
	EventManager()->UnregisterClient("Right", this, &HowToPlayState::OnArrowRight);
	EventManager()->UnregisterClient("MouseLClick", this, &HowToPlayState::OnMouseClick);

	if (m_controller)
		m_controller->ClearHTPKeys();


}


void HowToPlayState::Initialize(Renderer* renderer)
{
	m_Renderer = renderer;

	//Initialize the ObjectManager and the ObjectFactory
	m_ObjectManager.Initialize();// GamePlay();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory(&m_ObjectFactory);

	//Create the menu interface
	CreateHud();

	//Set Up Buttons
	m_vButtons.clear();
	m_vButtons.push_back(0);
	m_vButtons.push_back(1);
	m_vButtons.push_back(2);
	
}

void HowToPlayState::Update(float dt)
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
			/*globalGraphicsPointer->SetCrosshairShowing(false);*/
		}
	}
	SetSelectButton();
	
}

void HowToPlayState::Terminate()
{
	m_ObjectManager.Terminate();
}

void HowToPlayState::CreateHud(void)
{
	
	m_pInstructions1 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions1");
	m_pInstructions2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions2");	
	m_pInstructions3 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions3");
	m_pInstructions4 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions4");
	m_pInstructions5 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions5");
	m_pInstructions6 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions6");
	m_pInstructions7 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions7");
	m_pInstructions8 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions8");
	m_pInstructions9 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "instructions9");

	ShrinkAllInstructionScreens();
	EnlargeInstructionScreen(m_pInstructions1);

	m_pPreviousButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "previousButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* pButton = (RenderComp*)m_pPreviousButton->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.25f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.1f;
		pButton->m_width  = 0.2f;
	}

	m_pPreviousButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "previousButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	pButton = (RenderComp*)m_pPreviousButtonSelected->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.25f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	m_pNextButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "nextButton", eRENDERCOMP_TYPE::Render2D_Front);
	pButton = (RenderComp*)m_pNextButton->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.5f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.1f;
		pButton->m_width = 0.2f;
	}

	m_pNextButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "nextButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	pButton = (RenderComp*)m_pNextButtonSelected->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.5f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	m_pBackToMainMenu = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButton", eRENDERCOMP_TYPE::Render2D_Front);
	pButton = (RenderComp*)m_pBackToMainMenu->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.79f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.1f;
		pButton->m_width = 0.2f;
	}

	m_pBackToMainMenuSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	pButton = (RenderComp*)m_pBackToMainMenuSelected->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.79f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.1f;
		pButton->m_width = 0.2f;
	}
}

void HowToPlayState::OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton - 1);
	g_AudioSystem->PlaySound("MENU_Hover");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void HowToPlayState::OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton + 1);
	g_AudioSystem->PlaySound("MENU_Hover");	
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void HowToPlayState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = STATE_TYPE::MAIN_MENU_STATE;
	g_AudioSystem->PlaySound("MENU_Back");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void HowToPlayState::SetSelectButton(void)
{
	//ResetButtonColors();
	ShrinkAllButtons();

	switch (m_unCurrentButton)
	{
	case 0:
	{		
		GrowSelectedButton(m_pPreviousButtonSelected);
		m_SelectedState = STATE_TYPE::HOW_TO_PLAY_STATE;
	}
		break;
	case 1:
	{		
		GrowSelectedButton(m_pNextButtonSelected);
		m_SelectedState = STATE_TYPE::HOW_TO_PLAY_STATE;

	}
		break;
	case 2:
	{
		GrowSelectedButton(m_pBackToMainMenuSelected);
		m_SelectedState = STATE_TYPE::MAIN_MENU_STATE;
	}
		break;
	default:
		break;
	}
}

void HowToPlayState::SetInstructionScreen(void)
{
	ShrinkAllInstructionScreens();

	switch (m_unCurrentInstructionScreen)
	{
	case 0:
		EnlargeInstructionScreen(m_pInstructions1);
		break;
	case 1:
		EnlargeInstructionScreen(m_pInstructions2);
		break;
	case 2:
		EnlargeInstructionScreen(m_pInstructions3);
		break;
	case 3:
		EnlargeInstructionScreen(m_pInstructions4);
		break;
	case 4:
		EnlargeInstructionScreen(m_pInstructions5);
		break;
	case 5:
		EnlargeInstructionScreen(m_pInstructions6);
		break;
	case 6:
		EnlargeInstructionScreen(m_pInstructions7);
		break;
	case 7:
		EnlargeInstructionScreen(m_pInstructions8);
		break;
	case 8:
		EnlargeInstructionScreen(m_pInstructions9);
		break;
	default:
		break;
	}
}


bool HowToPlayState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

	if (fMouseX == fMousePrevX && fMouseY == fMousePrevY)
		return false;

	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_pPreviousButton->GetComponent("RenderComp");
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

	// Check Buttons - Previous Button


	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		return true;
	}

	//Check Buttons - Back To Main Menu Button
	pButton = (RenderComp*)m_pBackToMainMenu->GetComponent("RenderComp");
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

	// Check Buttons- Next Button
	pButton = (RenderComp*)m_pNextButton->GetComponent("RenderComp");
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

	return false;
}

bool HowToPlayState::Handle360Input()
{
	GamePad tmpPad = gamePad->GetState();
	bool detectedInput = false;

	if (tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton + 1);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton - 1);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{
		if (m_SelectedState == STATE_TYPE::MAIN_MENU_STATE)
		{
			m_NextState = m_SelectedState;
			g_AudioSystem->PlaySound("MENU_Back");
		}
		else if (m_unCurrentButton == 0)
		{
			SetCurrentInstructionScreen(m_unCurrentInstructionScreen - 1);
			SetInstructionScreen();
			g_AudioSystem->PlaySound("MENU_Accept");

		}
		else if (m_unCurrentButton == 1)
		{
			SetCurrentInstructionScreen(m_unCurrentInstructionScreen + 1);
			SetInstructionScreen();
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		detectedInput = true;

	}



	return detectedInput;
}

void HowToPlayState::SetCurrentButton(int numButton)
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

void HowToPlayState::SetCurrentInstructionScreen(int numInstructionScreen)
{
	if (numInstructionScreen > 8)
	{
		m_unCurrentInstructionScreen = 0;
	}
	else if (numInstructionScreen < 0)
	{
		m_unCurrentInstructionScreen = 8;
	}
	else
	{
		m_unCurrentInstructionScreen = (unsigned int)numInstructionScreen;
	}
}

void HowToPlayState::ResetButtonColors()
{
	SetObjectColor(m_pNextButton, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetObjectColor(m_pPreviousButton, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetObjectColor(m_pBackToMainMenu, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

void HowToPlayState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (m_SelectedState == STATE_TYPE::MAIN_MENU_STATE)
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Back");
	}
	else if (m_unCurrentButton == 0)
	{
		SetCurrentInstructionScreen(m_unCurrentInstructionScreen - 1);
		SetInstructionScreen();
		g_AudioSystem->PlaySound("MENU_Accept");

	}
	else if (m_unCurrentButton == 1)
	{
		SetCurrentInstructionScreen(m_unCurrentInstructionScreen + 1);
		SetInstructionScreen();
		g_AudioSystem->PlaySound("MENU_Accept");
	}

}

void HowToPlayState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (CheckMouseMove() || globalGraphicsPointer->GetCrosshairShowing())
	{
		SetSelectButton();
		m_NextState = m_SelectedState;
		if (m_NextState != STATE_TYPE::MAIN_MENU_STATE)
		{
			if (m_unCurrentButton == 0)
			{
				SetCurrentInstructionScreen(m_unCurrentInstructionScreen - 1);
				SetInstructionScreen();
				g_AudioSystem->PlaySound("MENU_Accept");
			}
			if (m_unCurrentButton == 1)
			{
				SetCurrentInstructionScreen(m_unCurrentInstructionScreen + 1);
				SetInstructionScreen();
				g_AudioSystem->PlaySound("MENU_Accept");
			}

		}
		else
		{
			g_AudioSystem->PlaySound("MENU_Back");

		}
	}
	globalGraphicsPointer->SetCrosshairShowing(true);

}

void HowToPlayState::EnlargeInstructionScreen(GameObject* instructionScreen)
{
	RenderComp* instructions = (RenderComp*)instructionScreen->GetComponent("RenderComp");
	if (instructions != nullptr)
	{
		instructions->m_posX = -0.005f;
		instructions->m_posY = -0.005f;
		instructions->m_height = 1.01f;
		instructions->m_width = 1.01f;
	}
}

void HowToPlayState::ShrinkInstructionScreen(GameObject* instructionScreen)
{
	RenderComp* instructions = (RenderComp*)instructionScreen->GetComponent("RenderComp");
	if (instructions != nullptr)
	{
		instructions->m_posX = 0.0f;
		instructions->m_posY = 0.0f;
		instructions->m_height = 0.0f;
		instructions->m_width = 0.0f;
	}
}

void HowToPlayState::ShrinkAllInstructionScreens()
{
	ShrinkInstructionScreen(m_pInstructions1);
	ShrinkInstructionScreen(m_pInstructions2);
	ShrinkInstructionScreen(m_pInstructions3);
	ShrinkInstructionScreen(m_pInstructions4);
	ShrinkInstructionScreen(m_pInstructions5);
	ShrinkInstructionScreen(m_pInstructions6);
	ShrinkInstructionScreen(m_pInstructions7);
	ShrinkInstructionScreen(m_pInstructions8);
	ShrinkInstructionScreen(m_pInstructions9);
}

void HowToPlayState::SetObjectColor(GameObject* obj, XMFLOAT4 color)
{
	RenderComp* renderComp = (RenderComp*)obj->GetComponent("RenderComp");
	if (renderComp)
	{
		renderComp->color = color;
	}
}

void HowToPlayState::ShrinkAllButtons()
{
	RenderComp* pButton = (RenderComp*)m_pNextButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pPreviousButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pBackToMainMenuSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}
}

void HowToPlayState::GrowSelectedButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.1f;
		pButton->m_width = 0.2f;
	}
}