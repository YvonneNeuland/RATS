#include "stdafx.h"
#include "GraphicsOptionsState.h"
#include "../Input/InputController.h"
#include "../Renderer/D3DGraphics.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../Upgrade System/GameData.h"
#include "../TinyXml/tinystr.h"
#include "../TinyXml/tinyxml.h"
#include <cassert>
#include <fstream>

#include "../../XTime.h"

extern InputController* m_controller;
extern D3DGraphics* globalGraphicsPointer;
extern	FFmod*		g_AudioSystem;
extern ThreeSixty* gamePad;
extern GameData* gameData;
extern XTime* m_Xtime;


GraphicsOptionsState::GraphicsOptionsState()
{
	dwPacketNo = 0;
}


GraphicsOptionsState::~GraphicsOptionsState()
{
}

void GraphicsOptionsState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_NextState = STATE_TYPE::GRAPHICS_MENU_STATE;

	m_unCurrentButton = 0;

	if (m_controller)
	{
		m_controller->SetGraphicsKeys();
	}
	using namespace Events;
	//Setup Input
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &GraphicsOptionsState::OnEnter);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &GraphicsOptionsState::OnEscape);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Up"), this, &GraphicsOptionsState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Down"), this, &GraphicsOptionsState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Left"), this, &GraphicsOptionsState::OnArrowLeft);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Right"), this, &GraphicsOptionsState::OnArrowRight);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "UpArr"), this, &GraphicsOptionsState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownArr"), this, &GraphicsOptionsState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "LeftArr"), this, &GraphicsOptionsState::OnArrowLeft);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyDown", "RightArr"), this, &GraphicsOptionsState::OnArrowRight);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &GraphicsOptionsState::OnMouseClick);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyDown", "MouseLDown"), this, &GraphicsOptionsState::OnMouseDown);
	
	dwPacketNo = gamePad->GetState().state.dwPacketNumber;
}

void GraphicsOptionsState::Exit()
{

	using namespace Events;

	EventManager()->UnregisterClient("Confirm", this, &GraphicsOptionsState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &GraphicsOptionsState::OnEscape);
	EventManager()->UnregisterClient("Up", this, &GraphicsOptionsState::OnArrowUp);
	EventManager()->UnregisterClient("Down", this, &GraphicsOptionsState::OnArrowDown);
	EventManager()->UnregisterClient("UpArr", this, &GraphicsOptionsState::OnArrowUp);
	EventManager()->UnregisterClient("DownArr", this, &GraphicsOptionsState::OnArrowDown);
	EventManager()->UnregisterClient("LeftArr", this, &GraphicsOptionsState::OnArrowLeft);
	EventManager()->UnregisterClient("RightArr", this, &GraphicsOptionsState::OnArrowRight);
	EventManager()->UnregisterClient("MouseLClick", this, &GraphicsOptionsState::OnMouseClick);
	EventManager()->UnregisterClient("MouseLDown", this, &GraphicsOptionsState::OnMouseDown);


	if (m_controller)
	{
		m_controller->ClearGraphicsKeys();
	}

	SaveGraphicsSettings();
}

void GraphicsOptionsState::Initialize(Renderer* renderer)
{
	m_Renderer = renderer;
	m_ObjectManager.Initialize();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory(&m_ObjectFactory);


	//Set Up Buttons
	m_vButtons.clear();
	m_vButtons.push_back(0);
	m_vButtons.push_back(1);
	m_vButtons.push_back(2);
	m_vButtons.push_back(3);
	m_vButtons.push_back(4);
	m_vButtons.push_back(5);

	bool success = LoadGraphicsSettings();
	CreateHud();


}

void GraphicsOptionsState::Update(float dt)
{

	globalGraphicsPointer->MapGamma(m_fGamma);

	// KEYBOARD
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
		}
	}

	SetSelectButton();
	UpdateSliderBar();
	
	
}

void GraphicsOptionsState::Terminate()
{
	m_vButtons.clear();
}

//Input Functions
void GraphicsOptionsState::OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton - 1);
	g_AudioSystem->PlaySound("MENU_Hover");

}

void GraphicsOptionsState::OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;


	SetCurrentButton(m_unCurrentButton + 1);
	g_AudioSystem->PlaySound("MENU_Hover");

}

void GraphicsOptionsState::OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	g_AudioSystem->PlaySound("MENU_Hover");

	switch (currentMode)
	{
	case NO_MODE:
		SetKey(m_nKeyIndex - 1);
		break;
	case SCREEN_RESOLUTION:
		SetResolution(m_nResolutionIndex - 1);
		break;
	case WINDOW_MODE:
		SetWindowMode(m_nWindowModeIndex - 1);
		break;
	case MSAA:
		SetMSAALevel(m_nMSAALevelIndex - 1);
		break;
	case VSYNC:
		SetVSync(m_nVSyncIndex - 1);
		break;
	case GAMMA:
		TurnGammaDown();
		break;
	default:
		break;
	}
}

