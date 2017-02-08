#include "stdafx.h"
#include "LevelSelectState.h"

#include "../BitmapFont/BitmapFontManager.h"
#include "../Input/InputController.h"
#include "../Renderer/D3DGraphics.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../../Wwise files/EventManager.h"
#include "../Renderer/RenderComp.h"
#include "../Upgrade System/GameData.h"

extern InputController* m_controller;
extern D3DGraphics* globalGraphicsPointer;
extern	FFmod*		g_AudioSystem;
extern ThreeSixty* gamePad;
extern GameData *gameData;


LevelSelectState::LevelSelectState()
{
}

LevelSelectState::~LevelSelectState()
{
}

void LevelSelectState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_pBitmapFontManager = bitmapFontManager;
	m_ObjectManager.Update(0.5f);

	if (m_controller)
	{
		m_controller->SetMenuKeys();
		m_controller->SetDebugKeys();
	}
		

	m_NextState = STATE_TYPE::LEVEL_SELECT_STATE;

	// TODO: a lot of other stuff

	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	m_ulCurrPacketNo = gamePad->GetState().state.dwPacketNumber;

	UpdateButtons();
	SwitchButton(0, 0);



	using namespace Events;

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &LevelSelectState::OnEnter);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &LevelSelectState::OnEscape);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Up"), this, &LevelSelectState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Down"), this, &LevelSelectState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "UpArr"), this, &LevelSelectState::OnArrowUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownArr"), this, &LevelSelectState::OnArrowDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Left"), this, &LevelSelectState::OnArrowLeft);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "LeftArr"), this, &LevelSelectState::OnArrowLeft);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Right"), this, &LevelSelectState::OnArrowRight);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "RightArr"), this, &LevelSelectState::OnArrowRight);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &LevelSelectState::OnMouseClick);


	//m_debugListener.SetOwner(this);
	m_debugListener.SetFlags(DebugListenFor::AllLevels);
	
}

void LevelSelectState::Exit()
{
	using namespace Events;

	m_debugListener.ClearClients();



	EventManager()->UnregisterClient("Confirm", this, &LevelSelectState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &LevelSelectState::OnEscape);
	EventManager()->UnregisterClient("Up", this, &LevelSelectState::OnArrowUp);
	EventManager()->UnregisterClient("Down", this, &LevelSelectState::OnArrowDown);
	EventManager()->UnregisterClient("UpArr", this, &LevelSelectState::OnArrowUp);
	EventManager()->UnregisterClient("DownArr", this, &LevelSelectState::OnArrowDown);
	EventManager()->UnregisterClient("MouseLClick", this, &LevelSelectState::OnMouseClick);
	EventManager()->UnregisterClient("Right", this, &LevelSelectState::OnArrowDown);
	EventManager()->UnregisterClient("RightArr", this, &LevelSelectState::OnArrowDown);
	EventManager()->UnregisterClient("LeftArr", this, &LevelSelectState::OnArrowDown);
	EventManager()->UnregisterClient("Left", this, &LevelSelectState::OnArrowDown);


	if (m_controller)
	{
		m_controller->ClearMenuKeys();
		m_controller->ClearDebugKeys();
	}
		

	gameData->m_bResetGame = true;
	
}

void LevelSelectState::Initialize(Renderer* renderer)
{
	m_Renderer = renderer;

	m_ObjectFactory.Initialize();
	m_ObjectManager.Initialize();
	m_ObjectManager.SetFactory(&m_ObjectFactory);

	m_LevelManager.LoadLevelFile("Assets/XML/LevelXML/RATS_Levels.xml");
	m_unNumLevels = m_LevelManager.GetNumLevels();

	CreateHUD();

}

