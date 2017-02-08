#include "stdafx.h"
#include "CreditsState.h"

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

#define SCROLL_SPEED 0.1f

CreditsState::CreditsState()
{
}


CreditsState::~CreditsState()
{
}

void CreditsState::Enter( BitmapFontManager* bitmapFontManager, bool resetSound )
{
	using namespace Events;

	if ( m_controller )
		m_controller->SetMenuKeys();


	// Key Useage Events
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Confirm" ), this, &CreditsState::OnEnter );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "Return" ), this, &CreditsState::OnEscape );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyPress", "MouseLClick" ), this, &CreditsState::OnMouseClick );

	m_pBitmapFontManager = bitmapFontManager;
	m_Renderer->SetRenderList( m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets() );
	m_Renderer->SetRenderSizes( m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes() );

	m_NextState = CREDITS_STATE;

	g_AudioSystem->StopBGM();
	g_AudioSystem->ResetListener();
	g_AudioSystem->PlayBGM("RATS_BGM_Credits");

	RenderNames();

	leaveTimer = 0;
}

void CreditsState::Exit()
{
	using namespace Events;

	EventManager()->UnregisterClient( "Confirm", this, &CreditsState::OnEnter );
	EventManager()->UnregisterClient( "Return", this, &CreditsState::OnEscape );
	EventManager()->UnregisterClient( "MouseLClick", this, &CreditsState::OnMouseClick );

	if(m_controller)
		m_controller->ClearMenuKeys();

	ClearNames();
}

void CreditsState::Initialize( Renderer* renderer )
{
	m_Renderer = renderer;

	m_ObjectManager.Initialize();
	m_ObjectFactory.Initialize();
	m_ObjectManager.SetFactory( &m_ObjectFactory );

	Names();
}

void CreditsState::Update( float dt )
{

	leaveTimer += dt;

	if (leaveTimer >= 89.0f)
	{
		leaveTimer = 0;
		m_NextState = MAIN_MENU_STATE;

	}


	ScrollText( dt );

	GamePad tmpPad = gamePad->GetState();

	// Select button (A)
	// Start Button pressed: Save and Exit
	if (tmpPad.buttons[buttonList::A] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::B] == buttonStatus::bPress ||
		tmpPad.buttons[buttonList::START] == buttonStatus::bPress)
	{
		m_NextState = MAIN_MENU_STATE;

	}

	// Probably loop through vector of comps, ScrollText will keep track/manipulate when strings are removed/added to the cycle
}

void CreditsState::Terminate()
{

}


//////////////////////////////////////////////////////////////////////
///////////////////   METHODS START HERE	//////////////////////////
//////////////////////////////////////////////////////////////////////

void CreditsState::InitText( std::string text, RenderComp *comp )
{
	comp->SetID( text );
	comp->m_posX = 0.2f;
	comp->m_posY = 1.3f;
	comp->m_width = 0.1f;
	comp->m_height = 0.2f;
}

void CreditsState::ScrollText( float dt )
{

	float move_length = SCROLL_SPEED;

	move_length *= dt;

	// Scroll all active things, probably inside of that list I made
	for ( unsigned int i = 0; i < m_NameRender.size(); i++ )
	{
		m_NameRender[i]->m_posY -= move_length;
	}
}

void CreditsState::OnEnter( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	m_NextState = MAIN_MENU_STATE;

}

void CreditsState::OnEscape( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	m_NextState = MAIN_MENU_STATE;
}