void GraphicsOptionsState::OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	g_AudioSystem->PlaySound("MENU_Hover");

	switch (currentMode)
	{
	case NO_MODE:
		SetKey(m_nKeyIndex + 1);
		break;
	case SCREEN_RESOLUTION:
		SetResolution(m_nResolutionIndex + 1);
		break;
	case WINDOW_MODE:
		SetWindowMode(m_nWindowModeIndex + 1);
		break;
	case MSAA:
		SetMSAALevel(m_nMSAALevelIndex + 1);
		break;
	case VSYNC:
		SetVSync(m_nVSyncIndex + 1);
		break;
	case GAMMA:
		TurnGammaUp();
		break;
	default:
		break;
	}
}

void GraphicsOptionsState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (m_NextState != m_SelectedState)
	{
		m_NextState = m_SelectedState;
		g_AudioSystem->PlaySound("MENU_Back");

	}
	else if (currentMode == GRAPHIC_MODE::NO_MODE && m_nKeyIndex == 0)
	{
		globalGraphicsPointer->ApplyGraphicsSettings(m_nMSAALevelIndex, m_nWindowModeIndex, m_nResolutionIndex, m_nVSyncIndex, m_fGamma);
		g_AudioSystem->PlaySound("MENU_Accept");
	}
	else
	{		
		g_AudioSystem->PlaySound("MENU_Error");
	}
}

void GraphicsOptionsState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = STATE_TYPE::OPTIONS_MENU_STATE;
	g_AudioSystem->PlaySound("MENU_Back");
}

void GraphicsOptionsState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;


	if (CheckMouseMove())
	{
		SetSelectButton();		

		using namespace Events;

		MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

		RECT clientRect;
		RenderComp* pButton = (RenderComp*)m_pResolutionLeftArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (!globalGraphicsPointer)
			return;

		GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

		fMouseX = fMouseX / (clientRect.right - clientRect.left);
		fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

		// Check Buttons - Screen Resolution Button

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetResolution(m_nResolutionIndex - 1);		
			g_AudioSystem->PlaySound("MENU_Accept");
		}		

		pButton = (RenderComp*)m_pResolutionRightArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetResolution(m_nResolutionIndex + 1);		
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		//Check Buttons - Windowed Mode
		pButton = (RenderComp*)m_pWindowModeLeftArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetWindowMode(m_nWindowModeIndex - 1);
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		pButton = (RenderComp*)m_pWindowModeRightArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetWindowMode(m_nWindowModeIndex + 1);
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		// Check Buttons- MSAA Level
		pButton = (RenderComp*)m_pMSAALevelLeftArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetMSAALevel(m_nMSAALevelIndex - 1);	
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		pButton = (RenderComp*)m_pMSAALevelRightArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetMSAALevel(m_nMSAALevelIndex + 1);		
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		//Check Buttons - VSync		
		pButton = (RenderComp*)m_pVSyncLeftArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetVSync(m_nVSyncIndex - 1);			
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		pButton = (RenderComp*)m_pVSyncRightArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			SetVSync(m_nVSyncIndex + 1);	
			g_AudioSystem->PlaySound("MENU_Accept");
		}

		// Check Buttons- Back Button
		pButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			m_NextState = m_SelectedState;			
			g_AudioSystem->PlaySound("MENU_Back");			
		}

		// Check Buttons - Apply Button
		pButton = (RenderComp*)m_pApplyButton->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			globalGraphicsPointer->ApplyGraphicsSettings(m_nMSAALevelIndex, m_nWindowModeIndex, m_nResolutionIndex, m_nVSyncIndex, m_fGamma);		
			g_AudioSystem->PlaySound("MENU_Accept");
		}
	}
}

void GraphicsOptionsState::OnMouseDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (CheckMouseMove() || globalGraphicsPointer->GetCrosshairShowing())
	{
		SetSelectButton();
		m_NextState = m_SelectedState;
		//if (m_unCurrentButton == 3)
		//{
		//	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
		//	g_AudioSystem->PlaySound("MENU_Accept");

		//}


		using namespace Events;

		MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

		RECT clientRect;
		RenderComp* pButton = (RenderComp*)m_pGammaLeftArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (!globalGraphicsPointer)
			return;

		GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

		fMouseX = fMouseX / (clientRect.right - clientRect.left);
		fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

		//Check Buttons - BGM LEFT ARR
		pButton = (RenderComp*)m_pGammaLeftArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			TurnGammaDown();
			return;
		}

		//Check Buttons - BGM RIGHT ARR
		pButton = (RenderComp*)m_pGammaRightArrow->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			TurnGammaUp();
			return;
		}

		//Check Buttons - BGM SLIDER INDICATOR
		pButton = (RenderComp*)m_pGammaSliderBar->GetComponent("RenderComp");
		if (!pButton)
		{
			return;
		}

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{

			float xDiff = globalGraphicsPointer->MOUSE_X - pButton->m_posX;
			float percent = (xDiff / pButton->m_width) * 100;

			m_nGamma = CapGamma(percent);
			m_fGamma = m_nGamma / 100.0f;

			//if (m_fOldMouseXGamma == 0.0f)
			//{
			//	m_fOldMouseXGamma = fMouseX;
			//}
			//else
			//{
			//	if (fMouseX > m_fOldMouseXGamma)
			//	{
			//		TurnGammaUp();
			//	}
			//	else if (fMouseX < m_fOldMouseXGamma)
			//	{
			//		TurnGammaDown();
			//	}

			//	m_fOldMouseXGamma = fMouseX;
			//}
		}
		else
		{
			m_fOldMouseXGamma = 0.0f;
		}
	}
}

