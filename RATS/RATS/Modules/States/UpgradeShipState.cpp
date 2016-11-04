#include "stdafx.h"
#include "UpgradeShipState.h"
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

UpgradeShipState::UpgradeShipState()
{
	m_vButtons.resize( MAX_UPGRADE_BUTTONS );
	m_DisplayDmgUpgrade = "Dmg: ";
	m_DisplaySpdUpgrade = "Spd: ";
	m_DisplayNumUpgrade = "Num: ";
	m_Count = "Upgrade Points to Spend: ";
	m_UpgradeCost = "Point Cost: ";

	starting_points = gameData->m_upgrade_points;

	whip_dmg = gameData->m_Whip.dmg_upgrade_num;
	whip_spd = gameData->m_Whip.fire_rate_upgrade_num;

	spread_dmg = gameData->m_Spread.dmg_upgrade_num;
	spread_num = gameData->m_Spread.fire_rate_upgrade_num;

	missile_dmg = gameData->m_Missile.dmg_upgrade_num;
	missile_num = gameData->m_Missile.fire_rate_upgrade_num;


	m_selected_button = WhipDmgDown;
	dwPacketNo = 0;

}


UpgradeShipState::~UpgradeShipState()
{
}


void UpgradeShipState::Enter( BitmapFontManager* bmfm, bool resetSound )
{
	this->m_pBitmapFontManager = bmfm;

	if ( m_controller )
	{
		m_controller->SetMenuKeys();
		m_controller->SetDebugKeys();
	}

	m_debugListener.SetFlags( MaxCreds );
	m_debugListener.SetOwner( this );

	m_Renderer->SetRenderList( m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets() );
	m_Renderer->SetRenderSizes( m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes() );

	ResetUpgrades();

	fCheckTimer = fGamepadTimer = 0.0f;
	fGamepadTimer = 0.0f;
	bStickHeld = false;
	HilightButton( WhipDmgDown );
	m_selected_button = WhipDmgDown;
	dwPacketNo = gamePad->GetState().state.dwPacketNumber;

	starting_points = gameData->m_upgrade_points;
	UpdatePoints();



	using namespace Events;

	m_NextState = STATE_TYPE::UPGRADE_SHIP_STATE;

	//Setup Input
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Confirm" ), this, &UpgradeShipState::OnEnter );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Return" ), this, &UpgradeShipState::OnEscape );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Up" ), this, &UpgradeShipState::OnArrowUp );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Down" ), this, &UpgradeShipState::OnArrowDown );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "UpArr" ), this, &UpgradeShipState::OnArrowUp );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "DownArr" ), this, &UpgradeShipState::OnArrowDown );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Left" ), this, &UpgradeShipState::OnArrowLeft );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "LeftArr" ), this, &UpgradeShipState::OnArrowLeft );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Right" ), this, &UpgradeShipState::OnArrowRight );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "RightArr" ), this, &UpgradeShipState::OnArrowRight );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "MouseLClick" ), this, &UpgradeShipState::OnMouseClick );

	MessageSystem::GetInstance()->Call<void, float&, float&>( "GetMousePos", fMousePrevX, fMousePrevY );
}


void UpgradeShipState::Exit()
{
	m_debugListener.ClearClients();

	using namespace Events;

	EventManager()->UnregisterClient( "Confirm", this, &UpgradeShipState::OnEnter );
	EventManager()->UnregisterClient( "Return", this, &UpgradeShipState::OnEscape );
	EventManager()->UnregisterClient( "Up", this, &UpgradeShipState::OnArrowUp );
	EventManager()->UnregisterClient( "Down", this, &UpgradeShipState::OnArrowDown );
	EventManager()->UnregisterClient( "UpArr", this, &UpgradeShipState::OnArrowUp );
	EventManager()->UnregisterClient( "DownArr", this, &UpgradeShipState::OnArrowDown );
	EventManager()->UnregisterClient( "MouseLClick", this, &UpgradeShipState::OnMouseClick );
	EventManager()->UnregisterClient( "Right", this, &UpgradeShipState::OnArrowDown );
	EventManager()->UnregisterClient( "RightArr", this, &UpgradeShipState::OnArrowDown );
	EventManager()->UnregisterClient( "LeftArr", this, &UpgradeShipState::OnArrowDown );
	EventManager()->UnregisterClient( "Left", this, &UpgradeShipState::OnArrowDown );

	if ( m_controller )
	{
		m_controller->ClearMenuKeys();
		m_controller->ClearDebugKeys();
	}

	gameData->ResetScore();
}

