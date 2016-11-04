#include "stdafx.h"
#include "ResultState.h"
#include "../BitmapFont/BitmapFontManager.h"
#include "../Input/InputController.h"
#include "../Renderer/D3DGraphics.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../../Wwise files/EventManager.h"
#include "../Renderer/RenderComp.h"
#include "../Upgrade System/GameData.h"
#include "../Object Manager/GameObject.h"

extern InputController* m_controller;
extern D3DGraphics* globalGraphicsPointer;
extern	FFmod*		g_AudioSystem;
extern ThreeSixty* gamePad;
extern GameData *gameData;

ResultState::ResultState()
{


}


ResultState::~ResultState()
{
}

void ResultState::Enter(BitmapFontManager* bmf, bool resetSound)
{
	// Set Renderer 
	this->m_pBitmapFontManager = bmf;

	if (m_controller)
		m_controller->SetMenuKeys();

	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());

	//CreateHud();
	//upgrades = gameData->m_upgrade_points;
	//std::cout << gameData->m_score_current;

	if (m_controller)
		m_controller->SetMenuKeys();

	if (resetSound)
	{
	}

	g_AudioSystem->StopBGM();
	g_AudioSystem->ResetListener();
	g_AudioSystem->PlayBGM("RATS_BGM_Results");

	using namespace Events;

	m_NextState = STATE_TYPE::RESULT_STATE;

	//Setup Input
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Confirm"), this, &ResultState::OnEnter);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "Return"), this, &ResultState::OnEscape);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "MouseLClick"), this, &ResultState::OnMouseClick);

	// Display strings for level statistics
	ShowHud();
}

void ResultState::Exit()
{
	using namespace Events;

	EventManager()->UnregisterClient("Confirm", this, &ResultState::OnEnter);
	EventManager()->UnregisterClient("Return", this, &ResultState::OnEscape);
	EventManager()->UnregisterClient("MouseLClick", this, &ResultState::OnMouseClick);

	if (m_controller)
		m_controller->ClearMenuKeys();

	gameData->m_bPlayerDead = false;
}

void ResultState::Initialize(Renderer* renderer)
{
	m_Renderer = renderer;
	m_ObjectManager.Initialize();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory(&m_ObjectFactory);

	CreateHud();

	/*GameObject *grab;

	grab = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0,0,0), m_score_total, Render2D_Text);
	ScoreTotal = reinterpret_cast<RenderComp*>(grab->GetComponent("RenderComp"));

	grab = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0,0,0), m_score_needed1, Render2D_Text);
	ScoreNeeded1 = reinterpret_cast<RenderComp*>(grab->GetComponent("RenderComp"));

	grab = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0,0,0), m_score_needed2, Render2D_Text);
	ScoreNeeded2 = reinterpret_cast<RenderComp*>(grab->GetComponent("RenderComp"));

	grab = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0,0,0), m_pylons_alive, Render2D_Text);
	PylonsAlive = reinterpret_cast<RenderComp*>(grab->GetComponent("RenderComp"));*/
}

void ResultState::Update(float dt)
{
	m_ObjectManager.Update(dt);
	// Select button (A)
	if (gameData->m_bUsingGamepad)
		Handle360Input();
}

void ResultState::Terminate()
{
}