//Private Functions
void GraphicsOptionsState::CreateHud(void)
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

	////////////////////
	//SCREEN RESOLUTION
	///////////////////

	//Draw Screen Resolution Button
	m_pResolutionButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "screenResolutionButton");
	RenderComp* screenResolutionButton = (RenderComp*)m_pResolutionButton->GetComponent("RenderComp");
	if (screenResolutionButton != nullptr)
	{
		screenResolutionButton->m_posX = 0.15f;
		screenResolutionButton->m_posY = 0.1f;
		screenResolutionButton->m_height = 0.125f;
		screenResolutionButton->m_width = 0.3f;
	}

	m_pResolutionButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "screenResolutionButtonSelected");
	screenResolutionButton = (RenderComp*)m_pResolutionButtonSelected->GetComponent("RenderComp");
	if (screenResolutionButton != nullptr)
	{
		screenResolutionButton->m_posX = 0.15f;
		screenResolutionButton->m_posY = 0.1f;
		screenResolutionButton->m_height = 0.0f;
		screenResolutionButton->m_width = 0.0f;
	}

	//Draw Screen Resolution Left Arrow
	m_pResolutionLeftArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow");
	RenderComp* screenResolutionLeftArrow = (RenderComp*)m_pResolutionLeftArrow->GetComponent("RenderComp");
	if (screenResolutionLeftArrow != nullptr)
	{
		screenResolutionLeftArrow->m_posX = 0.46f;
		screenResolutionLeftArrow->m_posY = 0.15f;
		screenResolutionLeftArrow->m_height = 0.05f;
		screenResolutionLeftArrow->m_width = 0.05f;
	}

	//Draw Screen Resolution Right Arrow
	m_pResolutionRightArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow");
	RenderComp* screenResolutionRightArrow = (RenderComp*)m_pResolutionRightArrow->GetComponent("RenderComp");
	if (screenResolutionRightArrow != nullptr)
	{
		screenResolutionRightArrow->m_posX = 0.83f;
		screenResolutionRightArrow->m_posY = 0.15f;
		screenResolutionRightArrow->m_height = 0.05f;
		screenResolutionRightArrow->m_width = 0.05f;
	}

	//Draw Selected Screen Resolution
	string res = globalGraphicsPointer->GetResolutionState(m_nResolutionIndex).m_displayString;
	m_pResolution = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), res, eRENDERCOMP_TYPE::Render2D_Text);
	RenderComp* resolution = (RenderComp*)m_pResolution->GetComponent("RenderComp");

	if (resolution != nullptr)
	{
		resolution->m_posX = 0.55f;
		resolution->m_posY = 0.15f;
		resolution->m_height = 0.2f;
		resolution->m_width = 0.2f;

		resolution->color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	}

	///////////////
	//WINDOW MODE
	//////////////

	//Draw Window Mode Button
	m_pWindowModeButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "windowModeButton");
	RenderComp* windowModeButton = (RenderComp*)m_pWindowModeButton->GetComponent("RenderComp");
	if (windowModeButton != nullptr)
	{
		windowModeButton->m_posX = 0.15f;
		windowModeButton->m_posY = 0.225f;
		windowModeButton->m_height = 0.125f;
		windowModeButton->m_width = 0.3f;
	}

	m_pWindowModeButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "windowModeButtonSelected");
	windowModeButton = (RenderComp*)m_pWindowModeButtonSelected->GetComponent("RenderComp");
	if (windowModeButton != nullptr)
	{
		windowModeButton->m_posX = 0.15f;
		windowModeButton->m_posY = 0.225f;
		windowModeButton->m_height = 0.0f;
		windowModeButton->m_width = 0.0f;
	}

	//Draw Window Mode Left Arrow
	m_pWindowModeLeftArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow");
	RenderComp* windowModeLeftArrow = (RenderComp*)m_pWindowModeLeftArrow->GetComponent("RenderComp");
	if (windowModeLeftArrow != nullptr)
	{
		windowModeLeftArrow->m_posX = 0.46f;
		windowModeLeftArrow->m_posY = 0.27f;
		windowModeLeftArrow->m_height = 0.05f;
		windowModeLeftArrow->m_width = 0.05f;
	}

	//Draw Window Mode Right Arrow
	m_pWindowModeRightArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow");
	RenderComp* windowModeRightArrow = (RenderComp*)m_pWindowModeRightArrow->GetComponent("RenderComp");
	if (windowModeRightArrow != nullptr)
	{
		windowModeRightArrow->m_posX = 0.83f;
		windowModeRightArrow->m_posY = 0.27f;
		windowModeRightArrow->m_height = 0.05f;
		windowModeRightArrow->m_width = 0.05f;
	}

	//Draw Window mode
	string winMode = globalGraphicsPointer->GetScreenState(m_nWindowModeIndex).m_displayString;
	m_pWindowMode = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), winMode, eRENDERCOMP_TYPE::Render2D_Text);
	RenderComp* windowMode = (RenderComp*)m_pWindowMode->GetComponent("RenderComp");

	if (windowMode != nullptr)
	{
		windowMode->m_posX = 0.55f;
		windowMode->m_posY = 0.27f;
		windowMode->m_height = 0.2f;
		windowMode->m_width = 0.2f;

		windowMode->color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	}

	/////////////
	//MSAA LEVEL
	////////////

	//Draw MSAA Level Button
	m_pMSAALevelButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "msaaLevelButton");
	RenderComp* msaaLevelButton = (RenderComp*)m_pMSAALevelButton->GetComponent("RenderComp");
	if (msaaLevelButton != nullptr)
	{
		msaaLevelButton->m_posX = 0.15f;
		msaaLevelButton->m_posY = 0.35f;
		msaaLevelButton->m_height = 0.125f;
		msaaLevelButton->m_width = 0.3f;
	}

	m_pMSAALevelButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "msaaLevelButtonSelected");
	msaaLevelButton = (RenderComp*)m_pMSAALevelButtonSelected->GetComponent("RenderComp");
	if (msaaLevelButton != nullptr)
	{
		msaaLevelButton->m_posX = 0.15f;
		msaaLevelButton->m_posY = 0.35f;
		msaaLevelButton->m_height = 0.0f;
		msaaLevelButton->m_width = 0.0f;
	}

	//Draw MSAA Level Left Arrow
	m_pMSAALevelLeftArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow");
	RenderComp* msaaLevelLeftArrow = (RenderComp*)m_pMSAALevelLeftArrow->GetComponent("RenderComp");
	if (msaaLevelLeftArrow != nullptr)
	{
		msaaLevelLeftArrow->m_posX = 0.46f;
		msaaLevelLeftArrow->m_posY = 0.395f;
		msaaLevelLeftArrow->m_height = 0.05f;
		msaaLevelLeftArrow->m_width = 0.05f;
	}

	//Draw MSAA Level Right Arrow
	m_pMSAALevelRightArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow");
	RenderComp* msaaLevelRightArrow = (RenderComp*)m_pMSAALevelRightArrow->GetComponent("RenderComp");
	if (msaaLevelRightArrow != nullptr)
	{
		msaaLevelRightArrow->m_posX = 0.83f;
		msaaLevelRightArrow->m_posY = 0.395f;
		msaaLevelRightArrow->m_height = 0.05f;
		msaaLevelRightArrow->m_width = 0.05f;
	}

	//Draw MSAA
	string msaa = globalGraphicsPointer->GetMSAAState(m_nMSAALevelIndex).m_displayString;
	m_pMSAALevel = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), msaa, eRENDERCOMP_TYPE::Render2D_Text);
	RenderComp* msaaLevel = (RenderComp*)m_pMSAALevel->GetComponent("RenderComp");

	if (msaaLevel != nullptr)
	{
		msaaLevel->m_posX = 0.625f;
		msaaLevel->m_posY = 0.395f;
		msaaLevel->m_height = 0.2f;
		msaaLevel->m_width = 0.2f;
		
		msaaLevel->color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	}

	////////
	//VSYNC
	///////

	//Draw VSync Button
	m_pVSyncButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "vsyncButton");
	RenderComp* vsyncButton = (RenderComp*)m_pVSyncButton->GetComponent("RenderComp");
	if (vsyncButton != nullptr)
	{
		vsyncButton->m_posX = 0.15f;
		vsyncButton->m_posY = 0.475f;
		vsyncButton->m_height = 0.125f;
		vsyncButton->m_width = 0.3f;
	}

	m_pVSyncButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "vsyncButtonSelected");
	vsyncButton = (RenderComp*)m_pVSyncButtonSelected->GetComponent("RenderComp");
	if (vsyncButton != nullptr)
	{
		vsyncButton->m_posX = 0.15f;
		vsyncButton->m_posY = 0.475f;
		vsyncButton->m_height = 0.0f;
		vsyncButton->m_width = 0.0f;
	}

	//Draw VSync Left Arrow
	m_pVSyncLeftArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow");
	RenderComp* vsyncLeftArrow = (RenderComp*)m_pVSyncLeftArrow->GetComponent("RenderComp");
	if (vsyncLeftArrow != nullptr)
	{
		vsyncLeftArrow->m_posX = 0.46f;
		vsyncLeftArrow->m_posY = 0.52f;
		vsyncLeftArrow->m_height = 0.05f;
		vsyncLeftArrow->m_width = 0.05f;
	}

	//Draw VSync Right Arrow
	m_pVSyncRightArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow");
	RenderComp* vsyncRightArrow = (RenderComp*)m_pVSyncRightArrow->GetComponent("RenderComp");
	if (vsyncRightArrow != nullptr)
	{
		vsyncRightArrow->m_posX = 0.83f;
		vsyncRightArrow->m_posY = 0.52f;
		vsyncRightArrow->m_height = 0.05f;
		vsyncRightArrow->m_width = 0.05f;
	}

	//Draw VSync
	string vsync = globalGraphicsPointer->GetVsyncState(m_nVSyncIndex).m_displayString;
	m_pVSync = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), vsync, eRENDERCOMP_TYPE::Render2D_Text);
	RenderComp* VSync = (RenderComp*)m_pVSync->GetComponent("RenderComp");

	if (VSync != nullptr)
	{
		VSync->m_posX = 0.625f;
		VSync->m_posY = 0.52f;
		VSync->m_height = 0.2f;
		VSync->m_width = 0.2f;

		VSync->color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	}

	/////////
	//GAMMA
	////////
	//Draw GAMMA Button
	m_pGammaButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "gammaButton");
	RenderComp* gammaButton = (RenderComp*)m_pGammaButton->GetComponent("RenderComp");
	if (gammaButton != nullptr)
	{
		gammaButton->m_posX = 0.15f;
		gammaButton->m_posY = 0.6f;
		gammaButton->m_height = 0.125f;
		gammaButton->m_width = 0.3f;
	}

	m_pGammaButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "gammaButtonSelected");
	gammaButton = (RenderComp*)m_pGammaButtonSelected->GetComponent("RenderComp");
	if (gammaButton != nullptr)
	{
		gammaButton->m_posX = 0.15f;
		gammaButton->m_posY = 0.6f;
		gammaButton->m_height = 0.0f;
		gammaButton->m_width = 0.0f;
	}

	//Draw GAMMA Left Arrow
	m_pGammaLeftArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow");
	RenderComp* gammaLeftArrow = (RenderComp*)m_pGammaLeftArrow->GetComponent("RenderComp");
	if (gammaLeftArrow != nullptr)
	{
		gammaLeftArrow->m_posX = 0.46f;
		gammaLeftArrow->m_posY = 0.645f;
		gammaLeftArrow->m_height = 0.05f;
		gammaLeftArrow->m_width = 0.05f;
	}

	//Draw GAMMA Right Arrow
	m_pGammaRightArrow = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow");
	RenderComp* gammaRightArrow = (RenderComp*)m_pGammaRightArrow->GetComponent("RenderComp");
	if (gammaRightArrow != nullptr)
	{
		gammaRightArrow->m_posX = 0.83f;
		gammaRightArrow->m_posY = 0.645f;
		gammaRightArrow->m_height = 0.05f;
		gammaRightArrow->m_width = 0.05f;
	}

	//Create the GAMMA Slider Indicator
	m_pGammaSliderBarIndicator = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sliderIndicator", Render2D_Front);
	RenderComp* gammaSliderIndicator = (RenderComp*)m_pGammaSliderBarIndicator->GetComponent("RenderComp");
	if (gammaSliderIndicator != nullptr)
	{
		gammaSliderIndicator->m_posX = 0.6f;
		gammaSliderIndicator->m_posY = 0.635f;
		gammaSliderIndicator->m_height = 0.0872f;
		gammaSliderIndicator->m_width = 0.02f;
	}

	//Create the GAMMA Slider Bar
	m_pGammaSliderBar = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sliderBar", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* gammaSliderBar = (RenderComp*)m_pGammaSliderBar->GetComponent("RenderComp");
	if (gammaSliderBar != nullptr)
	{
		gammaSliderBar->m_posX = 0.495f;
		gammaSliderBar->m_posY = 0.63275f;
		gammaSliderBar->m_height = 0.0875f;
		gammaSliderBar->m_width = 0.35f;
	}

	///////
	//BACK
	//////
	//Create the Back Button
	m_pBackButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButton");
	RenderComp* backButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
	if (backButton != nullptr)
	{
		backButton->m_posX = 0.6f;
		backButton->m_posY = 0.81f;
		backButton->m_height = 0.125f;
		backButton->m_width = 0.3f;
	}

	m_pBackButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButtonSelected");
	backButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
	if (backButton != nullptr)
	{
		backButton->m_posX = 0.6f;
		backButton->m_posY = 0.81f;
		backButton->m_height = 0.0f;
		backButton->m_width = 0.0f;
	}

	//Create the Apply Button
	m_pApplyButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "ApplyButton");
	RenderComp* applyButton = (RenderComp*)m_pApplyButton->GetComponent("RenderComp");
	if (applyButton != nullptr)
	{
		applyButton->m_posX = 0.25f;
		applyButton->m_posY = 0.81f;
		applyButton->m_height = 0.125f;
		applyButton->m_width = 0.3f;
	}

	m_pApplyButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "ApplyButtonSelected");
	applyButton = (RenderComp*)m_pApplyButtonSelected->GetComponent("RenderComp");
	if (applyButton != nullptr)
	{
		applyButton->m_posX = 0.25f;
		applyButton->m_posY = 0.81f;
		applyButton->m_height = 0.0f;
		applyButton->m_width = 0.0f;
	}
}