void LevelSelectState::Update(float dt)
{
	// keypad
	if (!gameData->m_bUsingGamepad)
	{
		fCheckTimer -= dt;
			if (fCheckTimer <= 0)
			{
				fCheckTimer = 0.1f;
				if (CheckMouseMove())
				{
					//g_AudioSystem->PlaySound("MENU_Hover");
					if (m_unCurrentRow != m_unLastRow || m_unCurrentCol != m_unLastCol)
					{
						m_unLastCol = m_unCurrentCol;
						m_unLastRow = m_unCurrentRow;
						g_AudioSystem->PlaySound("MENU_Hover");
					}
				}
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

// 			if (gamePad->GetState().state.dwPacketNumber != m_ulCurrPacketNo)
// 			{
// 				m_ulCurrPacketNo = gamePad->GetState().state.dwPacketNumber;
// 				bStickHeld = false;
// 				Handle360Input();
// 				globalGraphicsPointer->SetCrosshairShowing(false);
// 				fGamepadTimer = 0.15f;
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
	
	m_ObjectManager.Update(dt);
	UpdateButtons();
	SwitchButton(m_unCurrentRow, m_unCurrentCol);
}

void LevelSelectState::Terminate()
{
	m_LevelManager.Terminate();
}

void LevelSelectState::OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	SetRow(m_unCurrentRow - 1);
	g_AudioSystem->PlaySound("MENU_Hover");
}

void LevelSelectState::OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	SetRow(m_unCurrentRow + 1);
	g_AudioSystem->PlaySound("MENU_Hover");
}

void LevelSelectState::OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	SetCol(m_unCurrentCol + 1);
	g_AudioSystem->PlaySound("MENU_Hover");
}

void LevelSelectState::OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	SetCol(m_unCurrentCol - 1);
	g_AudioSystem->PlaySound("MENU_Hover");
}

void LevelSelectState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;


	if (m_unCurrentRow == m_vButtons.size() - 1)
	{
		ExitMenu();
		g_AudioSystem->PlaySound("MENU_Back");
	}
	else
	{
		if (m_vButtons[m_unCurrentRow][m_unCurrentCol].m_bLevelUnlocked)
		{
			EnterLevel(m_unSelectedLevel);
			g_AudioSystem->PlaySound("MENU_Accept");
		}
		else
		{
			g_AudioSystem->PlaySound("MENU_Error");
		}
	}
}

void LevelSelectState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	ExitMenu();
	g_AudioSystem->PlaySound("MENU_Back");
}

bool LevelSelectState::CheckMouseMove()
{
	if (!globalGraphicsPointer)
		return false;

	float x, y;
	Events::MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", x, y);

	RECT clientRect;

	GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

	x = x / (clientRect.right - clientRect.left);
	y = y / (clientRect.bottom - clientRect.top);

	return FindButton(x, y);
}

void LevelSelectState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (!globalGraphicsPointer)
		return;

	if (gameData->m_bUsingGamepad && args.GetData2() != -1.0f)
		return;

	float x, y;
	Events::MessageSystem::GetInstance()->Call<void, float&, float&>("GetMousePos", x, y);

	RECT clientRect;

	GetClientRect(globalGraphicsPointer->GetWindow(), &clientRect);

	x = x / (clientRect.right - clientRect.left);
	y = y / (clientRect.bottom - clientRect.top);

	if (FindButton(x, y))
	{
		if (m_unCurrentRow == m_vButtons.size() - 1)
		{
			ExitMenu();
			g_AudioSystem->PlaySound("MENU_Back");
		}
		else
		{
			if (m_vButtons[m_unCurrentRow][m_unCurrentCol].m_bLevelUnlocked)
			{
				EnterLevel(m_unSelectedLevel);
				g_AudioSystem->PlaySound("MENU_Accept");
			}
			else
			{
				g_AudioSystem->PlaySound("MENU_Error");
			}

		}
	}
}


