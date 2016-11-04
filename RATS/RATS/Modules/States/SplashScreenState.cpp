#include "stdafx.h"
#include "SplashScreenState.h"
#include "../Input/InputController.h"
#include "../Renderer/D3DGraphics.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../../XTime.h"
#include "../TinyXml/tinystr.h"
#include "../TinyXml/tinyxml.h"
#include <cassert>
#include <fstream>

extern InputController* m_controller;
extern D3DGraphics* globalGraphicsPointer;
extern	FFmod*		g_AudioSystem;
extern ThreeSixty* gamePad;
extern XTime *m_Xtime;

SplashScreenState::SplashScreenState()
{
}


SplashScreenState::~SplashScreenState()
{
}

void SplashScreenState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_NextState = STATE_TYPE::SPLASH_SCREEN_STATE;

	//Setup Input
	if (m_controller)
		m_controller->SetSplashScreenKeys();
	using namespace Events;

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &SplashScreenState::OnEnter);

	MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", fMousePrevX, fMousePrevY);

	dwPacketNo = gamePad->GetState().state.dwPacketNumber;
	m_Xtime->Restart();
}

void SplashScreenState::Exit()
{
	using namespace Events;

	EventManager()->UnregisterClient("Confirm", this, &SplashScreenState::OnEnter);

	if (m_controller)
		m_controller->ClearSplashScreenKeys();

	m_bFirstTimePlayed = true;
	SaveSplashScreenSettings();
}

void SplashScreenState::Initialize(Renderer* renderer)
{
	m_Renderer = renderer;
	m_ObjectManager.Initialize();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory(&m_ObjectFactory);

	CreateHud();
	LoadSplashScreenSettings();
}

void SplashScreenState::Update(float dt)
{
	if (timer < 10)
	{
		timer += dt;

	}

	if (timer < 3.0f)
	{
		gpComp->color = XMFLOAT4(1, 1, 1, 1);
		hfgComp->color = XMFLOAT4(1, 1, 1, 0);
		ratsComp->color = XMFLOAT4(1, 1, 1, 0);
	}
	else if (timer >= 3.0f && timer < 6.0f)
	{

		if (firstFade == false)
		{
			firstFade = true;
			globalGraphicsPointer->FadeOut();
		}

		gpComp->color = XMFLOAT4(1, 1, 1, 0);
		hfgComp->color = XMFLOAT4(1, 1, 1, 1);
		ratsComp->color = XMFLOAT4(1, 1, 1, 0);
	}
	else if (timer >= 6.0f && timer < 9.0f)
	{
		if (secondFade == false)
		{
			secondFade = true;
			globalGraphicsPointer->FadeOut();
		}

		gpComp->color = XMFLOAT4(1, 1, 1, 0);
		hfgComp->color = XMFLOAT4(1, 1, 1, 0);
		ratsComp->color = XMFLOAT4(1, 1, 1, 1);
	}
	else
	{
		m_NextState = STATE_TYPE::MAIN_MENU_STATE;
	}
	
	Events::CGeneralEventArgs2<unsigned char, float> tmpArgs(0, -1.0f);

	if (gamePad->GetState().buttons[buttonList::A] == buttonStatus::bPress ||
		gamePad->GetState().buttons[buttonList::START] == buttonStatus::bPress)
	{
		OnEnter(tmpArgs);
	}

	//if (!m_bLogo1Played)
	//{
	//	m_fLogo1Timer += dt;

	//	if (m_fLogo1Timer > 3.0f)
	//	{
	//		m_bLogo1Played = true;
	//	}
	//}
	//else if (!m_bFade1Played)
	//{
	//	RenderComp* hfgLogo = (RenderComp*)m_pRatsLogo->GetComponent("RenderComp");
	//	if (hfgLogo != nullptr)
	//	{
	//		hfgLogo->color.x -= dt;
	//		hfgLogo->color.y -= dt;
	//		hfgLogo->color.z -= dt;
	//		hfgLogo->color.w -= dt;

	//		if (hfgLogo->color.x <= 0.0f)
	//		{
	//			hfgLogo->color.x = 0.0f;
	//			hfgLogo->color.y = 0.0f;
	//			hfgLogo->color.z = 0.0f;
	//			hfgLogo->color.w = 0.0f;
	//			m_bFade1Played = true;
	//		}

	//	}
	//}
	//else if (!m_bFade2Played)
	//{
	//	RenderComp* ratsLogo = (RenderComp*)m_pRatsLogo->GetComponent("RenderComp");
	//	if (ratsLogo != nullptr)
	//	{
	//		ratsLogo->color.x += dt;
	//		ratsLogo->color.y += dt;
	//		ratsLogo->color.z += dt;
	//		ratsLogo->color.w += dt;

	//		if (ratsLogo->color.x >= 1.0f)
	//		{
	//			ratsLogo->color.x = 1.0f;
	//			ratsLogo->color.y = 1.0f;
	//			ratsLogo->color.z = 1.0f;
	//			ratsLogo->color.w = 1.0f;
	//			m_bFade2Played = true;
	//		}
	//		
	//	}
	//}
	//else
	//{
	//	m_fLogo2Timer += dt;

	//	if (m_fLogo2Timer > 3.0f)
	//	{
	//		m_NextState = STATE_TYPE::MAIN_MENU_STATE;
	//	}
	//}
	
}

