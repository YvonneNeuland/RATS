#include "stdafx.h"
#include "SoundOptionState.h"
#include "../Input/InputController.h"
#include "../Renderer/D3DGraphics.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../TinyXml/tinystr.h"
#include "../TinyXml/tinyxml.h"
#include "../Upgrade System/GameData.h"
#include <cassert>
#include <fstream>

#include "../../XTime.h"


extern InputController* m_controller;
extern D3DGraphics* globalGraphicsPointer;
extern	FFmod*		g_AudioSystem;
extern ThreeSixty* gamePad;
extern GameData* gameData;
extern XTime* m_Xtime;


SoundOptionState::SoundOptionState()
{
	dwPacketNo = 0;
}

SoundOptionState::~SoundOptionState()
{
}

void SoundOptionState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_NextState = STATE_TYPE::SOUND_MENU_STATE;

	m_unCurrentButton = 0;

	if (m_controller)
	{
		m_controller->SetSoundKeys();
	}
	using namespace Events;
	//Setup Input
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &SoundOptionState::OnEnter);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &SoundOptionState::OnEscape);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Up"), this, &SoundOptionState::OnArrowUp);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Down"), this, &SoundOptionState::OnArrowDown);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyDown", "Left"), this, &SoundOptionState::OnArrowLeft);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyDown", "Right"), this, &SoundOptionState::OnArrowRight);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "UpArr"), this, &SoundOptionState::OnArrowUp);
	
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownArr"), this, &SoundOptionState::OnArrowDown);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyDown", "LeftArr"), this, &SoundOptionState::OnArrowLeft);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyDown", "RightArr"), this, &SoundOptionState::OnArrowRight);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &SoundOptionState::OnMouseClick);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyDown", "MouseLDown"), this, &SoundOptionState::OnMouseDown);

	/*if (resetSound)
	{
		g_AudioSystem->PostEvent(AK::EVENTS::STOP_ALL);
		g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MX_MUSICLOOP_02);
	}*/

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMousePrevX, fMousePrevY);

	dwPacketNo = gamePad->GetState().state.dwPacketNumber;
}

void SoundOptionState::Exit()
{
	using namespace Events;

	EventManager()->UnregisterClient("Confirm", this, &SoundOptionState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &SoundOptionState::OnEscape);
	EventManager()->UnregisterClient("Up", this, &SoundOptionState::OnArrowUp);
	EventManager()->UnregisterClient("Down", this, &SoundOptionState::OnArrowDown);
	EventManager()->UnregisterClient("UpArr", this, &SoundOptionState::OnArrowUp);
	EventManager()->UnregisterClient("DownArr", this, &SoundOptionState::OnArrowDown);
	EventManager()->UnregisterClient("LeftArr", this, &SoundOptionState::OnArrowLeft);
	EventManager()->UnregisterClient("RightArr", this, &SoundOptionState::OnArrowRight);
	EventManager()->UnregisterClient("MouseLClick", this, &SoundOptionState::OnMouseClick);
	EventManager()->UnregisterClient("MouseLDown", this, &SoundOptionState::OnMouseDown);

	if (m_controller)
	{
		m_controller->ClearSoundKeys();
	}

	SaveSoundSettings();
}

void SoundOptionState::Initialize(Renderer* renderer)
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

	LoadSoundSettings();

}

void SoundOptionState::Update(float dt)
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

	SetSelectButton();
	UpdateSliderBars();

	
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

void SoundOptionState::Terminate()
{
	m_vButtons.clear();
}

//Input Functions
void SoundOptionState::OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton - 1);
	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_HOVER);
	g_AudioSystem->PlaySound("MENU_Hover");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void SoundOptionState::OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	SetCurrentButton(m_unCurrentButton + 1);
	g_AudioSystem->PlaySound("MENU_Hover");
	globalGraphicsPointer->SetCrosshairShowing(true);
}


void SoundOptionState::OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	TurnVolumeDown();
}

void SoundOptionState::OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	TurnVolumeUp();
}