void LevelSelectState::CreateHUD()
{

	//Create the menu background
	m_pMenuBackground = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "LevelSelectBackground", eRENDERCOMP_TYPE::Render2D_Back);
	RenderComp* menuBackground = (RenderComp*)m_pMenuBackground->GetComponent("RenderComp");
	if (menuBackground != nullptr)
	{
		menuBackground->m_posX = 0.0f;
		menuBackground->m_posY = 0.0f;
		menuBackground->m_height = 1.0f;
		menuBackground->m_width = 1.0f;
	}

	unsigned int currLevel = 0;
	RenderComp* pButtonRenderComp;
	RenderComp* pButtonTextRenderComp;
	RenderComp* pStarRenderComp;
	RenderComp* pShapeRenderComp;
	RenderComp* pLockRenderComp;
	string levelNum = "";
	int numTruncated = m_unNumLevels / m_unTotalNumCols;
	float numUntruncated = (float)m_unNumLevels / (float)m_unTotalNumCols;
	if (numUntruncated > numTruncated)
	{
		numTruncated++;
	}
	m_vButtons.resize(numTruncated);

	for (unsigned int row = 0; row < (unsigned int)numTruncated; row++)
	{
		for (unsigned int col = 0; col < m_unTotalNumCols; col++)
		{
			//Create new level button
			LevelButton newButton;

			//Set level button variables
			newButton.m_unLevelNumber = currLevel;
			newButton.m_unRow = row;
			newButton.m_unCol = col;
			newButton.m_bLevelUnlocked = gameData->m_Level_Information[currLevel].unlocked;
			newButton.m_bStar1Earned = gameData->m_Level_Information[currLevel].star_1;
			newButton.m_bStar2earned = gameData->m_Level_Information[currLevel].star_2;
			newButton.m_bStar3earned = gameData->m_Level_Information[currLevel].star_3;

			//Create main button object		
			newButton.m_pButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "LevelButton", eRENDERCOMP_TYPE::Render2D_Middle);
			

			//Setup main button position
			pButtonRenderComp = (RenderComp*)newButton.m_pButton->GetComponent("RenderComp");

			if (pButtonRenderComp)
			{
				pButtonRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				pButtonRenderComp->m_posX = (col * 0.15f) + 0.05f;
				pButtonRenderComp->m_posY = (row * 0.15f) + 0.05f;
				pButtonRenderComp->m_height = 0.15f;
				pButtonRenderComp->m_width = 0.15f;
			}

			//Create button level number text object
			levelNum = "";
			levelNum += to_string(currLevel + 1);
			newButton.m_pButtonText = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), levelNum, eRENDERCOMP_TYPE::Render2D_Text);
			pButtonTextRenderComp = (RenderComp*)newButton.m_pButtonText->GetComponent("RenderComp");
			if (pButtonTextRenderComp)
			{
				if (newButton.m_bLevelUnlocked)
				{
					//pButtonTextRenderComp->color = XMFLOAT4(0.23f, 0.28f, 0.34f, 1.0f);
					pButtonTextRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				}

				else
				{
					pButtonTextRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				}
				pButtonTextRenderComp->m_posX = pButtonRenderComp->m_posX + 0.015f;
				pButtonTextRenderComp->m_posY = pButtonRenderComp->m_posY + 0.015f;
				pButtonTextRenderComp->m_height = 0.2f;
				pButtonTextRenderComp->m_width = 0.2f;
			}

			//If the level is unlocked, set up the stars and shape image
			if (newButton.m_bLevelUnlocked)
			{
				//Set up the shape
				string levelShape = m_LevelManager.GetLevel(currLevel)->GetLevelShape();
				if (levelShape == "Sphere")
				{
					newButton.m_pShapeIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "SphereIcon", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else if (levelShape == "Torus")
				{
					newButton.m_pShapeIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "TorusIcon", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else
				{
					newButton.m_pShapeIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "CubeIcon", eRENDERCOMP_TYPE::Render2D_Front);
				}

				//Set up the position of the shape
				pShapeRenderComp = (RenderComp*)newButton.m_pShapeIcon->GetComponent("RenderComp");
				if (pShapeRenderComp)
				{
					pShapeRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pShapeRenderComp->m_posX = pButtonRenderComp->m_posX + 0.095f;
					pShapeRenderComp->m_posY = pButtonRenderComp->m_posY + 0.02f;
					pShapeRenderComp->m_height = 0.04f;
					pShapeRenderComp->m_width = 0.04f;
				}
				
				//Set Up Star 1
				if (newButton.m_bStar1Earned)
				{
					newButton.m_pStar1 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else
				{
					newButton.m_pStar1 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
				}

				pStarRenderComp = (RenderComp*)newButton.m_pStar1->GetComponent("RenderComp");

				if (pStarRenderComp)
				{
					pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.015f;
					pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.09f;
					pStarRenderComp->m_height = 0.04f;
					pStarRenderComp->m_width = 0.04f;
				}

				//Set Up Star 2
				if (newButton.m_bStar2earned)
				{
					newButton.m_pStar2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else
				{
					newButton.m_pStar2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
				}

				pStarRenderComp = (RenderComp*)newButton.m_pStar2->GetComponent("RenderComp");

				if (pStarRenderComp)
				{
					pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.055f;
					pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.07f;
					pStarRenderComp->m_height = 0.04f;
					pStarRenderComp->m_width = 0.04f;
				}

				//Set Up Star 3
				if (newButton.m_bStar3earned)
				{
					newButton.m_pStar3 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else
				{
					newButton.m_pStar3 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
				}

				pStarRenderComp = (RenderComp*)newButton.m_pStar3->GetComponent("RenderComp");

				if (pStarRenderComp)
				{
					pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.095f;
					pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.09f;
					pStarRenderComp->m_height = 0.04f;
					pStarRenderComp->m_width = 0.04f;
				}
			}
			else
			{
				newButton.m_pLock = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "Lock", eRENDERCOMP_TYPE::Render2D_Front);

				pLockRenderComp = (RenderComp*)newButton.m_pLock->GetComponent("RenderComp");
				if (pLockRenderComp)
				{
					pLockRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pLockRenderComp->m_posX = pButtonRenderComp->m_posX + 0.05f;
					pLockRenderComp->m_posY = pButtonRenderComp->m_posY + 0.07f;
					pLockRenderComp->m_height = 0.05f;
					pLockRenderComp->m_width = 0.05f;
				}

			}
			m_vButtons[row].push_back(newButton);

			currLevel++;
			if (currLevel == m_unNumLevels)
			{
				break;
			}
		}
	}

	m_vButtons.resize(m_vButtons.size() + 1);

	//Create Back Button
	LevelButton backButton;
	backButton.m_pButton = m_pBackButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButton", eRENDERCOMP_TYPE::Render2D_Front);
	RenderComp* pButton = (RenderComp*)backButton.m_pButton->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.4f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.1f;
		pButton->m_width  = 0.2f;
	}

	m_pBackButtonSelected = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "backButtonSelected");
	pButton = (RenderComp*)m_pBackButtonSelected->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = 0.4f;
		pButton->m_posY = 0.88f;
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}

	m_unCurrentRow = 0;
	m_unCurrentCol = 0;
	m_pSelectedButton = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "LevelButtonSelected");
	pButton = (RenderComp*)m_pSelectedButton->GetComponent("RenderComp");
	if (pButton != nullptr)
	{
		pButton->m_posX = GetButtonRenderComp(m_unCurrentRow, m_unCurrentCol)->m_posX;
		pButton->m_posY = GetButtonRenderComp(m_unCurrentRow, m_unCurrentCol)->m_posY;
		pButton->m_height = GetButtonRenderComp(m_unCurrentRow, m_unCurrentCol)->m_height;
		pButton->m_width = GetButtonRenderComp(m_unCurrentRow, m_unCurrentCol)->m_width;
	}


	m_vButtons[m_vButtons.size() - 1].push_back(backButton);

}