//Input Helper Functions
void GraphicsOptionsState::SetSelectButton(void)
{
	ShrinkAllButtons();
	//ResetButtonColors();

	switch (m_unCurrentButton)
	{
	case 0:
	{
		
		GrowSelectedButton(m_pResolutionButtonSelected);
		currentMode = GRAPHIC_MODE::SCREEN_RESOLUTION;
		m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;

	}
		break;
	case 1:
	{
		GrowSelectedButton(m_pWindowModeButtonSelected);
		currentMode = GRAPHIC_MODE::WINDOW_MODE;
		m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;

	}
		break;
	case 2:
	{
		GrowSelectedButton(m_pMSAALevelButtonSelected);
		currentMode = GRAPHIC_MODE::MSAA;
		m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;
	}
		break;
	case 3:
	{
		GrowSelectedButton(m_pVSyncButtonSelected);
		currentMode = GRAPHIC_MODE::VSYNC;
		m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;
	}
		break;
	case 4:
	{
		GrowSelectedButton(m_pGammaButtonSelected);
		currentMode = GRAPHIC_MODE::GAMMA;
		m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;
	}
		break;
	case 5:
	{
		currentMode = GRAPHIC_MODE::NO_MODE;

		if (m_nKeyIndex == 0)
		{
			RenderComp* pButton = (RenderComp*)m_pApplyButtonSelected->GetComponent("RenderComp");
			if (pButton)
			{
				pButton->m_height = 0.125f;
				pButton->m_width = 0.3f;
			}
			m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;
		}
		else
		{
			RenderComp* pButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
			if (pButton)
			{
				pButton->m_height = 0.125f;
				pButton->m_width = 0.3f;
			}
			m_SelectedState = STATE_TYPE::OPTIONS_MENU_STATE;
		}
	}
		break;
	default:
		break;
	}
}