void UpgradeShipState::ResetUpgrades( void )
{
	starting_points = gameData->m_upgrade_points;

	whip_dmg = gameData->m_Whip.dmg_upgrade_num;
	whip_spd = gameData->m_Whip.fire_rate_upgrade_num;

	spread_dmg = gameData->m_Spread.dmg_upgrade_num;
	spread_num = gameData->m_Spread.fire_rate_upgrade_num;

	missile_dmg = gameData->m_Missile.dmg_upgrade_num;
	missile_num = gameData->m_Missile.fire_rate_upgrade_num;

	WhipDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( whip_dmg ) );
	WhipSpdDisplay->SetID( m_DisplaySpdUpgrade + std::to_string( whip_spd ) );
	SpreadDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( spread_dmg ) );
	SpreadNumDisplay->SetID( m_DisplayNumUpgrade + std::to_string( spread_num ) );
	MissileDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( missile_dmg ) );
	MissileNumDisplay->SetID( m_DisplayNumUpgrade + std::to_string( missile_num ) );
	NumPoints->SetID( m_Count + std::to_string( starting_points ) );

	WhipDmgDisplay->m_renderInfo = nullptr;
	WhipSpdDisplay->m_renderInfo = nullptr;
	SpreadDmgDisplay->m_renderInfo = nullptr;
	SpreadNumDisplay->m_renderInfo = nullptr;
	MissileDmgDisplay->m_renderInfo = nullptr;
	MissileNumDisplay->m_renderInfo = nullptr;
	NumPoints->m_renderInfo = nullptr;

}

void UpgradeShipState::Initialize( Renderer* renderer )
{
	m_Renderer = renderer;

	//Initialize the ObjectManager and the ObjectFactory
	m_ObjectManager.Initialize();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory( &m_ObjectFactory );

	CreateHud();

}