void ResultState::CreateHud()
{
	// Text Strings on this Menu
	GameObject *obj;
	RenderComp *comp;

	m_pMenuBackground = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "menuBackground", eRENDERCOMP_TYPE::Render2D_Back);
	RenderComp* menuBackground = (RenderComp*)m_pMenuBackground->GetComponent("RenderComp");
	if (menuBackground != nullptr)
	{
		menuBackground->m_posX = 0.0f;
		menuBackground->m_posY = 0.0f;
		menuBackground->m_height = 1.0f;
		menuBackground->m_width = 1.0f;
	}

	// Screen Title
	m_score = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "Score:", Render2D_Text);
	comp = (RenderComp*)m_score->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.125f;
		comp->m_posY = 0.35f;
		comp->m_height = 0.3f;
		comp->m_width = 0.3f;
	}

	m_time = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "Time:", Render2D_Text);
	comp = (RenderComp*)m_time->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.125f;
		comp->m_posY = 0.45f;
		comp->m_height = 0.3f;
		comp->m_width = 0.3f;
	}


	// Screen Title
	obj = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), m_title, Render2D_Text);
	Title = (RenderComp*)obj->GetComponent("RenderComp");

	if (obj != nullptr)
	{
		Title->m_posX = 0.125f;
		Title->m_posY = 0.1f;
		Title->m_height = 0.3f;
		Title->m_width = 0.4f;
	}

	// Score Goal
	obj = m_ObjectManager.CreateObject(eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3(0, 0, 0), m_score_needed, Render2D_Text);
	ScoreNeeded = (RenderComp*)obj->GetComponent("RenderComp");

	if (obj != nullptr)
	{
		ScoreNeeded->m_posX = 0.36f + 0.025f + 0.025f;
		ScoreNeeded->m_posY = 0.35f;
		ScoreNeeded->m_height = 0.3f;
		ScoreNeeded->m_width = 0.3f;
	}

	// Time Goal
	obj = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), m_time_needed, Render2D_Text);
	TimeNeeded = (RenderComp*)obj->GetComponent("RenderComp");

	if (obj != nullptr)
	{
		TimeNeeded->m_posX = 0.36f + 0.025f + 0.025f;
		TimeNeeded->m_posY = 0.45f;
		TimeNeeded->m_height = 0.3f;
		TimeNeeded->m_width = 0.3f;
	}

	obj = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), m_level_passed, Render2D_Text);
	LevelStatus = (RenderComp*)obj->GetComponent("RenderComp");

	if (obj != nullptr)
	{
		LevelStatus->m_posX = 0.36f + 0.025f + 0.025f;
		LevelStatus->m_posY = 0.25f;
		LevelStatus->m_height = 0.3f;
		LevelStatus->m_width = 0.3f;
	}

	m_levelComplete = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "Result:", Render2D_Text);
	comp = (RenderComp*)m_levelComplete->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.1f + 0.025f;
		comp->m_posY = 0.25f;
		comp->m_height = 0.3f;
		comp->m_width = 0.3f;
	}

	//Dead Stars
	resultStarDead = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", Render2D_Front);

	comp = (RenderComp*)resultStarDead->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.2f;
		comp->m_posY = 0.65f;
		comp->m_height = 0.25f;
		comp->m_width = 0.25f;
	}
	scoreStarDead = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", Render2D_Front);

	comp = (RenderComp*)scoreStarDead->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.45f;
		comp->m_posY = 0.56f;
		comp->m_height = 0.25f;
		comp->m_width = 0.25f;
	}
	timeStarDead = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "UnearnedStar", Render2D_Front);

	comp = (RenderComp*)timeStarDead->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.7f;
		comp->m_posY = 0.65f;
		comp->m_height = 0.25f;
		comp->m_width = 0.25f;
	}

	//Alive stars
	resultStarAlive = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", Render2D_Front);

	comp = (RenderComp*)resultStarAlive->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.2f;
		comp->m_posY = 0.65f;
		comp->m_height = 0.25f;
		comp->m_width = 0.25f;
	}

	scoreStarAlive = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", Render2D_Front);

	comp = (RenderComp*)scoreStarAlive->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.45f;
		comp->m_posY = 0.56f;
		comp->m_height = 0.25f;
		comp->m_width = 0.25f;
	}

	timeStarAlive = m_ObjectManager.CreateObject(HUD_2D, DirectX::XMFLOAT3(0, 0, 0), "EarnedStar", Render2D_Front);

	comp = (RenderComp*)timeStarAlive->GetComponent("RenderComp");

	if (comp != nullptr)
	{
		comp->m_posX = 0.7f;
		comp->m_posY = 0.65f;
		comp->m_height = 0.25f;
		comp->m_width = 0.25f;
	}

}