void SoundOptionState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	if (m_NextState != m_SelectedState)
	{
		m_NextState = m_SelectedState;
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_BACK);
		g_AudioSystem->PlaySound("MENU_Back");

	}
	else
	{
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
		g_AudioSystem->PlaySound("MENU_Accept");

	}
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void SoundOptionState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	m_NextState = STATE_TYPE::OPTIONS_MENU_STATE;
	//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_BACK);
	g_AudioSystem->PlaySound("MENU_Back");
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void SoundOptionState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;
	
	if (CheckMouseMove() || globalGraphicsPointer->GetCrosshairShowing())
	{
		SetSelectButton();
		m_NextState = m_SelectedState;

		if (m_NextState == STATE_TYPE::OPTIONS_MENU_STATE)
		{
			//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_BACK);
			g_AudioSystem->PlaySound("MENU_Back");

		}		
	}
	globalGraphicsPointer->SetCrosshairShowing(true);
}

void SoundOptionState::OnMouseDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
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
		RenderComp* pButton = (RenderComp*)m_pBGMLeftButton->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (!globalGraphicsPointer)
			return;

		GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

		fMouseX = fMouseX / (clientRect.right - clientRect.left);
		fMouseY = fMouseY / (clientRect.bottom - clientRect.top);

		//Check Buttons - BGM LEFT ARR
		pButton = (RenderComp*)m_pBGMLeftButton->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			TurnVolumeDown();
			return;
		}

		//Check Buttons - BGM RIGHT ARR
		pButton = (RenderComp*)m_pBGMRightButton->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			TurnVolumeUp();
			return;
		}

		//Check Buttons - BGM SLIDER INDICATOR
		pButton = (RenderComp*)m_pBGMSliderBar->GetComponent("RenderComp");
		if (!pButton)
		{
			return;
		}

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{

			float xDiff = globalGraphicsPointer->MOUSE_X - pButton->m_posX;
			float percent = (xDiff / pButton->m_width) * 100;

			bool playSound = false;
			bool played = false;

			if (m_SelectedState == STATE_TYPE::SOUND_MENU_STATE)
			{

				switch (m_SelectSoundType)
				{
				case BGM:
				{
					m_nBGMVolume = CapVolume(percent);
					g_AudioSystem->SetChannelVolume(2, m_nBGMVolume);
				}
				break;
				case SFX:
				{
					m_SFXVolume = CapVolume(percent);
					g_AudioSystem->SetChannelVolume(1, m_SFXVolume);
					playSound = true;
				}
				break;
				default:
					break;
				}
			}

			if (playSound && !played)
			{
				//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
				g_AudioSystem->PlaySound("MENU_Hover");

				played = true;
			}

			//if (m_fOldMouseXBGM == 0.0f)
			//{
			//	m_fOldMouseXBGM = fMouseX;
			//}
			//else
			//{
			//	if (fMouseX > m_fOldMouseXBGM)
			//	{
			//		TurnVolumeUp();
			//	}
			//	else if (fMouseX < m_fOldMouseXBGM)
			//	{
			//		TurnVolumeDown();
			//	}

			//	//pButton->color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			//	m_fOldMouseXBGM = fMouseX;
			//}

		}
		else
		{
			//pButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			m_fOldMouseXBGM = 0.0f;
		}

		//Check Buttons - SFX LEFT ARR
		pButton = (RenderComp*)m_pSFXLeftButton->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			TurnVolumeDown();
			return;
		}

		//Check Buttons - SFX RIGHT ARR
		pButton = (RenderComp*)m_pSFXRightButton->GetComponent("RenderComp");
		if (!pButton)
			return;

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{
			TurnVolumeUp();
			return;
		}

		//Check Buttons - SFX SLIDER INDICATOR
		pButton = (RenderComp*)m_pSFXSliderBar->GetComponent("RenderComp");
		if (!pButton)
		{
			return;
		}

		if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
			fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
		{

			float xDiff = globalGraphicsPointer->MOUSE_X - pButton->m_posX;
			float percent = (xDiff / pButton->m_width) * 100;

			bool playSound = false;
			bool played = false;

			if (m_SelectedState == STATE_TYPE::SOUND_MENU_STATE)
			{

				switch (m_SelectSoundType)
				{
				case BGM:
				{
					m_nBGMVolume = CapVolume(percent);
					g_AudioSystem->SetChannelVolume(2, m_nBGMVolume);
				}
				break;
				case SFX:
				{
					m_SFXVolume = CapVolume(percent);
					g_AudioSystem->SetChannelVolume(1, m_SFXVolume);
					playSound = true;
				}
				break;
				default:
					break;
				}
			}

			if (playSound && !played)
			{

				static float nextPress = 0;

				nextPress += m_Xtime->Delta();

				if (nextPress < 0.03f)
				{
					//nothing
				}
				else
				{
					nextPress = 0;

					g_AudioSystem->PlaySound("MENU_Hover");

					played = true;
				}



			}
			//if (m_fOldMouseXSFX == 0.0f)
			//{
			//	m_fOldMouseXSFX = fMouseX;
			//}
			//else
			//{
			//	if (fMouseX > m_fOldMouseXSFX)
			//	{
			//		TurnVolumeUp();
			//	}
			//	else if (fMouseX < m_fOldMouseXSFX)
			//	{
			//		TurnVolumeDown();
			//	}

			//	//pButton->color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			//	m_fOldMouseXSFX = fMouseX;
			//}

		}
		else
		{
			//pButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			m_fOldMouseXSFX = 0.0f;
		}
	}
}