void UpgradeShipState::CreateHud()
{
	if ( Created )	// Not sure if this is still relevant
	{
		return;
	}

	//Create the menu background
	m_pMenuBackground = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "menuBackground", eRENDERCOMP_TYPE::Render2D_Back );
	RenderComp* menuBackground = (RenderComp*)m_pMenuBackground->GetComponent( "RenderComp" );
	if ( menuBackground != nullptr )
	{
		menuBackground->m_posX = 0.0f;
		menuBackground->m_posY = 0.0f;
		menuBackground->m_height = 1.0f;
		menuBackground->m_width = 1.0f;
	}

	//Create the Upgrade Buttons
	GameObject* obj;
	RenderComp* button;

	// Whip Icon
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "iconWhipGun", eRENDERCOMP_TYPE::Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		button->m_posX = 0.03f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.03f + 0.1f;
		button->m_height = 0.17f;
		button->m_width = 0.17f;
	}

	//m_vButtons[WhipGun] = button;


	//Create the Upgrade Left Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "leftArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.22f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.04f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[WhipDmgDown] = button;

	//Create the Upgrade Right Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "rightArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.3f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.04f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[WhipDmgUp] = button;

	// Text for Whip Dmg
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_DisplayDmgUpgrade + std::to_string( whip_dmg ), Render2D_Text );
	WhipDmgDisplay = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		WhipDmgDisplay->m_posX = 0.22f + 0.1f + 0.1f + 0.05f;
		WhipDmgDisplay->m_posY = 0.15f + 0.1f;
		WhipDmgDisplay->m_height = 0.15f;
		WhipDmgDisplay->m_width = 0.2f;
	}


	//Create the Whp Speed Left Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "leftArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.43f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.04f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[WhipSpdDown] = button;

	//Create the Whip Speed Right Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "rightArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.51f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.04f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[WhipSpdUp] = button;

	// Text for Whip Speed
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_DisplaySpdUpgrade + std::to_string( whip_spd ), Render2D_Text );
	WhipSpdDisplay = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		WhipSpdDisplay->m_posX = 0.43f + 0.1f + 0.1f + 0.05f;
		WhipSpdDisplay->m_posY = 0.15f + 0.1f;
		WhipSpdDisplay->m_height = 0.15f;
		WhipSpdDisplay->m_width = 0.2f;
	}

	// Spread Dmg Identification
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "iconSpreadGun", eRENDERCOMP_TYPE::Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		button->m_posX = 0.03f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.21f + 0.1f;
		button->m_height = 0.17f;
		button->m_width = 0.17f;
	}

	//m_vButtons[SpreadGun] = button;

	//Create the Spreaddmg Upgrade Left Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "leftArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.22f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.22f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[SpreadDmgDown] = button;

	//Create the Spreaddmg Upgrade Right Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "rightArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.3f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.22f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[SpreadDmgUp] = button;

	//Create the NumBullets Upgrade Left Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "leftArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.43f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.22f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[SpreadNumDown] = button;

	//Create the Numbullets Upgrade Right Arrow
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "rightArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.51f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.22f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[SpreadNumUp] = button;

	//////////////////////////////////////////////
	//////////////////////////////////////////////

	// Missile Weapon Icon
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "iconMissileGun", eRENDERCOMP_TYPE::Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		button->m_posX = 0.03f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.39f + 0.1f;
		button->m_height = 0.17f;
		button->m_width = 0.17f;
	}


	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, float3( 0, 0, 0 ), "leftArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.22f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.4f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[MissileDmgDown] = button;

	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, float3( 0, 0, 0 ), "rightArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.3f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.4f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[MissileDmgUp] = button;


	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, float3( 0, 0, 0 ), "leftArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.43f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.4f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[MissileNumDown] = button;

	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, float3( 0, 0, 0 ), "rightArrow", Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		button->m_posX = 0.51f + 0.1f + 0.1f + 0.05f;
		button->m_posY = 0.4f + 0.1f;
		button->m_height = 0.05f;
		button->m_width = 0.05f;
	}

	m_vButtons[MissileNumUp] = button;


	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_DisplayDmgUpgrade + std::to_string( missile_dmg ), Render2D_Text );
	MissileDmgDisplay = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		MissileDmgDisplay->m_posX = 0.22f + 0.1f + 0.1f + 0.05f;
		MissileDmgDisplay->m_posY = 0.48f + 0.1f;
		MissileDmgDisplay->m_height = 0.15f;
		MissileDmgDisplay->m_width = 0.2f;

	}

	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_DisplayNumUpgrade + std::to_string( missile_num ), Render2D_Text );
	MissileNumDisplay = (RenderComp*)obj->GetComponent( "RenderComp" );
	if ( button != nullptr )
	{
		MissileNumDisplay->m_posX = 0.43f + 0.1f + 0.1f + 0.05f;
		MissileNumDisplay->m_posY = 0.48f + 0.1f;
		MissileNumDisplay->m_height = 0.15f;
		MissileNumDisplay->m_width = 0.2f;
	}



	////////////////////////////////////////////
	// Buttons that leave the state

	// Save and Exit
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "backToMainMenuButton", eRENDERCOMP_TYPE::Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		button->m_posX = 0.75f;
		button->m_posY = 0.8f;
		button->m_height = 0.15f;
		button->m_width = 0.2f;
		button->color.w = 1.0f;
	}

	m_vButtons[Exit_USS] = button;

	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "resumeGameButton", eRENDERCOMP_TYPE::Render2D_Back );
	button = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		button->m_posX = 0.5f;
		button->m_posY = 0.8f;
		button->m_height = 0.15f;
		button->m_width = 0.2f;
	}

	m_vButtons[Save_USS] = button;

	//STAR

	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "EarnedStar", eRENDERCOMP_TYPE::Render2D_Front );
	starToSpend = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( starToSpend != nullptr )
	{
		starToSpend->m_posX = 0.84f;
		starToSpend->m_posY = 0.702f;
		starToSpend->m_height = 0.03f;
		starToSpend->m_width = 0.03f;
	}


	// Text Strings on this Menu

	// Spread Damage Display
	obj = m_ObjectManager.CreateObject( HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_DisplayDmgUpgrade + std::to_string( spread_dmg ), Render2D_Text );
	SpreadDmgDisplay = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		SpreadDmgDisplay->m_posX = 0.22f + 0.1f + 0.1f + 0.05f;
		SpreadDmgDisplay->m_posY = 0.32f + 0.1f;
		SpreadDmgDisplay->m_height = 0.15f;
		SpreadDmgDisplay->m_width = 0.2f;
	}

	// Text for Spread Bullets
	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_DisplayNumUpgrade + std::to_string( spread_num ), Render2D_Text );
	SpreadNumDisplay = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		SpreadNumDisplay->m_posX = 0.43f + 0.1f + 0.1f + 0.05f;
		SpreadNumDisplay->m_posY = 0.32f + 0.1f;
		SpreadNumDisplay->m_height = 0.15f;
		SpreadNumDisplay->m_width = 0.2f;
	}

	// Number of points to spend
	obj = m_ObjectManager.CreateObject( HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_Count + std::to_string( starting_points ), Render2D_Text );
	NumPoints = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( button != nullptr )
	{
		NumPoints->m_posX = 0.15f + 0.07f;
		NumPoints->m_posY = 0.7f;
		NumPoints->m_height = 0.15f;
		NumPoints->m_width = 0.2f;
	}

	obj = m_ObjectManager.CreateObject( HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "Whip", Render2D_Text );
	RenderComp* temp = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( temp != nullptr )
	{
		temp->m_posX = 0.11f;
		temp->m_posY = 0.18f;
		temp->m_height = 0.2f;
		temp->m_width = 0.2f;
	}

	obj = m_ObjectManager.CreateObject( HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "Spread", Render2D_Text );
	temp = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( temp != nullptr )
	{
		temp->m_posX = 0.11f;
		temp->m_posY = 0.2f + 0.165f;
		temp->m_height = 0.2f;
		temp->m_width = 0.2f;
	}

	obj = m_ObjectManager.CreateObject( HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), "Missile", Render2D_Text );
	temp = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( temp != nullptr )
	{
		temp->m_posX = 0.11f;
		temp->m_posY = 0.2f + 0.18f + 0.165f;
		temp->m_height = 0.2f;
		temp->m_width = 0.2f;
	}

	obj = m_ObjectManager.CreateObject( eOBJECT_TYPE::HUD_2D, DirectX::XMFLOAT3( 0, 0, 0 ), m_UpgradeCost + std::to_string( 0 ), Render2D_Text );
	UpCostDisplay = (RenderComp*)obj->GetComponent( "RenderComp" );

	if ( UpCostDisplay != nullptr )
	{
		UpCostDisplay->m_posX = 0.2f;
		UpCostDisplay->m_posY = 0.85f;
		UpCostDisplay->m_height = 0.15f;
		UpCostDisplay->m_width = 0.2f;
	}

	Created = true;
}