void ResultState::ShowHud()
{
	upgrades = 0;

	// Did you survive, then getting to this screen means you beat the level!
	if (!gameData->m_bPlayerDead)
	{
		if (gameData->AchieveStar(1))	// Winning a level gets you the first star, always!
		{
			upgrades++;
		}

		m_level_passed = "Victory!";
		LevelStatus->color = DirectX::XMFLOAT4(0, 1, 0, 1);

		resultStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
		resultStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);
	}

	// You died, you didn't get any stars
	else
	{
		m_level_passed = "Defeat!";
		resultStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);
		resultStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
		LevelStatus->color = DirectX::XMFLOAT4(1, 0, 0, 1);
		ShowDeadHud();	// One option
		return;
	}


	// Set colors to mark success per goal
	if (gameData->m_score_current > score)
	{
		scoreStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
		scoreStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);

		ScoreNeeded->color = DirectX::XMFLOAT4(0, 1, 0, 1);
		if (gameData->AchieveStar(2))
			upgrades++;
	}

	else
	{
		scoreStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);
		scoreStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
		ScoreNeeded->color = DirectX::XMFLOAT4(1, 0, 0, 1);
	}


	if (gameData->m_level_timer < time)
	{

		timeStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
		timeStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);

		TimeNeeded->color = DirectX::XMFLOAT4(0, 1, 0, 1);
		if (gameData->AchieveStar(3))
			upgrades++;
	}

	else
	{

		timeStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);
		timeStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);

		TimeNeeded->color = DirectX::XMFLOAT4(1, 0, 0, 1);
	}

	gameData->m_upgrade_points += upgrades;
	ShowStrings();

	// Update game information, save it to file
	gameData->LevelSave();
	gameData->SaveGame();
}

void ResultState::ShowDeadHud()
{
	TimeNeeded->color = DirectX::XMFLOAT4(1, 0, 0, 1);
	ScoreNeeded->color = DirectX::XMFLOAT4(1, 0, 0, 1);
	LevelStatus->color = DirectX::XMFLOAT4(1, 0, 0, 1);

	timeStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);
	timeStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
	scoreStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);
	scoreStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);
	resultStarAlive->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 0);
	resultStarDead->GetComponent<RenderComp>()->color = XMFLOAT4(1, 1, 1, 1);

	ShowStrings();
}

void ResultState::ShowStrings()
{
	// Assign goal strings (level passing and upgrade points is handled in ShowHud() and ShowDeadHud() )
	m_score_needed = std::to_string(gameData->m_score_current) + " / " + std::to_string(score);

	int secondsTaken = (((int)gameData->m_level_timer) % 60);

	string time1;

	if (secondsTaken < 10)
	{
		time1 = std::to_string(((int)gameData->m_level_timer) / 60) + ":0" + std::to_string(secondsTaken);
	}
	else
	{
		time1 = std::to_string(((int)gameData->m_level_timer) / 60) + ":" + std::to_string(secondsTaken);

	}

	int secondsNeeded = (((int)time) % 60);

	string time2;

	if (secondsNeeded < 10)
	{
		time2 = std::to_string(((int)time) / 60) + ":0" + std::to_string(secondsNeeded);

	}
	else
	{
		time2 = std::to_string(((int)time) / 60) + ":" + std::to_string(secondsNeeded);

	}

	m_time_needed = time1 + " / " + time2;

	//m_time_needed = "Time Goal: " + std::to_string((int)time);
	//m_time_total = "Your Time: " + std::to_string((int)gameData->m_level_timer);
	m_title = "Mission Summary";

	// Assign Render Comp Strings
	Title->m_renderInfo = nullptr;
	ScoreNeeded->m_renderInfo = nullptr;
	TimeNeeded->m_renderInfo = nullptr;
	LevelStatus->m_renderInfo = nullptr;



	Title->SetID(m_title);
	ScoreNeeded->SetID(m_score_needed);
	TimeNeeded->SetID(m_time_needed);
	LevelStatus->SetID(m_level_passed);
}


///////////////////////////////
///// KEY EVENT FUNCTIONS /////
///////////////////////////////

void ResultState::OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;
	// Continue!
	NextScreen();
}


void ResultState::OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;
	// Continue!
	NextScreen();
}

void ResultState::OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (gameData->m_bUsingGamepad)
		return;
	// Continue!
	NextScreen();
}

void ResultState::Handle360Input()
{
	/////////////////////////////////////////////////////////////
	// TODO: Setup states code to handle hovering over buttons //

	GamePad tmpPad = gamePad->GetState();

	// Select button (A)
	// Start Button pressed: Save and Exit
	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{
		NextScreen();
	}



}

void ResultState::NextScreen()
{


	g_AudioSystem->PlaySound("MENU_Accept");
	gameData->SaveGame();

	if (globalGraphicsPointer->justBeatTheGame)
	{
		globalGraphicsPointer->justBeatTheGame = false;
		m_NextState = CREDITS_STATE;
	}
	else
	{
		m_NextState = LEVEL_SELECT_STATE;

	}



	// Destroy objects?
}