//Private Functions
void SoundOptionState::CreateHud(void)
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

	/////////////
	//BGM Volume
	////////////

	//Create the BGM Volume Button
	m_pBGMButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "bgmVolumeButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* bgmVolumeButton = (RenderComp*)m_pBGMButton->GetComponent("RenderComp");
	if (bgmVolumeButton != nullptr)
	{
		bgmVolumeButton->m_posX = 0.15f;
		bgmVolumeButton->m_posY = 0.17f;
		bgmVolumeButton->m_height = 0.175f;
		bgmVolumeButton->m_width = 0.3f;
	}

	m_pBGMButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "bgmVolumeButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	bgmVolumeButton = (RenderComp*)m_pBGMButtonSelected->GetComponent("RenderComp");
	if (bgmVolumeButton != nullptr)
	{
		bgmVolumeButton->m_posX = 0.15f;
		bgmVolumeButton->m_posY = 0.17f;
		bgmVolumeButton->m_height = 0.0f;
		bgmVolumeButton->m_width = 0.0f;
	}

	//Create the BGM Slider Indicator
	m_pBGMSliderIndicator = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sliderIndicator", Render2D_Front);
	RenderComp* bgmSliderIndicator = (RenderComp*)m_pBGMSliderIndicator->GetComponent("RenderComp");
	if (bgmSliderIndicator != nullptr)
	{
		bgmSliderIndicator->m_posX = 0.6f;
		bgmSliderIndicator->m_posY = 0.225f;
		bgmSliderIndicator->m_height = 0.0875f;
		bgmSliderIndicator->m_width = 0.02f;
	}

	//Create the BGM Slider Bar
	m_pBGMSliderBar = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sliderBar", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* bgmSliderBar = (RenderComp*)m_pBGMSliderBar->GetComponent("RenderComp");
	if (bgmSliderBar != nullptr)
	{
		bgmSliderBar->m_posX = 0.507f;
		bgmSliderBar->m_posY = 0.225f;
		bgmSliderBar->m_height = 0.0875f;
		bgmSliderBar->m_width = 0.43f;
	}

	//Create the BGM Left Arrow
	m_pBGMLeftButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* bgmLeftArrow = (RenderComp*)m_pBGMLeftButton->GetComponent("RenderComp");
	if (bgmLeftArrow != nullptr)
	{
		bgmLeftArrow->m_posX = 0.46f;
		bgmLeftArrow->m_posY = 0.24f;
		bgmLeftArrow->m_height = 0.05f;
		bgmLeftArrow->m_width = 0.05f;
	}

	//Create the BGM Right Arrow
	m_pBGMRightButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* bgmRightArrow = (RenderComp*)m_pBGMRightButton->GetComponent("RenderComp");
	if (bgmRightArrow != nullptr)
	{
		bgmRightArrow->m_posX = 0.93f;
		bgmRightArrow->m_posY = 0.24f;
		bgmRightArrow->m_height = 0.05f;
		bgmRightArrow->m_width = 0.05f;
	}

	/////////////
	//SFX Volume
	////////////

	//Create the SFX Volume Button
	m_pSFXButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sfxVolumeButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* sfxVolumeButton = (RenderComp*)m_pSFXButton->GetComponent("RenderComp");
	if (sfxVolumeButton != nullptr)
	{
		sfxVolumeButton->m_posX = 0.15f;
		sfxVolumeButton->m_posY = 0.375f;
		sfxVolumeButton->m_height	= 0.175f;
		sfxVolumeButton->m_width	= 0.3f;
	}

	m_pSFXButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sfxVolumeButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	sfxVolumeButton = (RenderComp*)m_pSFXButtonSelected->GetComponent("RenderComp");
	if (sfxVolumeButton != nullptr)
	{
		sfxVolumeButton->m_posX = 0.15f;
		sfxVolumeButton->m_posY = 0.375f;
		sfxVolumeButton->m_height = 0.0f;
		sfxVolumeButton->m_width = 0.0f;
	}

	//Create the SFX Slider Indicator
	m_pSFXSliderIndicator = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sliderIndicator", Render2D_Front);
	RenderComp* sfxSliderIndicator = (RenderComp*)m_pSFXSliderIndicator->GetComponent("RenderComp");
	if (sfxSliderIndicator != nullptr)
	{
		sfxSliderIndicator->m_posX = 0.6f;
		sfxSliderIndicator->m_posY = 0.425f;
		sfxSliderIndicator->m_height = 0.0875f;
		sfxSliderIndicator->m_width = 0.02f;
	}

	//Create the SFX Slider Bar
	m_pSFXSliderBar = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "sliderBar", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* sfxSliderBar = (RenderComp*)m_pSFXSliderBar->GetComponent("RenderComp");
	if (sfxSliderBar != nullptr)
	{
		sfxSliderBar->m_posX = 0.507f;
		sfxSliderBar->m_posY = 0.425f;
		sfxSliderBar->m_height = 0.0875f;
		sfxSliderBar->m_width = 0.43f;
	}

	//Create the SFX Left Arrow
	m_pSFXLeftButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "leftArrow", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* sfxLeftArrow = (RenderComp*)m_pSFXLeftButton->GetComponent("RenderComp");
	if (sfxLeftArrow != nullptr)
	{
		sfxLeftArrow->m_posX = 0.46f;
		sfxLeftArrow->m_posY = 0.44f;
		sfxLeftArrow->m_height = 0.05f;
		sfxLeftArrow->m_width = 0.05f;
	}

	//Create the SFX Right Arrow
	m_pSFXRightButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "rightArrow", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* sfxRightArrow = (RenderComp*)m_pSFXRightButton->GetComponent("RenderComp");
	if (sfxRightArrow != nullptr)
	{
		sfxRightArrow->m_posX = 0.93f;
		sfxRightArrow->m_posY = 0.44f;
		sfxRightArrow->m_height = 0.05f;
		sfxRightArrow->m_width = 0.05f;
	}
	
	//Create the Back Button
	m_pBackButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* backButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
	if (backButton != nullptr)
	{
		backButton->m_posX = 0.3f;
		backButton->m_posY = 0.785f;
		backButton->m_height	= 0.125f;
		backButton->m_width		= 0.3f;
	}

	m_pBackButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButtonSelected", eRENDERCOMP_TYPE::Render2D_Front);
	backButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
	if (backButton != nullptr)
	{
		backButton->m_posX = 0.3f;
		backButton->m_posY = 0.785f;
		backButton->m_height = 0.0f;
		backButton->m_width = 0.0f;
	}

	//Create the Select Button
	m_pSelectButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "selectButton", Render2D_Front);
	RenderComp* selectButton = (RenderComp*)m_pSelectButton->GetComponent("RenderComp");

	if (selectButton != nullptr)
	{
		selectButton->m_posX = 0.025f;
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

//Input Helper Functions
void SoundOptionState::SetSelectButton(void)
{
	ShrinkAllButtons();
	//ResetButtonColors();

	RenderComp* selectButton = (RenderComp*)m_pSelectButton->GetComponent("RenderComp");

	switch (m_unCurrentButton)
	{
		case 0:
		{
			if (selectButton != nullptr)
			{
				selectButton->m_posX = 0.025f;
				selectButton->m_posY = 0.095f;
			}	

			GrowSelectedButton(m_pBGMButtonSelected);

			m_SelectSoundType = SOUND_TYPE::BGM;
			m_SelectedState = STATE_TYPE::SOUND_MENU_STATE;

		}
			break;
		case 1:
		{
			if (selectButton != nullptr)
			{
				selectButton->m_posX = 0.025f;
				selectButton->m_posY = 0.3f;
			}

			GrowSelectedButton(m_pSFXButtonSelected);

			m_SelectSoundType = SOUND_TYPE::SFX;
			m_SelectedState = STATE_TYPE::SOUND_MENU_STATE;

		}
			break;		
		case 2:
		{
			if (selectButton != nullptr)
			{
				selectButton->m_posX = 0.225f;
				selectButton->m_posY = 0.8f;
			}

			RenderComp* pButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
			if (pButton)
			{
				pButton->m_height = 0.125f;
				pButton->m_width = 0.3f;
			}

			m_SelectedState = STATE_TYPE::OPTIONS_MENU_STATE;
		}
			break;	
		default:
			break;
	}
}

bool SoundOptionState::CheckMouseMove()
{
	using namespace Events;

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMouseX, fMouseY);

	if (fMouseX == fMousePrevX && fMouseY == fMousePrevY)
		return false;

	RECT clientRect;
	RenderComp* pButton = (RenderComp*)m_pBGMButton->GetComponent("RenderComp");
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

	////////
	//BGM
	///////

	// Check Buttons - BGM Volume Button


	if (fMouseX > pButton->m_posX && fMouseX <= pButton->m_posX + pButton->m_width &&
		fMouseY > pButton->m_posY && fMouseY <= pButton->m_posY + pButton->m_height)
	{
		if (m_unCurrentButton != 0)
			g_AudioSystem->PlaySound("MENU_Hover");

		m_unCurrentButton = 0;
		return true;
	}

	//Check Buttons - BGM LEFT ARR
	pButton = (RenderComp*)m_pBGMLeftButton->GetComponent("RenderComp");
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

	//Check Buttons - BGM RIGHT ARR
	pButton = (RenderComp*)m_pBGMRightButton->GetComponent("RenderComp");
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

	//Check Buttons - BGM SLIDER INDICATOR
	pButton = (RenderComp*)m_pBGMSliderIndicator->GetComponent("RenderComp");
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

	//Check Buttons - BGM SLIDER BAR
	pButton = (RenderComp*)m_pBGMSliderBar->GetComponent("RenderComp");
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

	////////
	//SFX
	///////

	//Check Buttons - SFX Volume Button
	pButton = (RenderComp*)m_pSFXButton->GetComponent("RenderComp");
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

	//Check Buttons - SFX LEFT ARR
	pButton = (RenderComp*)m_pSFXLeftButton->GetComponent("RenderComp");
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

	//Check Buttons - SFX RIGHT ARR
	pButton = (RenderComp*)m_pSFXRightButton->GetComponent("RenderComp");
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

	//Check Buttons - SFX SLIDER INDICATOR
	pButton = (RenderComp*)m_pSFXSliderIndicator->GetComponent("RenderComp");
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

	//Check Buttons - SFX SLIDER BAR
	pButton = (RenderComp*)m_pSFXSliderBar->GetComponent("RenderComp");
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

	///////////////////////////////////////
	//Check Buttons - Back Button
	//////////////////////////////////////
	pButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
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

bool SoundOptionState::Handle360Input()
{
	GamePad tmpPad = gamePad->GetState();
	bool detectedInput = false;

	if (tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton - 1);
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_HOVER);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bHeld)
	{
		SetCurrentButton(m_unCurrentButton + 1);
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_HOVER);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	//
	if (tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bHeld)
	{
		TurnVolumeUp();
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bHeld)
	{
		TurnVolumeDown();
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{
		m_NextState = m_SelectedState;
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_HOVER);
		g_AudioSystem->PlaySound("MENU_Hover");
		detectedInput = true;
	}

	return detectedInput;
}