bool UpgradeShipState::CheckButton( int index, float x, float y )
{
	if ( m_vButtons[index] == nullptr )
		return false;

	if ( x > m_vButtons[index]->m_posX && x <= m_vButtons[index]->m_posX + m_vButtons[index]->m_width &&
		 y > m_vButtons[index]->m_posY && y <= m_vButtons[index]->m_posY + m_vButtons[index]->m_height )
	{
		// Play Sound
		//g_AudioSystem->PostEvent();
		return true;
	}

	return false;
}

void UpgradeShipState::UpdatePoints()
{
	NumPoints->SetID( m_Count + std::to_string( starting_points ) );
	NumPoints->m_renderInfo = nullptr;
}

int UpgradeShipState::HowManyPoints()
{
	int points = 0;

	// WHIP
	if ( m_selected_button == WhipDmgUp )
	{
		points = whip_dmg + 1;
	}

	if ( m_selected_button == WhipDmgDown )
	{
		points = whip_dmg;
	}

	if ( m_selected_button == WhipSpdUp )
	{
		points = whip_spd + 1;
	}

	if ( m_selected_button == WhipSpdDown )
	{
		points = whip_spd;
	}

	// SPREAD
	if ( m_selected_button == SpreadDmgUp )
	{
		points = spread_dmg + 1;
	}

	if ( m_selected_button == SpreadDmgDown )
	{
		points = spread_dmg;
	}

	if ( m_selected_button == SpreadNumUp )
	{
		points = spread_num + 1;
	}

	if ( m_selected_button == SpreadNumDown )
	{
		points = spread_num;
	}

	// MISSILE
	if ( m_selected_button == MissileDmgUp )
	{
		points = missile_dmg + 1;
	}

	if ( m_selected_button == MissileDmgDown )
	{
		points = missile_dmg;
	}

	if ( m_selected_button == MissileNumUp )
	{
		points = missile_num + 1;
	}

	if ( m_selected_button == MissileNumDown )
	{
		points = missile_num;
	}


	return points;
}