void LevelSelectState::UpdateButtons()
{
	unsigned int currLevel = 0;
	RenderComp* pButtonRenderComp;
	RenderComp* pButtonTextRenderComp;
	RenderComp* pStarRenderComp;
	string levelNum = "";
	int numTruncated = m_unNumLevels / m_unTotalNumCols;
	float numUntruncated = (float)m_unNumLevels / (float)m_unTotalNumCols;
	if (numUntruncated > numTruncated)
	{
		numTruncated++;
	}


	for (unsigned int row = 0; row < (unsigned int)(numTruncated); row++)
	{
		for (unsigned int col = 0; col < m_unTotalNumCols; col++)
		{
			//if the level has changed to unlocked
			if (m_vButtons[row][col].m_bLevelUnlocked != gameData->m_Level_Information[currLevel].unlocked)
			{
				m_vButtons[row][col].m_bLevelUnlocked = gameData->m_Level_Information[currLevel].unlocked;

				m_vButtons[row][col].m_bStar1Earned = gameData->m_Level_Information[currLevel].star_1;
				m_vButtons[row][col].m_bStar2earned = gameData->m_Level_Information[currLevel].star_2;
				m_vButtons[row][col].m_bStar3earned = gameData->m_Level_Information[currLevel].star_3;

				if (m_vButtons[row][col].m_bLevelUnlocked)
				{
					// kill the lock
					if (m_vButtons[row][col].m_pLock)
					{
						m_vButtons[row][col].m_pLock->SetDead();
					}

					//Set up the shape
					string levelShape = m_LevelManager.GetLevel(currLevel)->GetLevelShape();
					if (levelShape == "Sphere")
					{
						m_vButtons[row][col].m_pShapeIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "SphereIcon", eRENDERCOMP_TYPE::Render2D_Front);
					}
					else if (levelShape == "Torus")
					{
						m_vButtons[row][col].m_pShapeIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "TorusIcon", eRENDERCOMP_TYPE::Render2D_Front);
					}
					else
					{
						m_vButtons[row][col].m_pShapeIcon = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "CubeIcon", eRENDERCOMP_TYPE::Render2D_Front);
					}

					//Set up the position of the shape
					RenderComp* pShapeRenderComp = (RenderComp*)m_vButtons[row][col].m_pShapeIcon->GetComponent("RenderComp");
					if (pShapeRenderComp)
					{
						pShapeRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						pShapeRenderComp->m_posX = GetButtonRenderComp(row, col)->m_posX + 0.095f;
						pShapeRenderComp->m_posY = GetButtonRenderComp(row, col)->m_posY + 0.02f;
						pShapeRenderComp->m_height = 0.04f;
						pShapeRenderComp->m_width = 0.04f;
					}					

					////Setup main button position
					pButtonRenderComp = (RenderComp*)m_vButtons[row][col].m_pButton->GetComponent("RenderComp");

					//Set Up Star 1
					if (m_vButtons[row][col].m_bStar1Earned)
					{
						m_vButtons[row][col].m_pStar1 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
					}
					else
					{
						m_vButtons[row][col].m_pStar1 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
					}

					pStarRenderComp = (RenderComp*)m_vButtons[row][col].m_pStar1->GetComponent("RenderComp");

					if (pStarRenderComp)
					{
						pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.015f;
						pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.09f;
						pStarRenderComp->m_height = 0.04f;
						pStarRenderComp->m_width = 0.04f;
					}

					//Set Up Star 2
					if (m_vButtons[row][col].m_bStar2earned)
					{
						m_vButtons[row][col].m_pStar2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
					}
					else
					{
						m_vButtons[row][col].m_pStar2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
					}

					pStarRenderComp = (RenderComp*)m_vButtons[row][col].m_pStar2->GetComponent("RenderComp");

					if (pStarRenderComp)
					{
						pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.055f;
						pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.07f;
						pStarRenderComp->m_height = 0.04f;
						pStarRenderComp->m_width = 0.04f;
					}

					//Set Up Star 3
					if (m_vButtons[row][col].m_bStar3earned)
					{
						m_vButtons[row][col].m_pStar3 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
					}
					else
					{
						m_vButtons[row][col].m_pStar3 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
					}

					pStarRenderComp = (RenderComp*)m_vButtons[row][col].m_pStar3->GetComponent("RenderComp");

					if (pStarRenderComp)
					{
						pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.095f;
						pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.09f;
						pStarRenderComp->m_height = 0.04f;
						pStarRenderComp->m_width = 0.04f;
					}
				}
				else     //The button is now locked
				{
					//Kill the shape if it exists
					if (m_vButtons[row][col].m_pShapeIcon)
					{
						m_vButtons[row][col].m_pShapeIcon->SetDead();
					}

					//Add the lock if it doesn't exist					
					m_vButtons[row][col].m_pLock = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "Lock", eRENDERCOMP_TYPE::Render2D_Front);

					RenderComp* pLockRenderComp = (RenderComp*)m_vButtons[row][col].m_pLock->GetComponent("RenderComp");
					if (pLockRenderComp)
					{
						pLockRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
						pLockRenderComp->m_posX = GetButtonRenderComp(row, col)->m_posX + 0.05f;
						pLockRenderComp->m_posY = GetButtonRenderComp(row, col)->m_posY + 0.07f;
						pLockRenderComp->m_height = 0.05f;
						pLockRenderComp->m_width = 0.05f;
					}					
					

					//Create button level number text object				
					pButtonTextRenderComp = (RenderComp*)m_vButtons[row][col].m_pButtonText->GetComponent("RenderComp");
					if (pButtonTextRenderComp)
					{	
						pButtonTextRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);									
					}

					if (m_vButtons[row][col].m_pStar1 != nullptr)
					{
						m_vButtons[row][col].m_pStar1->SetDead();
					}

					if (m_vButtons[row][col].m_pStar2 != nullptr)
					{
						m_vButtons[row][col].m_pStar2->SetDead();
					}

					if (m_vButtons[row][col].m_pStar3 != nullptr)
					{
						m_vButtons[row][col].m_pStar3->SetDead();
					}
				}

			}

			//if star 1 changed, kill the button and remake it
			if (m_vButtons[row][col].m_bStar1Earned != gameData->m_Level_Information[currLevel].star_1)
			{
				m_vButtons[row][col].m_bStar1Earned = gameData->m_Level_Information[currLevel].star_1;
				if (m_vButtons[row][col].m_pStar1 != nullptr)
				{
					m_vButtons[row][col].m_pStar1->SetDead();
				}
				//Setup main button position
				pButtonRenderComp = (RenderComp*)m_vButtons[row][col].m_pButton->GetComponent("RenderComp");

				if (pButtonRenderComp)
				{
					pButtonRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pButtonRenderComp->m_posX = (col * 0.15f) + 0.05f;
					pButtonRenderComp->m_posY = (row * 0.15f) + 0.05f;
					pButtonRenderComp->m_height = 0.15f;
					pButtonRenderComp->m_width = 0.15f;
				}

				//Set Up Star 1
				if (m_vButtons[row][col].m_bStar1Earned)
				{
					m_vButtons[row][col].m_pStar1 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else
				{
					m_vButtons[row][col].m_pStar1 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
				}

				pStarRenderComp = (RenderComp*)m_vButtons[row][col].m_pStar1->GetComponent("RenderComp");

				if (pStarRenderComp)
				{
					pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.015f;
					pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.09f;
					pStarRenderComp->m_height = 0.04f;
					pStarRenderComp->m_width = 0.04f;
				}

			}

			//if star 2 changed, kill the button and remake it
			if (m_vButtons[row][col].m_bStar2earned != gameData->m_Level_Information[currLevel].star_2)
			{
				m_vButtons[row][col].m_bStar2earned = gameData->m_Level_Information[currLevel].star_2;
				if (m_vButtons[row][col].m_pStar2 != nullptr)
				{
					m_vButtons[row][col].m_pStar2->SetDead();
				}
				//Setup main button position
				pButtonRenderComp = (RenderComp*)m_vButtons[row][col].m_pButton->GetComponent("RenderComp");

				if (pButtonRenderComp)
				{
					pButtonRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pButtonRenderComp->m_posX = (col * 0.15f) + 0.05f;
					pButtonRenderComp->m_posY = (row * 0.15f) + 0.05f;
					pButtonRenderComp->m_height = 0.15f;
					pButtonRenderComp->m_width = 0.15f;
				}
				//Set Up Star 2
				if (m_vButtons[row][col].m_bStar2earned)
				{
					m_vButtons[row][col].m_pStar2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else
				{
					m_vButtons[row][col].m_pStar2 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
				}

				pStarRenderComp = (RenderComp*)m_vButtons[row][col].m_pStar2->GetComponent("RenderComp");

				if (pStarRenderComp)
				{
					pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.055f;
					pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.07f;
					pStarRenderComp->m_height = 0.04f;
					pStarRenderComp->m_width = 0.04f;
				}
			}

			//if star 3 changed, kill the button and remake it
			if (m_vButtons[row][col].m_bStar3earned != gameData->m_Level_Information[currLevel].star_3)
			{
				m_vButtons[row][col].m_bStar3earned = gameData->m_Level_Information[currLevel].star_3;
				if (m_vButtons[row][col].m_pStar3 != nullptr)
				{
					m_vButtons[row][col].m_pStar3->SetDead();
				}

				//Setup main button position
				pButtonRenderComp = (RenderComp*)m_vButtons[row][col].m_pButton->GetComponent("RenderComp");

				if (pButtonRenderComp)
				{
					pButtonRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pButtonRenderComp->m_posX = (col * 0.15f) + 0.05f;
					pButtonRenderComp->m_posY = (row * 0.15f) + 0.05f;
					pButtonRenderComp->m_height = 0.15f;
					pButtonRenderComp->m_width = 0.15f;
				}

				//Set Up Star 3
				if (m_vButtons[row][col].m_bStar3earned)
				{
					m_vButtons[row][col].m_pStar3 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front);
				}
				else
				{
					m_vButtons[row][col].m_pStar3 = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", eRENDERCOMP_TYPE::Render2D_Middle3);
				}

				pStarRenderComp = (RenderComp*)m_vButtons[row][col].m_pStar3->GetComponent("RenderComp");

				if (pStarRenderComp)
				{
					pStarRenderComp->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					pStarRenderComp->m_posX = pButtonRenderComp->m_posX + 0.095f;
					pStarRenderComp->m_posY = pButtonRenderComp->m_posY + 0.09f;
					pStarRenderComp->m_height = 0.04f;
					pStarRenderComp->m_width = 0.04f;
				}

			}


			currLevel++;
			if (currLevel == m_unNumLevels)
			{
				break;
			}
		}
	}
}