void SoundOptionState::SetCurrentButton(int numButton)
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

int SoundOptionState::CapVolume(int volume)
{
	if (volume < 0)
	{
		volume = 0;
	}

	if (volume > 100)
	{
		volume = 100;
	}

	return volume;
}

void SoundOptionState::UpdateSliderBars(void)
{
	RenderComp* bgmSliderIndicator = (RenderComp*)m_pBGMSliderIndicator->GetComponent("RenderComp");
	if (bgmSliderIndicator != nullptr)
	{
		bgmSliderIndicator->m_posX = 0.53f + ((m_nBGMVolume / 100.0f)* 0.37f);
	}

	RenderComp* sfxSliderIndicator = (RenderComp*)m_pSFXSliderIndicator->GetComponent("RenderComp");
	if (sfxSliderIndicator != nullptr)
	{
		sfxSliderIndicator->m_posX = 0.53f + ((m_SFXVolume / 100.0f)* 0.37f);
	}	
}

void SoundOptionState::TurnVolumeDown()
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

	bool playSound = false;

	if (m_SelectedState == STATE_TYPE::SOUND_MENU_STATE)
	{

		switch (m_SelectSoundType)
		{
		case BGM:
		{
			m_nBGMVolume = CapVolume(m_nBGMVolume - 1);
			//g_AudioSystem->SetRTCPValue(AK::GAME_PARAMETERS::MX_VOLUME, (AkRtpcValue)m_nBGMVolume);
			g_AudioSystem->SetChannelVolume(2, m_nBGMVolume);
		}
			break;
		case SFX:
		{
			m_SFXVolume = CapVolume(m_SFXVolume - 1);
			/*g_AudioSystem->SetRTCPValue(AK::GAME_PARAMETERS::FX_VOLUME, (AkRtpcValue)m_SFXVolume);*/
			g_AudioSystem->SetChannelVolume(1, m_SFXVolume);
			playSound = true;
		}
			break;
		
		default:
			break;
		}
	}

	if (playSound)
	{
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
		g_AudioSystem->PlaySound("MENU_Hover");

	}
}