void UpgradeShipState::Update( float dt )
{


	if ( starting_points >= 10 )
	{
		starToSpend->m_posX = 0.885f;
	}
	else
	{
		starToSpend->m_posX = 0.85f;
	}

	// GAMEPAD STUFF
	if ( gameData->m_bUsingGamepad )
	{
		fGamepadTimer -= dt;
		if ( fGamepadTimer <= 0 )
		{
			if ( Handle360Input() )
				fGamepadTimer = 0.1f;

			// 			if (gamePad->GetState().state.dwPacketNumber != dwPacketNo)
			// 			{
			// 				dwPacketNo = gamePad->GetState().state.dwPacketNumber;
			// 				bStickHeld = false;
			// 				Handle360Input();
			// 				globalGraphicsPointer->SetCrosshairShowing(false);
			// 
			// 				fGamepadTimer = 0.13f;
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

	// Key/Mouse
	else
	{
		CheckMouseMove();

		Events::MessageSystem::GetInstance()->Call<void, float&, float&>( "GetMousePos", fMouseX, fMouseY );
		if ( fMouseX != fMousePrevX || fMouseY != fMousePrevY )
		{
			globalGraphicsPointer->SetCrosshairShowing( true );
			fMousePrevX = fMouseX;
			fMousePrevY = fMouseY;
		}
	}

	//std::cout << (int)m_ObjectFactory.m_RenderBuckets[4][0][4] << endl;


	//std::cout<< m_ObjectFactory.m_RenderSizes[4][0] << endl;


}

void UpgradeShipState::Terminate()
{

}

void UpgradeShipState::OnArrowRight( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_bUsingGamepad )
		return;

	ChangeButton( m_selected_button + 1 );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void UpgradeShipState::OnArrowLeft( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_bUsingGamepad )
		return;

	ChangeButton( m_selected_button - 1 );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void UpgradeShipState::OnArrowUp( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_bUsingGamepad )
		return;

	//using namespace Events;
	ChangeButton( m_selected_button - 4 );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void UpgradeShipState::OnArrowDown( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_bUsingGamepad )
		return;

	//using namespace Events;
	ChangeButton( m_selected_button + 4 );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void UpgradeShipState::OnEnter( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_bUsingGamepad )
		return;

	// Do Selected Button
	DoWorkFunction( m_selected_button );
	globalGraphicsPointer->SetCrosshairShowing( true );
}

void UpgradeShipState::OnEscape( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_bUsingGamepad )
		return;

	// Exit Menu no save
	globalGraphicsPointer->SetCrosshairShowing( true );
	ExitState();
}

void UpgradeShipState::OnMouseClick( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_bUsingGamepad )
		return;

	float x, y;
	Events::MessageSystem::GetInstance()->Call<void, float&, float&>( "GetMousePos", x, y );

	RECT clientRect;
	if ( !globalGraphicsPointer )
		return;

	GetClientRect( globalGraphicsPointer->GetWindow(), &clientRect );

	x = x / (clientRect.right - clientRect.left);
	y = y / (clientRect.bottom - clientRect.top);

	for ( unsigned int i = 0; i < m_vButtons.size(); i++ )
	{
		if ( CheckButton( i, x, y ) )
		{
			// Do Function
			DoWorkFunction( i );
			return;
		}
	}
}

void UpgradeShipState::DoWorkFunction( int button )
{
	if ( button != Exit_USS )
		g_AudioSystem->PlaySound( "MENU_Accept" );

	switch ( button )
	{

		case WhipDmgUp:
			UpgradeWhipDmg();
			break;

		case WhipDmgDown:
			DowngradeWhipDmg();
			break;

		case WhipSpdUp:
			UpgradeWhipSpd();
			break;

		case WhipSpdDown:
			DowngradeWhipSpd();
			break;

		case SpreadDmgDown:
			DowngradeSpreadDmg();
			break;

		case SpreadDmgUp:
			UpgradeSpreadDmg();
			break;

		case SpreadNumDown:
			DowngradeSpreadNum();
			break;

		case SpreadNumUp:
			UpgradeSpreadNum();
			break;

		case MissileDmgDown:
			DowngradeMissileDmg();
			break;

		case MissileDmgUp:
			UpgradeMissileDmg();
			break;

		case MissileNumDown:
			DowngradeMissileNum();
			break;

		case MissileNumUp:
			UpgradeMissileNum();
			break;

		case Save_USS:
			m_vButtons[m_selected_button]->color = DirectX::XMFLOAT4( 1, 1, 1, 1 );
			SaveUpgrades();
			break;

		case Exit_USS:
			g_AudioSystem->PlaySound( "MENU_Back" );
			m_vButtons[m_selected_button]->color = DirectX::XMFLOAT4( 1, 1, 1, 1 );
			ExitState();
			break;

		default:
			break;
	}

	UpdatePoints();
	int points = HowManyPoints();
	UpCostDisplay->SetID( m_UpgradeCost + std::to_string( points ) );
	UpCostDisplay->m_renderInfo = nullptr;
}