bool LevelSelectState::FindButton(float xPos, float yPos)
{
	float testXPos, testYPos;
	RenderComp* currComp;
	for (unsigned int row = 0; row < m_vButtons.size() - 1; row++)
	{
		for (unsigned int col = 0; col < m_vButtons[row].size(); col++)
		{
			currComp = (RenderComp*)m_vButtons[row][col].m_pButton->GetComponent("RenderComp");
			testXPos = xPos - currComp->m_posX;
			testYPos = yPos - currComp->m_posY;
			if (testXPos < 0 || testYPos < 0 ||
				testXPos > currComp->m_width || testYPos > currComp->m_height)
				continue;

			m_unCurrentRow = m_vButtons[row][col].m_unRow;
			m_unCurrentCol = m_vButtons[row][col].m_unCol;
			m_unSelectedLevel = m_vButtons[row][col].m_unLevelNumber;
			return true;
		}
	}

	currComp = (RenderComp*)m_pBackButton->GetComponent("RenderComp");
	testXPos = xPos - currComp->m_posX;
	testYPos = yPos - currComp->m_posY;
	if (testXPos < 0 || testYPos < 0 ||
		testXPos > currComp->m_width || testYPos > currComp->m_height)
	{
		return false;
	}
	else
	{
		m_unCurrentRow = m_vButtons.size() - 1;
		m_unCurrentCol = 0;
		m_unSelectedLevel = 0;
		return true;
	}

	return false;
}