void SoundOptionState::TurnVolumeUp()
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

	bool playSound = false;
	bool played = false;

	if (m_SelectedState == STATE_TYPE::SOUND_MENU_STATE)
	{

		switch (m_SelectSoundType)
		{
		case BGM:
		{
			m_nBGMVolume = CapVolume(m_nBGMVolume + 1);
			g_AudioSystem->SetChannelVolume(2, m_nBGMVolume);
		}
			break;
		case SFX:
		{
			m_SFXVolume = CapVolume(m_SFXVolume + 1);
			g_AudioSystem->SetChannelVolume(1, m_SFXVolume);
			playSound = true;
		}
			break;		
		default:
			break;
		}
	}

	if (playSound && !played)
	{
		//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_MENU_ACCEPT);
		g_AudioSystem->PlaySound("MENU_Hover");

		played = true;
	}
}

void SoundOptionState::SaveSoundSettings()
{
	TiXmlDocument doc;

	TiXmlDeclaration* pDel = new TiXmlDeclaration{ "1.0", "utf-8", "" };

	doc.LinkEndChild(pDel);

	TiXmlElement* pRoot = new TiXmlElement{ "SoundSettings" };

	doc.LinkEndChild(pRoot);

	TiXmlElement* pVolumes = new TiXmlElement{ "Volumes" };

	pRoot->LinkEndChild(pVolumes);

	pVolumes->SetAttribute("BGM", m_nBGMVolume);
	pVolumes->SetAttribute("SFX", m_SFXVolume);

	doc.SaveFile("Assets/XML/SoundSettings.xml");
}