void CreditsState::OnMouseClick( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{

}




void CreditsState::Names()
{
	m_NameList.push_back( "      Development  Team" );
	m_NameList.push_back( "Yvonne  Neuland" );
	m_NameList.push_back( "Grant  Taylor" );
	m_NameList.push_back( "Mark  A.  Miller" );
	m_NameList.push_back( "A.  Genaro  Garcia" );
	m_NameList.push_back( "Devin  Aldrich" );
	m_NameList.push_back( "" );

	m_NameList.push_back("       Production Staff  ");
	m_NameList.push_back( "Rod  Moye");
	m_NameList.push_back( "Jason  Hinders");
	m_NameList.push_back( "Mike  Lebo");
	m_NameList.push_back( "Chris  Marks");
	m_NameList.push_back( "Don  Thomas");
	m_NameList.push_back( "Kris  Ducote");
	m_NameList.push_back( "Justin  Murphy");
	m_NameList.push_back("Carlos  Lugo");
	m_NameList.push_back("Patrick  Kelly");

	m_NameList.push_back( "Derrick  Kayden  Snodgrass" );
	m_NameList.push_back( "Mike  Davis" );
	m_NameList.push_back( "Jim  Merrell" );
	m_NameList.push_back( "Samuel  Kiykendall" );
	m_NameList.push_back( "Alan  Alexis  Roman" );
	m_NameList.push_back( "Yuri  Cantrell");
	m_NameList.push_back( "Ryan  Simmons");
	m_NameList.push_back( "Matt  Schulz");
	m_NameList.push_back( "Mohammad  Ghonaim");
	m_NameList.push_back( "James Zoutis");		///Need his name
	m_NameList.push_back( "" );

	m_NameList.push_back( "      Artists   ");
	m_NameList.push_back( "Sierra  Mayo               2D Art");
	m_NameList.push_back( "Eric  Broe                 3D Art");
	m_NameList.push_back( "Michael  Barita            3D Art");
	m_NameList.push_back( "Gustavo  Reyes             3D Art");
	m_NameList.push_back( "Eddie  Cooper              3D Art");
	m_NameList.push_back( "Austen  Seebrg             3D Art");
	m_NameList.push_back( "Alexander  Higgins         3D Art");
	m_NameList.push_back( "Richard  Sun               Video Editing");
	m_NameList.push_back( "");

	m_NameList.push_back( "      Audio Crew");
	m_NameList.push_back( "Vincent  Indiano");
	m_NameList.push_back( "Wes  Luttrell");
	m_NameList.push_back( "Leonardo  Manrique");
	m_NameList.push_back( "Shawn  Carpenter");
	m_NameList.push_back( "Nick  Pilotta");
	m_NameList.push_back( "Daniel  Rosado") ;
	m_NameList.push_back( "");

	m_NameList.push_back( "      Additional Assets Provided By");
	m_NameList.push_back( "Game-icons.net");
	m_NameList.push_back( "sbed");
	m_NameList.push_back( "PriorBlue");
	m_NameList.push_back( "Lorc");
	m_NameList.push_back( "Lord  Berandas");
	m_NameList.push_back( "Delapouite");


	m_NameList.push_back( "");
	m_NameList.push_back("Special thanks to all the");
	m_NameList.push_back("testers who tested our game!");
	m_NameList.push_back( "");

	m_NameList.push_back("");
	m_NameList.push_back("Special thanks to friends and family");
	m_NameList.push_back("for their love and support!");
	m_NameList.push_back("");

	m_NameList.push_back("And of course,");
	m_NameList.push_back("thank you, stalwart player.");

}

/*
Audio Crew

Vincent Indiano
Wes Luttrell
Leonardo Manrique
Mohammad Ghonaim
Shawn Carpenter
Nick Pilotta
Daniel Rosado


Mike Davis Producer
Jim Merrell Producer
Samuel Kuykendall Producer
Alan Alexis Roman Producer
Yuri Cantrell Producer
Ryan Simmons Producer
Matt Schulz Producer
Mohammad Ghonaim Producer
Sierra Mayo 2D Artist 
Eric Broe 3D Artist
Michael Barita 3D Artist
Gustavo Reyes 3D Artist
Eddie Cooper 3D Artist
Austen Seeberg 3D Artist
Alexander Higgins 3D Artist
Richard Sun Video Editing
*/


void CreditsState::ClearNames()
{
	m_NameRender.clear();
	m_ObjectManager.ResetAll();
}

void CreditsState::RenderNames()
{
	m_NameRender.resize( m_NameList.size() );
	float y = 1.0f;
	for ( unsigned int i = 0; i < m_NameList.size(); i++ )
	{
		GameObject* temp = m_ObjectManager.CreateObject(HUD_2D, float3( 0, 0, 0 ), m_NameList[i], Render2D_Text);

		m_NameRender[i] = temp->GetComponent<RenderComp>();
		m_NameRender[i]->m_posX = 0.15f;
		m_NameRender[i]->m_posY = y + (0.125f * i);
		m_NameRender[i]->m_width = 0.2f;
		m_NameRender[i]->m_height = 0.3f;

	}
}