bool GraphicsOptionsState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_pResolutionButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (!globalGraphicsPointer)
		return false;

	GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

	fMouseX = fMouseX / (clientRect.right - clientRect.left);
	fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

	// Check Buttons - Screen Resolution Button


	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		currentMode = GRAPHIC_MODE::SCREEN_RESOLUTION;
		return true;
	}

	pButton = (RenderComp*)m_pResolutionLeftArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		currentMode = GRAPHIC_MODE::SCREEN_RESOLUTION;
		return true;
	}

	pButton = (RenderComp*)m_pResolutionRightArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		currentMode = GRAPHIC_MODE::SCREEN_RESOLUTION;
		return true;
	}

	//Check Buttons - Windowed Mode
	pButton = (RenderComp*)m_pWindowModeButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 1)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 1;
		currentMode = GRAPHIC_MODE::WINDOW_MODE;
		return true;
	}

	pButton = (RenderComp*)m_pWindowModeLeftArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 1)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 1;
		currentMode = GRAPHIC_MODE::WINDOW_MODE;
		return true;
	}

	pButton = (RenderComp*)m_pWindowModeRightArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 1)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 1;
		currentMode = GRAPHIC_MODE::WINDOW_MODE;
		return true;
	}

	// Check Buttons- MSAA Level
	pButton = (RenderComp*)m_pMSAALevelButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 2)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 2;
		currentMode = GRAPHIC_MODE::MSAA;
		return true;
	}

	pButton = (RenderComp*)m_pMSAALevelLeftArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 2)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 2;
		currentMode = GRAPHIC_MODE::MSAA;
		return true;
	}

	pButton = (RenderComp*)m_pMSAALevelRightArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 2)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 2;
		currentMode = GRAPHIC_MODE::MSAA;
		return true;
	}

	//Check Buttons - VSync
	pButton = (RenderComp*)m_pVSyncButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 3)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 3;
		currentMode = GRAPHIC_MODE::VSYNC;
		return true;
	}

	pButton = (RenderComp*)m_pVSyncLeftArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 3)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 3;
		currentMode = GRAPHIC_MODE::VSYNC;
		return true;
	}

	pButton = (RenderComp*)m_pVSyncRightArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 3)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 3;
		currentMode = GRAPHIC_MODE::VSYNC;
		return true;
	}

	//Check Buttons - Gamma
	pButton = (RenderComp*)m_pGammaButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 4)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 4;
		currentMode = GRAPHIC_MODE::GAMMA;
		return true;
	}

	pButton = (RenderComp*)m_pGammaLeftArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 4)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 4;
		currentMode = GRAPHIC_MODE::GAMMA;
		return true;
	}

	pButton = (RenderComp*)m_pGammaRightArrow->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 4)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 4;
		currentMode = GRAPHIC_MODE::GAMMA;
		return true;
	}

	pButton = (RenderComp*)m_pGammaSliderBar->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 4)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 4;
		currentMode = GRAPHIC_MODE::GAMMA;
		return true;
	}

	pButton = (RenderComp*)m_pGammaSliderBarIndicator->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 4)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 4;
		currentMode = GRAPHIC_MODE::GAMMA;
		return true;
	}

	// Check Buttons - Back Button
	pButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 5 || m_nKeyIndex != 1)
			g_AudioSystem->PlaySound("MENU_Hover");
		currentMode = GRAPHIC_MODE::NO_MODE;
		m_nKeyIndex = 1;
		m_unCurrentButton = 5;
		return true;
	}

	pButton = (RenderComp*)m_pApplyButton->GetComponent("RenderComp");
	if (!pButton)
		return false;

	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 5 || m_nKeyIndex != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 5;
		m_nKeyIndex = 0;
		currentMode = GRAPHIC_MODE::NO_MODE;
		return true;
	}

	return false;
}