void SoundOptionState::LoadSoundSettings()
{
	string filePath = "Assets/XML/SoundSettings.xml";

	assert(filePath.c_str() != nullptr);

	TiXmlDocument doc;

	if (doc.LoadFile(filePath.c_str()) == false)
	{
		return;
	}

	TiXmlElement* pSoundSettings = doc.RootElement();

	if (pSoundSettings == nullptr)
	{
		return;
	}

	TiXmlElement* pVolumes = pSoundSettings->FirstChildElement();

	while (pVolumes != nullptr)
	{
		int bgm;
		pVolumes->Attribute("BGM", &bgm);
		m_nBGMVolume = bgm;

		int sfx;
		pVolumes->Attribute("SFX", &sfx);
		m_SFXVolume = sfx;
		
		pVolumes = pVolumes->NextSiblingElement();
	}

	g_AudioSystem->SetChannelVolume(2, m_nBGMVolume);
	g_AudioSystem->SetChannelVolume(1, m_SFXVolume);
}

void SoundOptionState::ResetButtonColors()
{
	RenderComp* pButton = (RenderComp*)m_pBGMButton->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	pButton = (RenderComp*)m_pSFXButton->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	pButton = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void SoundOptionState::ShrinkAllButtons()
{
	RenderComp* pButton = (RenderComp*)m_pBGMButtonSelected->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	pButton = (RenderComp*)m_pSFXButtonSelected->GetComponent("RenderComp");
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
}

void SoundOptionState::GrowSelectedButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.175f;
		pButton->m_width = 0.3f; 
	}							  
}