bool UpgradeShipState::EnoughPoints( const int &upgrade_val )
{
	if ( upgrade_val + 1 <= starting_points )
	{
		return true;
	}

	return false;
}

int UpgradeShipState::CalculatePointsReturned( const int &upgrade_val )
{
	// Put this in a function in case this changes later
	return upgrade_val;
}

void UpgradeShipState::AdjustPoints( const int &upgrade_val )
{
	starting_points -= (upgrade_val + 1);
}

void UpgradeShipState::ReturnPoints( const int &upgrade_val )
{
	starting_points += upgrade_val;
}

// Upgrade functions for Whip Weapon
void UpgradeShipState::UpgradeWhipDmg()
{
	if ( EnoughPoints( whip_dmg ) && whip_dmg < gameData->m_Whip.dmg_upgrade_max )
	{
		AdjustPoints( whip_dmg );
		++whip_dmg;
		WhipDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( whip_dmg ) );
		WhipDmgDisplay->m_renderInfo = nullptr;
	}

	/*if ( starting_points > 0 && whip_dmg < gameData->m_Whip.dmg_upgrade_max )
	{
	++whip_dmg;
	--starting_points;
	WhipDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( whip_dmg ) );
	WhipDmgDisplay->m_renderInfo = nullptr;
	}
	else*/
	return;
}

void UpgradeShipState::DowngradeWhipDmg()
{
	if ( whip_dmg > gameData->m_Whip.dmg_upgrade_num )
	{
		ReturnPoints( whip_dmg );
		--whip_dmg;
		WhipDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( whip_dmg ) );
		WhipDmgDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}