bool GraphicsOptionsState::Handle360Input()
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

		g_AudioSystem->PlaySound("MENU_Hover");

		switch (currentMode)
		{
		case NO_MODE:
			SetKey(m_nKeyIndex - 1);
			break;
		case SCREEN_RESOLUTION:
			SetResolution(m_nResolutionIndex - 1);
			break;
		case WINDOW_MODE:
			SetWindowMode(m_nWindowModeIndex - 1);
			break;
		case MSAA:
			SetMSAALevel(m_nMSAALevelIndex - 1);
			break;
		case VSYNC:
			SetVSync(m_nVSyncIndex - 1);
			break;
		case GAMMA:
			TurnGammaDown();
			break;
		default:
			break;
		}
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bHeld)
	{

		g_AudioSystem->PlaySound("MENU_Hover");

		switch (currentMode)
		{
		case NO_MODE:
			SetKey(m_nKeyIndex + 1);
			break;
		case SCREEN_RESOLUTION:
			SetResolution(m_nResolutionIndex + 1);
			break;
		case WINDOW_MODE:
			SetWindowMode(m_nWindowModeIndex + 1);
			break;
		case MSAA:
			SetMSAALevel(m_nMSAALevelIndex + 1);
			break;
		case VSYNC:
			SetVSync(m_nVSyncIndex + 1);
			break;
		case GAMMA:
			TurnGammaUp();
			break;
		default:
			break;
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

void GraphicsOptionsState::SetCurrentButton(int numButton)
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

void GraphicsOptionsState::ShrinkAllButtons()
{
	RenderComp* pButton = (RenderComp*)m_pResolutionButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pWindowModeButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pMSAALevelButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pVSyncButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pApplyButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pGammaButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}
}

void GraphicsOptionsState::GrowSelectedButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.125f;
		pButton->m_width = 0.3f;
	}
}