bool LevelSelectState::Handle360Input()
{
	GamePad tmpPad = gamePad->GetState();
	Events::CGeneralEventArgs2<unsigned char, float> tmpArgs(0, -1.0f);
	bool detectedInput = false;

	if (tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bHeld ||
		tmpPad.buttons[buttonList::DPAD_LEFT] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::DPAD_LEFT] == buttonStatus::bHeld)
	{
		OnArrowLeft(tmpArgs);
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bHeld ||
		tmpPad.buttons[buttonList::DPAD_RIGHT] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::DPAD_RIGHT] == buttonStatus::bHeld)
	{
		OnArrowRight(tmpArgs);
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bHeld ||
		tmpPad.buttons[buttonList::DPAD_DOWN] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::DPAD_DOWN] == buttonStatus::bHeld)
	{
		OnArrowDown(tmpArgs);
		detectedInput = true;
	}

	if (tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bPress ||
		tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bHeld ||
		tmpPad.buttons[buttonList::DPAD_UP] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::DPAD_UP] == buttonStatus::bHeld)
	{
		OnArrowUp(tmpArgs);
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress
		|| tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{
		OnEnter(tmpArgs);
		detectedInput = true;
	}

	if (tmpPad.buttons[buttonList::B] == buttonStatus::bPress)
	{
		OnEscape(tmpArgs);
		detectedInput = true;
	}


	return detectedInput;

}