void SplashScreenState::Terminate()
{

}

void SplashScreenState::CreateHud(void)
{
	//Create the Rats Logo
	m_pHFGLogo = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "HFGLogo", eRENDERCOMP_TYPE::Render2D_Back);
	hfgComp = (RenderComp*)m_pHFGLogo->GetComponent("RenderComp");
	if (hfgComp != nullptr)
	{
		hfgComp->m_posX = 0.0f;
		hfgComp->m_posY = 0.0f;
		hfgComp->m_height = 1.0f;
		hfgComp->m_width = 1.0f;
	}

	//Create the Rats Logo
	m_pGPGamesLogo = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "GPGamesLogo", eRENDERCOMP_TYPE::Render2D_Back);
	gpComp = (RenderComp*)m_pGPGamesLogo->GetComponent("RenderComp");
	if (gpComp != nullptr)
	{
		gpComp->m_posX = 0.0f;
		gpComp->m_posY = 0.0f;
		gpComp->m_height = 1.0f;
		gpComp->m_width = 1.0f;
	}

	//Create the Rats Logo
	m_pRatsLogo = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "RatsLogo", eRENDERCOMP_TYPE::Render2D_Back);
	ratsComp = (RenderComp*)m_pRatsLogo->GetComponent("RenderComp");
	if (ratsComp != nullptr)
	{
		ratsComp->m_posX = 0.0f;
		ratsComp->m_posY = 0.0f;
		ratsComp->m_height = 1.0f;
		ratsComp->m_width = 1.0f;
		ratsComp->color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

void SplashScreenState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (m_bFirstTimePlayed)
	{
		if (timer < 3.0f)
		{
			timer = 3.0f;
		}
		else if (timer >= 3.0f && timer < 6.0f)
		{
			timer = 6.0f;
		}
		else
		{
			timer = 9.0f;
			//m_NextState = STATE_TYPE::MAIN_MENU_STATE;
		}
	}
}

void SplashScreenState::SaveSplashScreenSettings()
{
	TiXmlDocument doc;

	TiXmlDeclaration* pDel = new TiXmlDeclaration{ "1.0", "utf-8", "" };

	doc.LinkEndChild(pDel);

	TiXmlElement* pRoot = new TiXmlElement{ "SoundSettings" };

	doc.LinkEndChild(pRoot);

	TiXmlElement* pPlayed = new TiXmlElement{ "Played" };

	pRoot->LinkEndChild(pPlayed);

	pPlayed->SetAttribute("Played", m_bFirstTimePlayed);
	

	doc.SaveFile("Assets/XML/SplashScreenSettings.xml");
}

void SplashScreenState::LoadSplashScreenSettings()
{
	string filePath = "Assets/XML/SplashScreenSettings.xml";

	assert(filePath.c_str() != nullptr);

	TiXmlDocument doc;

	if (doc.LoadFile(filePath.c_str()) == false)
	{
		return;
	}

	TiXmlElement* pSettings = doc.RootElement();

	if (pSettings == nullptr)
	{
		return;
	}

	TiXmlElement* pPlayed = pSettings->FirstChildElement();

	while (pPlayed != nullptr)
	{
		int played;
		pPlayed->Attribute("Played", &played);
		m_bFirstTimePlayed = (bool)played;
		

		pPlayed = pPlayed->NextSiblingElement();
	}
}