void GraphicsOptionsState::ResetButtonColors()
{
	SetObjectColor(m_pBackButton, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetObjectColor(m_pApplyButton, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));	
}

void GraphicsOptionsState::SetObjectColor(GameObject* obj, XMFLOAT4 color)
{
	RenderComp* renderComp = (RenderComp*)obj->GetComponent("RenderComp");
	if (renderComp)
	{
		renderComp->color = color;
	}
}

void GraphicsOptionsState::SetResolution(int index)
{
	if (index < 0)
	{
		m_nResolutionIndex = globalGraphicsPointer->GetNumResolutions() - 1;
	}
	else if (((UINT)index) > globalGraphicsPointer->GetNumResolutions() - 1)
	{
		m_nResolutionIndex = 0;
	}
	else
	{
		m_nResolutionIndex = index;
	}

	ChangeStringText(m_pResolution, globalGraphicsPointer->GetResolutionState(m_nResolutionIndex).m_displayString);
}

void GraphicsOptionsState::SetWindowMode(int index)
{
	if (index < 0)
	{
		m_nWindowModeIndex = 2;
	}
	else if (index > 2)
	{
		m_nWindowModeIndex = 0;
	}
	else
	{
		m_nWindowModeIndex = index;
	}


	ChangeStringText(m_pWindowMode, globalGraphicsPointer->GetScreenState(m_nWindowModeIndex).m_displayString);
}

void GraphicsOptionsState::SetMSAALevel(int index)
{
	if (index < 0)
	{
		m_nMSAALevelIndex = 3;
	}
	else if (index > 3)
	{
		m_nMSAALevelIndex = 0;
	}
	else
	{
		m_nMSAALevelIndex = index;
	}

	ChangeStringText(m_pMSAALevel, globalGraphicsPointer->GetMSAAState(m_nMSAALevelIndex).m_displayString);
}

void GraphicsOptionsState::SetVSync(int index)
{
	if (index < 0)
	{
		m_nVSyncIndex = 1;
	}
	else if (index > 1)
	{
		m_nVSyncIndex = 0;
	}
	else
	{
		m_nVSyncIndex = index;
	}

	ChangeStringText(m_pVSync, globalGraphicsPointer->GetVsyncState(m_nVSyncIndex).m_displayString);
}