void UpgradeShipState::UpgradeWhipSpd()
{
	if ( EnoughPoints( whip_spd ) && whip_spd < gameData->m_Whip.fire_rate_upgrade_max )
	{
		AdjustPoints( whip_spd );
		++whip_spd;
		WhipSpdDisplay->SetID( m_DisplaySpdUpgrade + std::to_string( whip_spd ) );
		WhipSpdDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}

void UpgradeShipState::DowngradeWhipSpd()
{
	if ( whip_spd > gameData->m_Whip.fire_rate_upgrade_num )
	{
		ReturnPoints( whip_spd );
		--whip_spd;
		WhipSpdDisplay->SetID( m_DisplaySpdUpgrade + std::to_string( whip_spd ) );
		WhipSpdDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}



// Upgrade functions for Spread Gun
void UpgradeShipState::UpgradeSpreadDmg()
{
	if ( EnoughPoints( spread_dmg ) && spread_dmg < gameData->m_Spread.dmg_upgrade_max )
	{
		AdjustPoints( spread_dmg );
		++spread_dmg;
		SpreadDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( spread_dmg ) );
		SpreadDmgDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}

void UpgradeShipState::DowngradeSpreadDmg()
{
	if ( spread_dmg > gameData->m_Spread.dmg_upgrade_num )
	{
		ReturnPoints( spread_dmg );
		--spread_dmg;
		SpreadDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( spread_dmg ) );
		SpreadDmgDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}

void UpgradeShipState::UpgradeSpreadNum()
{
	if ( EnoughPoints( spread_num ) && spread_num < gameData->m_Spread.fire_rate_upgrade_max )
	{
		AdjustPoints( spread_num );
		++spread_num;
		SpreadNumDisplay->SetID( m_DisplayNumUpgrade + std::to_string( spread_num ) );
		SpreadNumDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}

void UpgradeShipState::DowngradeSpreadNum()
{
	if ( spread_num > gameData->m_Spread.fire_rate_upgrade_num )
	{
		ReturnPoints( spread_num );
		--spread_num;
		SpreadNumDisplay->SetID( m_DisplayNumUpgrade + std::to_string( spread_num ) );
		SpreadNumDisplay->m_renderInfo = nullptr;

	}
	else
		return;
}


void UpgradeShipState::UpgradeMissileDmg()
{
	if ( EnoughPoints( missile_dmg ) && missile_dmg < gameData->m_Missile.dmg_upgrade_max )
	{
		AdjustPoints( missile_dmg );
		++missile_dmg;
		MissileDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( missile_dmg ) );
		MissileDmgDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}

void UpgradeShipState::DowngradeMissileDmg()
{
	if ( missile_dmg > gameData->m_Missile.dmg_upgrade_num )
	{
		ReturnPoints( missile_dmg );
		--missile_dmg;
		MissileDmgDisplay->SetID( m_DisplayDmgUpgrade + std::to_string( missile_dmg ) );
		MissileDmgDisplay->m_renderInfo = nullptr;
	}
	else
		return;
}

void UpgradeShipState::UpgradeMissileNum()
{
	if ( EnoughPoints( missile_num ) && missile_num < gameData->m_Missile.fire_rate_upgrade_max )
	{
		AdjustPoints( missile_num );
		++missile_num;
		MissileNumDisplay->SetID( m_DisplayNumUpgrade + std::to_string( missile_num ) );
		MissileNumDisplay->m_renderInfo = nullptr;

	}
	else
		return;
}

void UpgradeShipState::DowngradeMissileNum()
{
	if ( missile_num > gameData->m_Missile.fire_rate_upgrade_num )
	{
		ReturnPoints( missile_num );
		--missile_num;
		MissileNumDisplay->SetID( m_DisplayNumUpgrade + std::to_string( missile_num ) );
		MissileNumDisplay->m_renderInfo = nullptr;

	}
	else
		return;
}


void UpgradeShipState::SaveUpgrades()
{
	gameData->m_Whip.dmg_upgrade_num = whip_dmg;
	gameData->m_Whip.fire_rate_upgrade_num = whip_spd;

	gameData->m_Spread.dmg_upgrade_num = spread_dmg;
	gameData->m_Spread.fire_rate_upgrade_num = spread_num;

	gameData->m_Missile.dmg_upgrade_num = missile_dmg;
	gameData->m_Missile.fire_rate_upgrade_num = missile_num;

	gameData->m_upgrade_points = starting_points;

	gameData->m_Spread.UpdateMembers();
	gameData->m_Whip.UpdateMembers();

	m_NextState = GAME_PLAY_STATE;
}

// Disabling the exit button at this time
void UpgradeShipState::ExitState()
{
	m_NextState = MAIN_MENU_STATE;
	//ResetUpgrades();
}


bool UpgradeShipState::Handle360Input()
{
	/////////////////////////////////////////////////////////////
	// TODO: Setup states code to handle hovering over buttons //

	GamePad tmpPad = gamePad->GetState();
	bool detectedInput = false;

	// Right (?)
	if ( tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bPress ||
		 tmpPad.stickDir[0][stickDirections::sRight] == buttonStatus::bHeld )
	{
		ChangeButton( m_selected_button + 1 );
		//g_AudioSystem->PlaySound( "MENU_Hover" );
		detectedInput = true;
	}

	// Left
	if ( tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bPress ||
		 tmpPad.stickDir[0][stickDirections::sLeft] == buttonStatus::bHeld )
	{
		ChangeButton( m_selected_button - 1 );
		//g_AudioSystem->PlaySound( "MENU_Hover" );
		detectedInput = true;
	}

	// Up
	if ( tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bPress ||
		 tmpPad.stickDir[0][stickDirections::sUp] == buttonStatus::bHeld )
	{
		ChangeButton( m_selected_button - 4 );
		//g_AudioSystem->PlaySound( "MENU_Hover" );
		detectedInput = true;
	}

	// Down
	if ( tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bPress ||
		 tmpPad.stickDir[0][stickDirections::sDown] == buttonStatus::bHeld )
	{
		ChangeButton( m_selected_button + 4 );
		//g_AudioSystem->PlaySound( "MENU_Hover" );
		detectedInput = true;
	}

	// Select button (A)
	if ( tmpPad.buttons[buttonList::A] == buttonStatus::bPress )
	{
		//g_AudioSystem->PlaySound( "MENU_Accept" );
		// DoButtonworkThing
		DoWorkFunction( m_selected_button );
		detectedInput = true;
	}

	// Start Button pressed: Save and Exit
	if ( tmpPad.buttons[buttonList::START] == buttonStatus::bPress )
	{
		SaveUpgrades();
		detectedInput = true;
	}

	return detectedInput;

}

// Highlight Button
void UpgradeShipState::HilightButton( int button )
{
	// Hilight new button
	if ( button == Save_USS )	// resumeGameButton
	{
		// Select new one
		m_vButtons[button]->SetID( "resumeGameButtonSelected" );
	}

	else if ( button == Exit_USS ) // backToMainMenu
	{
		// Unselect old one
		//m_vButtons[m_selected_button]->SetID("");

		// Select new one
		m_vButtons[button]->SetID( "backToMainMenuButtonSelected" );
	}

	else // Arrows
	{
		// Light up new (incoming) button
		m_vButtons[button]->color = DirectX::XMFLOAT4( 0.0f, 1.5f, 0.0f, 1 );
	}


	// De-select old button
	if ( m_selected_button == Save_USS )
	{
		m_vButtons[m_selected_button]->SetID( "resumeGameButton" );
	}

	else if ( m_selected_button == Exit_USS )
	{
		m_vButtons[m_selected_button]->SetID( "backToMainMenuButton" );
	}

	else // Arrows
	{
		// Unlight old button
		m_vButtons[m_selected_button]->color = DirectX::XMFLOAT4( 1, 1, 1, 1 );
	}

	g_AudioSystem->PlaySound( "MENU_Hover" );
}

void UpgradeShipState::ChangeButton( int newButton )
{
	if ( newButton < 0 )
		return;

	if ( newButton >= MAX_UPGRADE_BUTTONS )
	{
		HilightButton( Save_USS );
		m_selected_button = Save_USS;
		return;
	}


	if ( newButton % 4 >= 0 || newButton == Save_USS )
	{
		if ( (newButton >= 0) && (newButton < MAX_UPGRADE_BUTTONS) )
		{
			HilightButton( newButton );
			m_selected_button = (UPGRADE_BUTTONS)newButton;

			int points = HowManyPoints();
			UpCostDisplay->SetID( m_UpgradeCost + std::to_string( points ) );
			UpCostDisplay->m_renderInfo = nullptr;

			return;
		}
	}

	if ( m_selected_button >= Save_USS && newButton < MAX_UPGRADE_BUTTONS )
	{
		HilightButton( SpreadDmgDown );
		m_selected_button = SpreadDmgDown;

		int points = HowManyPoints();
		UpCostDisplay->SetID( m_UpgradeCost + std::to_string( points ) );
		UpCostDisplay->m_renderInfo = nullptr;
	}



	//HilightButton(newButton);
	//m_selected_button = (UPGRADE_BUTTONS)newButton;

}

bool UpgradeShipState::CheckMouseMove()
{
	using namespace Events;

	// This will give us access to current mouse position
	// MessageSystem::GetInstance()->Call<void, float&, float&>( "GetMousePos", fMouseX, fMouseY );

	RECT clientRect;

	if ( !globalGraphicsPointer )
		return false;

	float x, y;
	x = fMouseX;
	y = fMouseY;

	GetClientRect( globalGraphicsPointer->GetWindow(), &clientRect );

	// Percentile measurement of mouse position (this is how we place 2d images on screen)
	x = x / (clientRect.right - clientRect.left);
	y = y / (clientRect.bottom - clientRect.top);


	// Check all the buttons for mouse hovering over it
	for ( unsigned int i = 0; i < MAX_UPGRADE_BUTTONS; i++ )
	{
		// The button exists, so check it
		if ( m_vButtons[i] != nullptr )
		{
			if ( i == m_selected_button )
				continue;

			if ( CheckButton( i, x, y ) )
			{
				HilightButton( i );
				m_selected_button = (UPGRADE_BUTTONS)i;

				int points = HowManyPoints();
				UpCostDisplay->SetID( m_UpgradeCost + std::to_string( points ) );
				UpCostDisplay->m_renderInfo = nullptr;

				return true;
			}
		}
	}

	return false;
}

void DummyFunction()
{

}