void LevelSelectState::SwitchButton(unsigned int row, unsigned int col)
{
	//g_AudioSystem->PlaySound("MENU_Hover");
	//ResetButtonColors();
	m_unCurrentCol = col;
	m_unCurrentRow = row;
	m_unSelectedLevel = m_vButtons[m_unCurrentRow][m_unCurrentCol].m_unLevelNumber;

	if (row == m_vButtons.size()-1)
	{
		//SetObjectColor(m_pBackButton, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		ShrinkButton(m_pSelectedButton);
		GrowButton(m_pBackButtonSelected, 0.1f, 0.2f);
	}
	else
	{
		//SetObjectColor(m_vButtons[row][col].m_pButton, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		ShrinkButton(m_pBackButtonSelected);
		RenderComp* pButton = GetButtonRenderComp(m_unCurrentRow, m_unCurrentCol);
		MoveButton(m_pSelectedButton, pButton->m_posX, pButton->m_posY);
		GrowButton(m_pSelectedButton, 0.15f, 0.15f);
	}
}

void LevelSelectState::EnterLevel(unsigned int level)
{
	globalGraphicsPointer->SetCrosshairShowing(true);
	gameData->m_levelShape = m_LevelManager.GetLevel(m_unSelectedLevel)->GetLevelShape();
	gameData->m_cur_level = m_unSelectedLevel;

	m_NextState = STATE_TYPE::UPGRADE_SHIP_STATE;
}