void GraphicsOptionsState::SetKey(int index)
{
	if (index < 0)
	{
		m_nKeyIndex = 1;
	}
	else if (index > 1)
	{
		m_nKeyIndex = 0;
	}
	else
	{
		m_nKeyIndex = index;
	}
}

void GraphicsOptionsState::ChangeStringText(GameObject* obj, string newString)
{
	RenderComp* messageComp = (RenderComp*)obj->GetComponent("RenderComp");
	if (messageComp)
	{
		messageComp->SetID(newString);
		messageComp->m_renderInfo = nullptr;
	}
}

void GraphicsOptionsState::SaveGraphicsSettings()
{
	TiXmlDocument doc;

	TiXmlDeclaration* pDel = new TiXmlDeclaration{ "1.0", "utf-8", "" };

	doc.LinkEndChild(pDel);

	TiXmlElement* pRoot = new TiXmlElement{ "SoundSettings" };

	doc.LinkEndChild(pRoot);

	TiXmlElement* pGraphics = new TiXmlElement{ "Volumes" };

	pRoot->LinkEndChild(pGraphics);

	pGraphics->SetAttribute("ResolutionIndex", m_nResolutionIndex);
	pGraphics->SetAttribute("WindowModeIndex", m_nWindowModeIndex);
	pGraphics->SetAttribute("MSAALevelIndex", m_nMSAALevelIndex);
	pGraphics->SetAttribute("VSyncIndex", m_nVSyncIndex);
	pGraphics->SetAttribute("GammaLevel", (int)(m_fGamma * 100));

	doc.SaveFile("Assets/XML/GraphicsSettings.xml");
}

bool GraphicsOptionsState::LoadGraphicsSettings()
{
	string filePath = "Assets/XML/GraphicsSettings.xml";

	assert(filePath.c_str() != nullptr);

	TiXmlDocument doc;

	if (doc.LoadFile(filePath.c_str()) == false)
	{
		return false;
	}

	TiXmlElement* pGraphics = doc.RootElement();

	if (pGraphics == nullptr)
	{
		return false;
	}

	TiXmlElement* pGraphicSettings = pGraphics->FirstChildElement();

	while (pGraphicSettings != nullptr)
	{
		int ResolutionIndex;
		pGraphicSettings->Attribute("ResolutionIndex", &ResolutionIndex);
		m_nResolutionIndex = ResolutionIndex;

		int WindowModeIndex;
		pGraphicSettings->Attribute("WindowModeIndex", &WindowModeIndex);
		m_nWindowModeIndex = WindowModeIndex;

		int MSAALevelIndex;
		pGraphicSettings->Attribute("MSAALevelIndex", &MSAALevelIndex);
		m_nMSAALevelIndex = MSAALevelIndex;

		int VSyncIndex;
		pGraphicSettings->Attribute("VSyncIndex", &VSyncIndex);
		m_nVSyncIndex = VSyncIndex;

		int GammaLevel;
		pGraphicSettings->Attribute("GammaLevel", &GammaLevel);
		m_fGamma = ((float)GammaLevel) * 0.01f;

		pGraphicSettings = pGraphicSettings->NextSiblingElement();
	}

	globalGraphicsPointer->ApplyGraphicsSettings(m_nMSAALevelIndex, m_nWindowModeIndex, m_nResolutionIndex, m_nVSyncIndex, m_fGamma);

	return true;
}

void GraphicsOptionsState::TurnGammaUp(void)
{
	static float nextPress = 0;

	nextPress += m_Xtime->Delta();

	if (nextPress < 0.03f)
	{
		return;
	}
	else
	{
		nextPress = 0;
	}

	if (currentMode == GRAPHIC_MODE::GAMMA)
	{
		m_nGamma = CapGamma(m_nGamma + 1);
		m_fGamma = m_nGamma / 100.0f;
	}
}

void GraphicsOptionsState::TurnGammaDown(void)
{

	static float nextPress = 0;

	nextPress += m_Xtime->Delta();

	if (nextPress < 0.03f)
	{
		return;
	}
	else
	{
		nextPress = 0;
	}

	if (currentMode == GRAPHIC_MODE::GAMMA)
	{
		m_nGamma = CapGamma(m_nGamma - 1);
		m_fGamma = m_nGamma / 100.0f;
	}
}

int GraphicsOptionsState::CapGamma(int gamma)
{
	if (gamma < 0)
	{
		gamma = 0;
	}

	if (gamma > 100)
	{
		gamma = 100;
	}

	return gamma;
}

void GraphicsOptionsState::UpdateSliderBar(void)
{
	RenderComp* sliderIndicator = (RenderComp*)m_pGammaSliderBarIndicator->GetComponent("RenderComp");
	RenderComp* sliderBar = (RenderComp*)m_pGammaSliderBar->GetComponent("RenderComp");
	if (sliderIndicator != nullptr && sliderBar != nullptr)
	{
		sliderIndicator->m_posX = (sliderBar->m_posX + sliderIndicator->m_width) + ((m_nGamma / 100.0f)* (sliderBar->m_width - (sliderIndicator->m_width* 3.0f)));
	}
}