void LevelSelectState::ExitMenu()
{
	globalGraphicsPointer->SetCrosshairShowing(true);

	m_NextState = MAIN_MENU_STATE;
}


void LevelSelectState::SetObjectColor(GameObject* obj, XMFLOAT4 color)
{
	RenderComp* renderComp = (RenderComp*)obj->GetComponent("RenderComp");
	if (renderComp)
	{
		renderComp->color = color;
	}
}

void LevelSelectState::ResetButtonColors()
{
	for (unsigned int row = 0; row < m_vButtons.size() - 1; row++)
	{
		for (unsigned int col = 0; col < m_vButtons[row].size(); col++)
		{
			SetObjectColor(m_vButtons[row][col].m_pButton, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	SetObjectColor(m_pBackButton, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

}

void LevelSelectState::SetRow(int newRow)
{
	if (newRow > (int)(m_vButtons.size() - 1))
	{
		m_unCurrentRow = 0;
	}
	else if (newRow < 0)
	{
		m_unCurrentRow = m_vButtons.size() - 1;
	}
	else
	{
		m_unCurrentRow = newRow;
	}

	SetCol(m_unCurrentCol);

}

void LevelSelectState::SetCol(int newCol)
{
	if (newCol > (int)(m_vButtons[m_unCurrentRow].size() - 1))
	{
		m_unCurrentCol = 0;
	}
	else if (newCol < 0)
	{
		m_unCurrentCol = m_vButtons[m_unCurrentRow].size() - 1;
	}
	else
	{
		m_unCurrentCol = newCol;
	}


}

RenderComp* LevelSelectState::GetButtonRenderComp(unsigned int row, unsigned int col)
{
	return (RenderComp*)m_vButtons[row][col].m_pButton->GetComponent("RenderComp");
}

void LevelSelectState::MoveButton(GameObject* obj, float x, float y)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_posX = x;
		pButton->m_posY = y;
	}
}

void LevelSelectState::ShrinkButton(GameObject* obj)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = 0.0f;
		pButton->m_width = 0.0f;
	}
}

void LevelSelectState::GrowButton(GameObject* obj, float height, float width)
{
	RenderComp* pButton = (RenderComp*)obj->GetComponent("RenderComp");
	if (pButton)
	{
		pButton->m_height = height;
		pButton->m_width = width;
	}
}