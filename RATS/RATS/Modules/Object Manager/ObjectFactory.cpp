#include "stdafx.h"
#include "ObjectFactory.h"
#include "../Components/HealthComponent.h"
#include "../Components/InputComponent.h"
#include "../Components//GlowComponent.h"
#include "../AI/AICompBase.h"
#include "../Renderer/RenderComp.h"
#include "../Renderer/D3DGraphics.h"
#include "../Collision/ColliderComp.h"
//#include "../Components/Movement/GroundClampComponent.h"
//#include "../Components/Movement/TerrainFollowComponent.h"
#include "../Components/Movement/GroundFollowComponent.h"
#include "../AI/AIManager.h"
#include "GameObject.h"

#include <typeinfo>

// Defines for estimated maximums for each component type
#define NUM_COLLISION_COMPONENTS 1200
#define NUM_2D_RENDER_COMPONENTS 800
#define NUM_MOVEMENT_COMPONENTS 1200
#define NUM_HEALTH_COMPONENTS 1200
#define NUM_RENDER_COMPONENTS 1800
#define NUM_GLOW_COMPONENTS 1200
#define NUM_INPUT_COMPONENTS 5
#define NUM_AI_COMPONENTS 1200
#define NUM_TRANSPARENT 5
#define WORLD_BUCKET 0
#define DUMMY_BUCKET 0
#define BADBUCKETS 1
#define NUM_MENU_COMPONENTS 50
#define NUM_GLOW_HASH 1750

extern ThreeSixty* gamePad;
extern D3DGraphics* globalGraphicsPointer;
// "Health Bar" for 2D test

ObjectFactory::ObjectFactory()
{
	m_RenderBuckets.resize( eRENDERCOMP_TYPE::MAX_RenderComp );
	m_RenderSizes.resize( eRENDERCOMP_TYPE::MAX_RenderComp );

	m_RenderBuckets[Opaque].resize( eOBJECT_TYPE::MAX_obj );
	m_RenderSizes[Opaque].resize( eOBJECT_TYPE::MAX_obj );

	for ( unsigned int i = 0; i < MAX_obj; i++ )
	{
		if ( i >= Arrow_Sigma && i <= Arrow_Omega )
			m_RenderBuckets[Opaque][i].resize( 1 );
		else
			m_RenderBuckets[Opaque][i].resize( NUM_RENDER_COMPONENTS );
		m_RenderSizes[Opaque][i] = 0;
	}

	m_RenderBuckets[Transparent].resize( BADBUCKETS );
	m_RenderBuckets[Transparent][DUMMY_BUCKET].resize( NUM_TRANSPARENT );
	m_RenderSizes[Transparent].resize( BADBUCKETS );
	m_RenderSizes[Transparent][DUMMY_BUCKET] = 0;

	m_RenderBuckets[Render2D_Back].resize( BADBUCKETS );
	m_RenderBuckets[Render2D_Back][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[Render2D_Back].resize( BADBUCKETS );
	m_RenderSizes[Render2D_Back][DUMMY_BUCKET] = 0;

	m_RenderBuckets[Render2D_Middle].resize( BADBUCKETS );
	m_RenderBuckets[Render2D_Middle][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[Render2D_Middle].resize( BADBUCKETS );
	m_RenderSizes[Render2D_Middle][DUMMY_BUCKET] = 0;

	m_RenderBuckets[Render2D_Middle2].resize( BADBUCKETS );
	m_RenderBuckets[Render2D_Middle2][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[Render2D_Middle2].resize( BADBUCKETS );
	m_RenderSizes[Render2D_Middle2][DUMMY_BUCKET] = 0;

	m_RenderBuckets[Render2D_Middle3].resize( BADBUCKETS );
	m_RenderBuckets[Render2D_Middle3][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[Render2D_Middle3].resize( BADBUCKETS );
	m_RenderSizes[Render2D_Middle3][DUMMY_BUCKET] = 0;

	m_RenderBuckets[Render2D_Front].resize( BADBUCKETS );
	m_RenderBuckets[Render2D_Front][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[Render2D_Front].resize( BADBUCKETS );
	m_RenderSizes[Render2D_Front][DUMMY_BUCKET] = 0;

	m_RenderBuckets[Render2D_Text].resize( BADBUCKETS );
	m_RenderBuckets[Render2D_Text][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[Render2D_Text].resize( BADBUCKETS );
	m_RenderSizes[Render2D_Text][DUMMY_BUCKET] = 0;

	m_RenderBuckets[RenderLightning].resize( BADBUCKETS );
	m_RenderBuckets[RenderLightning][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[RenderLightning].resize( BADBUCKETS );
	m_RenderSizes[RenderLightning][DUMMY_BUCKET] = 0;

	m_RenderBuckets[RenderParticle].resize( BADBUCKETS );
	m_RenderBuckets[RenderParticle][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[RenderParticle].resize( BADBUCKETS );
	m_RenderSizes[RenderParticle][DUMMY_BUCKET] = 0;

	m_RenderBuckets[RenderAnimTex].resize( BADBUCKETS );
	m_RenderBuckets[RenderAnimTex][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[RenderAnimTex].resize( BADBUCKETS );
	m_RenderSizes[RenderAnimTex][DUMMY_BUCKET] = 0;

	m_RenderBuckets[RenderRibbon].resize( BADBUCKETS );
	m_RenderBuckets[RenderRibbon][DUMMY_BUCKET].resize( NUM_RENDER_COMPONENTS );
	m_RenderSizes[RenderRibbon].resize( BADBUCKETS );
	m_RenderSizes[RenderRibbon][DUMMY_BUCKET] = 0;

}



ObjectFactory::~ObjectFactory()
{
	// Health Cleanup
	auto it = m_HealthDead.begin();
	while ( it != m_HealthDead.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_HealthLive.begin();
	while ( it != m_HealthLive.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	// Input Cleanup
	it = m_InputDead.begin();
	while ( it != m_InputDead.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_InputLive.begin();
	while ( it != m_InputLive.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}


	for ( unsigned int render = 0; render < m_RenderBuckets.size(); render++ )
	{
		for ( unsigned int object = 0; object < m_RenderBuckets[render].size(); object++ )
		{
			for ( unsigned int makeitso = 0; makeitso < m_RenderBuckets[render][object].size(); makeitso++ )
			{
				delete m_RenderBuckets[render][object][makeitso];
				//m_RenderBuckets[render][object][makeitso]->SetType(eCOMPONENT_TYPE::Render);

			}
		}
	}

	it = m_MoveDead.begin();
	while ( it != m_MoveDead.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_MoveLive.begin();
	while ( it != m_MoveLive.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_BVDead.begin();
	while ( it != m_BVDead.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_BVLive.begin();
	while ( it != m_BVLive.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_AIDead.begin();
	while ( it != m_AIDead.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_AILive.begin();
	while ( it != m_AILive.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	for ( unsigned int i = 0; i < m_GlowLive.size(); i++ )
	{
		for ( unsigned int j = 0; j < NUM_GLOW_COMPONENTS; j++ )
		{
			delete m_GlowLive[i][j];
		}
	}

}


bool ObjectFactory::Register( int Comp_Type, int Sub_Type, void( *MakeMe )() )
// Needs the Root component type, and the sub type for accessing the vector holding register values
// Registration needs to happen immediately- could happen in Factory's constructor for purposes of figuring it out
{

	return false;
}

void ObjectFactory::SetAIManager( AIManager *aim )
{
	m_AIManager = aim;
}

void ObjectFactory::Initialize()
{
	for ( unsigned int render = 0; render < m_RenderBuckets.size(); render++ )
	{
		for ( unsigned int object = 0; object < m_RenderBuckets[render].size(); object++ )
		{
			for ( unsigned int makeitso = 0; makeitso < m_RenderBuckets[render][object].size(); makeitso++ )
			{
				m_RenderBuckets[render][object][makeitso] = new RenderComp();
				m_RenderBuckets[render][object][makeitso]->SetType( eCOMPONENT_TYPE::Render );

			}
		}
	}

	//return true;
}

bool ObjectFactory::InitializeGamePlay()
{

	// Movement Component list
	m_MoveDead.resize( NUM_MOVEMENT_COMPONENTS );

	auto it = m_MoveDead.begin();
	while ( it != m_MoveDead.end() )
	{
		(*it) = new GroundFollowComponent();
		(*it)->SetType( eCOMPONENT_TYPE::Movement );
		it++;
	}


	// Health Component list
	m_HealthDead.resize( NUM_HEALTH_COMPONENTS );
	it = m_HealthDead.begin();
	while ( it != m_HealthDead.end() )
	{
		(*it) = new HealthComponent();
		(*it)->SetType( eCOMPONENT_TYPE::Health );
		it++;
	}

	// Input Component list
	m_InputDead.resize( NUM_INPUT_COMPONENTS );
	it = m_InputDead.begin();
	while ( it != m_InputDead.end() )
	{
		(*it) = new InputComponent();
		(*it)->SetType( eCOMPONENT_TYPE::Input );
		it++;
	}

	// Already sized in Constructor
	for ( unsigned int render = 0; render < m_RenderBuckets.size(); render++ )
	{
		for ( unsigned int object = 0; object < m_RenderBuckets[render].size(); object++ )
		{
			for ( unsigned int makeitso = 0; makeitso < m_RenderBuckets[render][object].size(); makeitso++ )
			{
				m_RenderBuckets[render][object][makeitso] = new RenderComp();
				m_RenderBuckets[render][object][makeitso]->SetType( eCOMPONENT_TYPE::Render );
			}
		}
	}

	// AI Component(s) list(s)
	m_AIDead.resize( NUM_AI_COMPONENTS );
	it = m_AIDead.begin();
	while ( it != m_AIDead.end() )
	{
		(*it) = new AICompBase();
		(*it)->SetType( eCOMPONENT_TYPE::AI );
		it++;
	}


	// Collision Comp List
	m_BVDead.resize( NUM_COLLISION_COMPONENTS );
	it = m_BVDead.begin();
	while ( it != m_BVDead.end() )
	{
		(*it) = new ColliderComp();
		(*it)->SetType( eCOMPONENT_TYPE::Collision );
		it++;
	}

	m_GlowLive.resize( NUM_GLOW_HASH );	// Hash size according to Mark
	m_GlowSizes.resize( NUM_GLOW_HASH );
	for ( unsigned int i = 0; i < m_GlowLive.size(); i++ )
	{
		m_GlowLive[i].resize( NUM_GLOW_COMPONENTS );
		m_GlowSizes[i] = 0;
		for ( unsigned int j = 0; j < NUM_GLOW_COMPONENTS; j++ )
		{
			m_GlowLive[i][j] = new GlowComponent();
			m_GlowLive[i][j]->SetType( eCOMPONENT_TYPE::Glow );
		}
	}

	return true;
}


//bool ObjectFactory::CreateObject( GameObject** newGameObject, int objectType )
//{
//	return false;
//}

void ObjectFactory::RecycleComponents( std::list<GameObject*>::iterator &gameObject, bool keepRender )
{

	// Return components to dead list
	std::list<BaseComponent*> &objList = (*gameObject)->GetComponents();

	/*if( (*gameObject)->GetType() == InnerStar)
	{
	std::cout << "What's going on here?\n";
	}*/

	auto it = objList.begin();
	while ( objList.size() > 0 )
	{
		//int type = (*it)->GetType();

		RecycleComponent( gameObject, (*it)->GetType() );
		it = objList.begin();
	}

	/*if ( !keepRender )
	{
	RecycleComponent( gameObject, Render );
	}*/


	return;

}

bool ObjectFactory::AddGlowEffect( GameObject *obj, std::string glowID )
{
	// Paranoia
	if ( obj == nullptr )
	{
		std::cout << "Sent GlowEffect a null object pointer\n";
		return false;	// Seriously, don't do that
	}

	int bucket = globalGraphicsPointer->HashString( glowID );
	if ( m_GlowSizes[bucket] < NUM_GLOW_COMPONENTS )
	{
		BaseComponent *toAdd;
		toAdd = m_GlowLive[bucket][m_GlowSizes[bucket]];
		toAdd->SetOwner( obj );
		reinterpret_cast<GlowComponent*>(toAdd)->SetGlowID( glowID );
		m_GlowSizes[bucket]++;

		obj->GetComponents().push_back( toAdd );
		return true;
	}

	std::cout << "Glow Effect says we have more than 800 effects of " << glowID << "\n";
	return false;
}

bool ObjectFactory::RemoveGlowEffect( BaseComponent *obj, int gtype )
{
	int index = FindGlowIndex(obj , gtype );
	if ( index == -1 ) { std::cout << "Glow comp not found\n"; return false; }	// Ugly error checking
	
	m_GlowSizes[gtype]--;
	BaseComponent *live;
	live = m_GlowLive[gtype][m_GlowSizes[gtype]];
	m_GlowLive[gtype][m_GlowSizes[gtype]] = m_GlowLive[gtype][index];
	reinterpret_cast<GlowComponent*>(m_GlowLive[gtype][index])->Reset();

	m_GlowLive[gtype][index] = live;
	return true;
}

bool ObjectFactory::FindIterator( BaseComponent* findme, std::list<BaseComponent*> &list, std::list<BaseComponent*>::iterator &store )
{
	std::list<BaseComponent*>::iterator it = list.begin();

	while ( it != list.end() )
	{
		if ( (*it) == findme )
		{
			store = it;
			return true;
		}
		it++;
	}

	return false;
}

// Beautiful Switch Statement
bool ObjectFactory::InitializeObject( GameObject* gameObject, int objectType, std::string render_ID, eRENDERCOMP_TYPE render_type )
{

	//std::string what = "FFFragles";
	switch ( objectType )
	{

		case Audio_OneShot:
		case Audio_LoopHelp:
			return IntializeDummyOneShot( gameObject, objectType, render_ID );
			break;

			// Enemies first
			// Enemies likely have enough differences for individual methods ( maybe just a quick internal helper function)
		case Enemy_Merc:
		case Enemy_Bomber:
		case Enemy_Koi:
		case Enemy_Disruptor:
		case Enemy_MineLayer:
		case Enemy_Dodger:
		case Enemy_FenceLayer:
		case Enemy_Goliath:
		case Enemy_Boss:
			return InitializeEnemy( gameObject, objectType, render_ID );
			break;

		case Bullet_Simple:
		case Bullet_Spread:
		case EvilBullet_Simple:
		case EvilBullet_Disrupt:
			return IntializeProjectile( gameObject, objectType, render_ID );
			break;

		case EvilBullet_HeatSeek:
		case EvilBullet_Mine:
		case Bullet_Whip:
		case Bullet_Missile:
		case EvilBullet_FencePost:
			return IntializeSmartProjectile( gameObject, objectType, render_ID );
			break;

		case Env_Prominence_Bullet:
			return InitializeAOEProjectile( gameObject, objectType, render_ID );
			break;

		case Pickup_Health:
		case Pickup_Shield:
			return InitializePowerUp( gameObject, objectType, render_ID );
			break;

		case VFX_Lightning:
			return InitializeVFXLightning( gameObject, objectType, render_ID );
			break;

		case VFX_Particle:
			return InitializeVFXParticle( gameObject, objectType, render_ID );
			break;

		case VFX_AnimTex:
			return InitializeVFXAnimTex( gameObject, objectType, render_ID );
			break;

		case VFX_Ribbon:
			return InitializeVFXPRibbon( gameObject, objectType, render_ID );
			break;

		case Orb_Blue:
			return InitializeOrb( gameObject, objectType, render_ID );
			break;

		case Pylon_Test:
			return InitializePylon( gameObject, objectType, render_ID );
			break;

		case Pylon_Lambda:
		case Pylon_Sigma:
		case Pylon_Omega:
			return InitializePyonParts( gameObject, objectType, render_ID );
			break;
		case Env_Prominence:
		case Env_Tornado:
		case Env_Singularity:
			return InitializeEnvironmentalHazard( gameObject, objectType, render_ID );
			break;

		case Arrow_Sigma:
		case Arrow_Lambda:
		case Arrow_Omega:
			return InitializeArrows( gameObject, objectType, render_ID );
			break;

		case Camera_Obj:
			return true;
			break;

			// Hmmm
		case HUD_2D:
			return InitializeRender2D( gameObject, objectType, render_ID, render_type );
			break;

		case Landing_Indicator:
			return InitializeLandingZone(gameObject, objectType, render_ID);
			break;

		case Player_Obj:
			return InitializePlayer( gameObject, objectType, render_ID );
			break;

		case World_Obj:
			return InitializeWorld( gameObject, objectType, render_ID );
			break;

		case Distort_Obj_A:
		case Distort_Obj_B:
		case Distort_Obj_C:
		case Distort_Obj_D:
			return InitializeDistortable( gameObject, objectType, render_ID );
			break;

		case InnerStar:
			return InitializeInnerStar( gameObject, objectType, render_ID );
			break;

		case Scenery_Obj:
			return InitializeScenery( gameObject, objectType, render_ID );
			break;

		default:
			// Something went wrong (you forgot to add the case for the Init call)
			return false;
	}

	// Something else went wrong, like seriously wrong (you need to return the Init Function (boolean value))
	return false;
}

bool ObjectFactory::InitializeVFXLightning( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{

		toAdd = AddRenderComponent( eRENDERCOMP_TYPE::RenderLightning, objectType );
		toAdd->SetOwner( gameObject );
		//std::string name = "Lightning";
		reinterpret_cast<RenderComp*>(toAdd)->SetID( "Lightning" );
		objList.push_back( toAdd );
	}
	else return false;

	return true;
}

// Ribbon Effects, in case render component needs special settings
bool ObjectFactory::InitializeVFXPRibbon( GameObject* gameObject, int objectType, std::string render_ID )
{
	//std::list<BaseComponent*> &objList = gameObject->GetComponents();
	//gameObject->SetType( (eOBJECT_TYPE)objectType );
	//BaseComponent *toAdd;

	//// Render Component
	//if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	//{
	//	toAdd = AddRenderComponent( eRENDERCOMP_TYPE::RenderRibbon, objectType );
	//	reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
	//	toAdd->SetOwner( gameObject );
	//	objList.push_back( toAdd );
	//}
	//else return false;

	return true;
}

bool ObjectFactory::InitializeArrows( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( eRENDERCOMP_TYPE::Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );
	}
	else return false;

	// AI Component
	/*if ( !m_AIDead.empty() )
	{
	m_AILive.splice( m_AILive.begin(), m_AIDead, m_AIDead.begin() );
	toAdd = (*m_AILive.begin());
	toAdd->SetOwner( gameObject );
	m_AIManager->RegisterMinion( (AICompBase*)toAdd );
	objList.push_back( toAdd );
	}
	else return false;*/

	return true;
}

bool ObjectFactory::InitializeWorld( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Transparent][WORLD_BUCKET] < NUM_TRANSPARENT )
	{
		toAdd = AddRenderComponent( Transparent, WORLD_BUCKET );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );

		objList.push_back( toAdd );

	}
	else return false;

	return true;
}

bool ObjectFactory::InitializeDistortable( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_TRANSPARENT )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );

		objList.push_back( toAdd );
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializeInnerStar( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );

		objList.push_back( toAdd );

	}
	else return false;

	return true;
}

// Isn't needed as the code stands right now; may re-vitalize in later refactoring
bool ObjectFactory::InitializeCamera( GameObject *gameObject, int objectType, std::string render_ID )
{
	//std::list<BaseComponent*> &objList = gameObject->GetComponents();
	//gameObject->SetType( (eOBJECT_TYPE)objectType );
	//BaseComponent *toAdd;

	//// Render Component
	//if ( !m_RenderDead.empty() )
	//{
	//	//m_RenderLive.splice(m_RenderLive.begin(), m_RenderDead, m_RenderDead.begin());
	//	m_LiveRenderBuckets[eRENDERCOMP_TYPE::Transparent][objectType].splice(
	//		m_LiveRenderBuckets[eRENDERCOMP_TYPE::Transparent][objectType].begin(),
	//		m_RenderDead, m_RenderDead.begin() );

	//	//toAdd = (*m_RenderLive.begin());
	//	toAdd = (*m_LiveRenderBuckets[eRENDERCOMP_TYPE::Transparent][objectType].begin());
	//	toAdd->SetOwner( gameObject );
	//	std::string name;
	//	name = "Camera";
	//	reinterpret_cast<RenderComp*>(toAdd)->SetID(name);
	//	objList.push_back( toAdd );

	//}
	//else return false;

	//return true;

	return false;
}


bool ObjectFactory::IntializeProjectile( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	num_Bullets++;

	// Health Component
	if ( !m_HealthDead.empty() )
	{
		m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
		toAdd = (*m_HealthLive.begin());
		toAdd->SetOwner( gameObject );

		if (objectType == Bullet_Spread)
		{
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth(1.5f);

		}
		else
		{
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth(3.2f);

		}

		reinterpret_cast<HealthComponent*>(toAdd)->SetTimer( true );
		objList.push_back( toAdd );
	}
	else return false;

	// Move Component
	if ( !m_MoveDead.empty() )
	{
		m_MoveLive.splice( m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin() );
		toAdd = (*m_MoveLive.begin());
		(*m_MoveLive.begin())->SetOwner( gameObject );
		objList.push_back( toAdd );
	}
	else return false;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );

		if ( objectType != Bullet_Spread )
		{
			//reinterpret_cast<RenderComp*>(toAdd)->SetGlowID(render_ID + " Glow");

			if ( AddGlowEffect( gameObject, render_ID + " Glow" ) )
			{

			}
			else return false;

		}
		objList.push_back( toAdd );

	}
	else return false;

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		objList.push_back( toAdd );
		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 1 );
		(*m_BVLive.begin())->SetOwner( gameObject );
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializePowerUp( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		if ( objectType == Pickup_Shield )
		{
			reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
			//reinterpret_cast<RenderComp*>(toAdd)->SetGlowID(render_ID + " Glow");

			if ( AddGlowEffect( gameObject, render_ID + " Glow" ) )
			{

			}
			else return false;
		}
		else if ( objectType == Pickup_Health )
		{
			reinterpret_cast<RenderComp*>(toAdd)->SetID( "Invisible" );
			//reinterpret_cast<RenderComp*>(toAdd)->SetGlowID(render_ID + " Glow");

			if ( AddGlowEffect( gameObject, render_ID + " Glow" ) )
			{

			}
			else return false;

		}
		objList.push_back( toAdd );
	}
	else return false;

	if ( !m_HealthDead.empty() )
	{
		m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
		toAdd = (*m_HealthLive.begin());
		toAdd->SetOwner( gameObject );
		reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 12.5f );
		reinterpret_cast<HealthComponent*>(toAdd)->SetTimer( true );
		objList.push_back( toAdd );
	}
	else return false;

	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		toAdd->SetOwner( gameObject );
		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 2.5f );
		reinterpret_cast<ColliderComp*>(toAdd)->SetAggroRadius( 12 );

		objList.push_back( toAdd );
	}
	else return false;

	// AI Component
	if (!m_AIDead.empty())
	{
		m_AILive.splice(m_AILive.begin(), m_AIDead, m_AIDead.begin());
		toAdd = (*m_AILive.begin());
		toAdd->SetOwner(gameObject);
		m_AIManager->RegisterMinion((AICompBase*)toAdd);
		objList.push_back(toAdd);
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializePlayer( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	gameObject->SetVelocityInWorldSpace( false );

	// Move Component
	if ( !m_MoveDead.empty() )
	{
		m_MoveLive.splice( m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin() );
		toAdd = (*m_MoveLive.begin());
		objList.push_back( toAdd );
		(*m_MoveLive.begin())->SetOwner( gameObject );
	}
	else return false;

	// Health Component
	if ( !m_HealthDead.empty() )
	{
		m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
		toAdd = (*m_HealthLive.begin());
		reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 150 );
		reinterpret_cast<HealthComponent*>(toAdd)->SetMaxEnergy(100);
		//reinterpret_cast<HealthComponent*>(toAdd)->SetEnergy(0);
		objList.push_back( toAdd );
		(*m_HealthLive.begin())->SetOwner( gameObject );
	}
	else return false;

	// Input Component
	if ( !m_InputDead.empty() )
	{
		m_InputLive.splice( m_InputLive.begin(), m_InputDead, m_InputDead.begin() );
		toAdd = (*m_InputLive.begin());
		objList.push_back( toAdd );
		(*m_InputLive.begin())->SetOwner( gameObject );
	}
	else return false;

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		objList.push_back( toAdd );
		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 4 );
		(*m_BVLive.begin())->SetOwner( gameObject );
	}
	else return false;


	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{

		toAdd = AddRenderComponent( Opaque, objectType );

		/*m_LiveRenderBuckets[eRENDERCOMP_TYPE::Opaque][objectType].splice(
			m_LiveRenderBuckets[eRENDERCOMP_TYPE::Opaque][objectType].begin(),
			m_RenderDead, m_RenderDead.begin() );

			toAdd = (*m_LiveRenderBuckets[eRENDERCOMP_TYPE::Opaque][objectType].begin());*/

		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );
	}
	else return false;


	return true;

}

bool ObjectFactory::InitializeEnemy( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Movement Component
	if ( !m_MoveDead.empty() )
	{
		m_MoveLive.splice( m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin() );
		toAdd = (*m_MoveLive.begin());
		objList.push_back( toAdd );
		(*m_MoveLive.begin())->SetOwner( gameObject );
	}
	else return false;

	// Health Component
	if ( !m_HealthDead.empty() )
	{
		m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
		toAdd = (*m_HealthLive.begin());
		toAdd->SetOwner( gameObject );
		if ( objectType == eOBJECT_TYPE::Enemy_Bomber )
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 10 );
		else if ( objectType == eOBJECT_TYPE::Enemy_Disruptor ||
				  objectType == Enemy_Dodger ||
				  objectType == Enemy_Goliath )
				  reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 45 );
		else if ( objectType == eOBJECT_TYPE::Enemy_MineLayer )
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 75 );
		else if ( objectType == eOBJECT_TYPE::Enemy_Boss )
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 2400 );
		else
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 48 );

		objList.push_back( toAdd );
	}
	else return false;


	// AI Component
	if ( !m_AIDead.empty() )
	{
		m_AILive.splice( m_AILive.begin(), m_AIDead, m_AIDead.begin() );
		toAdd = (*m_AILive.begin());
		toAdd->SetOwner( gameObject );
		m_AIManager->RegisterMinion( (AICompBase*)toAdd );
		objList.push_back( toAdd );
	}
	else return false;

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		objList.push_back( toAdd );

		if ( objectType == eOBJECT_TYPE::Enemy_Boss /*|| objectType == eOBJECT_TYPE::Enemy_Disruptor*/ )
			reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 15 );
		else
			reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 4 );

		if ( objectType == Enemy_Dodger )
			reinterpret_cast<ColliderComp*>(toAdd)->SetAggroRadius( 15 );

		(*m_BVLive.begin())->SetOwner( gameObject );
	}
	else return false;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );	// Nyar
	}
	else return false;

	if (objectType != Enemy_Boss)
	{
		if (AddGlowEffect(gameObject, render_ID + " Glow"))
		{

		}
		else return false;
	}
	

	// Glow Component
	/*int bucket = globalGraphicsPointer->HashString(render_ID + " Glow");
	if ( m_GlowSizes[bucket] < NUM_GLOW_COMPONENTS )
	{
	toAdd = m_GlowLive[bucket][m_GlowSizes[bucket]];
	reinterpret_cast<GlowComponent*>(toAdd)->SetGlowID(render_ID + " Glow");
	m_GlowSizes[bucket]++;
	objList.push_back(toAdd);

	}
	else return false;*/

	return true;
}

bool ObjectFactory::InitializePylon( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Health Component
	if ( !m_HealthDead.empty() )
	{
		m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
		toAdd = (*m_HealthLive.begin());
		toAdd->SetOwner( gameObject );
		reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 300 );
		reinterpret_cast<HealthComponent*>(toAdd)->SetHealth( 150 );
		reinterpret_cast<HealthComponent*>(toAdd)->SetMaxEnergy( 300 );
		objList.push_back( toAdd );
	}
	else return false;

	// 	Render Component
	// 		if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	// 		{
	// 			toAdd = AddRenderComponent(Opaque, objectType);
	// 			toAdd->SetOwner( gameObject );
	// 			reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
	// 			objList.push_back( toAdd );
	// 		}
	// 		else return false;

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		toAdd->SetOwner( gameObject );
		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 6 );
		objList.push_back( toAdd );
	}
	else return false;

	//std::cout << "Pylon Created" << std::endl;
	//gameObject->GetTransform().RotateBy(2, 90, true);

	return true;
}

bool ObjectFactory::InitializeVFXAnimTex( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[RenderAnimTex][DUMMY_BUCKET] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( RenderAnimTex, DUMMY_BUCKET );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializeVFXParticle( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[RenderParticle][DUMMY_BUCKET] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( RenderParticle, DUMMY_BUCKET );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );
	}
	else return false;

	// Movement Component
	if ( !m_MoveDead.empty() )
	{
		m_MoveLive.splice( m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin() );
		toAdd = (*m_MoveLive.begin());
		objList.push_back( toAdd );
		(*m_MoveLive.begin())->SetOwner( gameObject );
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializeRender2D( GameObject* gameObject, int objectType, std::string render_ID, eRENDERCOMP_TYPE render_type )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[render_type][DUMMY_BUCKET] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( render_type, DUMMY_BUCKET );
		toAdd->SetOwner( gameObject );
		objList.push_back( toAdd );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializeOrb( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		//reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( "Invisible" );
		//reinterpret_cast<RenderComp*>(toAdd)->SetGlowID(render_ID + " Glow");

		if ( AddGlowEffect( gameObject, render_ID + " Glow" ) )
		{

		}
		else return false;
		objList.push_back( toAdd );
	}
	else return false;

	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		toAdd->SetOwner( gameObject );
		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 2.5f );
		reinterpret_cast<ColliderComp*>(toAdd)->SetAggroRadius( 12 );

		objList.push_back( toAdd );
	}

	if ( !m_HealthDead.empty() )
	{
		m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
		toAdd = (*m_HealthLive.begin());
		toAdd->SetOwner( gameObject );
		reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 12.5f );
		reinterpret_cast<HealthComponent*>(toAdd)->SetTimer( true );
		objList.push_back( toAdd );
	}
	else return false;

	// AI Component
	if (!m_AIDead.empty())
	{
		m_AILive.splice(m_AILive.begin(), m_AIDead, m_AIDead.begin());
		toAdd = (*m_AILive.begin());
		toAdd->SetOwner(gameObject);
		m_AIManager->RegisterMinion((AICompBase*)toAdd);
		objList.push_back(toAdd);
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializeEnvironmentalHazard( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	if ( objectType != eOBJECT_TYPE::Env_Tornado && objectType != Env_Prominence )
	{
		// Render Component
		if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
		{
			toAdd = AddRenderComponent( Opaque, objectType );
			toAdd->SetOwner( gameObject );
			reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
			objList.push_back( toAdd );
		}
		else return false;
	}


	// Movement Component
	if ( !m_MoveDead.empty() )
	{
		m_MoveLive.splice( m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin() );
		toAdd = (*m_MoveLive.begin());
		objList.push_back( toAdd );
		(*m_MoveLive.begin())->SetOwner( gameObject );
	}
	else return false;

	// AI Component
	if ( !m_AIDead.empty() )
	{
		m_AILive.splice( m_AILive.begin(), m_AIDead, m_AIDead.begin() );
		toAdd = (*m_AILive.begin());
		toAdd->SetOwner( gameObject );
		m_AIManager->RegisterMinion( (AICompBase*)toAdd );
		objList.push_back( toAdd );
	}
	else return false;

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		objList.push_back( toAdd );

		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 5 );
		if ( objectType == eOBJECT_TYPE::Env_Singularity )
		{
			reinterpret_cast<ColliderComp*>(toAdd)->SetAggroRadius( 30 );

		}

		(*m_BVLive.begin())->SetOwner( gameObject );
	}
	else return false;

	return true;
}

//std::vector<std::vector<std::list<BaseComponent*>>> *ObjectFactory::GetRenderBuckets()
//{
//	//return &m_LiveRenderBuckets;
//	return nullptr;
//}

std::vector<std::vector<std::vector<BaseComponent*>>> *ObjectFactory::GetRenderBuckets()
{
	return &m_RenderBuckets;
}


// Helper Functions

void ObjectFactory::RecycleComponent( std::list<GameObject*>::iterator &gameObject, int compType )
{
	// Return components to dead list
	std::list<BaseComponent*> &objList = (*gameObject)->GetComponents();
	std::list<BaseComponent*>::iterator toSplice;
	auto it = objList.begin();

	while ( (*it)->GetType() != compType )
		it++;

	if ( compType != Render ) // render is a special case
	{
		if ( compType == Movement )
		{
			if ( FindIterator( (*it), m_MoveLive, toSplice ) )
			{
				m_MoveDead.splice( m_MoveDead.begin(), m_MoveLive, toSplice );
			}
		}

		else if ( compType == Health )
		{
			if ( FindIterator( (*it), m_HealthLive, toSplice ) )
			{
				reinterpret_cast<HealthComponent*>(*toSplice)->SetTimer( false );
				reinterpret_cast<HealthComponent*>(*toSplice)->SetHealth( 0.1f );
				reinterpret_cast<HealthComponent*>(*toSplice)->SetInvincibility( false );
				m_HealthDead.splice( m_HealthDead.begin(), m_HealthLive, toSplice );
			}
		}

		else if ( compType == Input )
		{
			if ( FindIterator( (*it), m_InputLive, toSplice ) )
			{
				reinterpret_cast<InputComponent*>((*toSplice))->ResetVals();
				m_InputDead.splice( m_InputDead.begin(), m_InputLive, toSplice );
			}
		}

		else if ( compType == AI )
		{
			if ( FindIterator( (*it), m_AILive, toSplice ) )
			{
				m_AIManager->KillMinion( reinterpret_cast<AICompBase*>((*it)) );
				m_AIDead.splice( m_AIDead.begin(), m_AILive, toSplice );
			}
		}

		else if ( compType == Collision )
		{
			if ( FindIterator( (*it), m_BVLive, toSplice ) )
			{
				m_BVDead.splice( m_BVDead.begin(), m_BVLive, toSplice );
			}
		}

		else if ( compType == Glow )
		{
			// Really should put a virtual Reset() in base component
			//reinterpret_cast<GlowComponent*>(*it)->Reset(); // this happens in Remove now
			
			// Returns true/false for success, can use this for error checking
			
			
			int gtype = globalGraphicsPointer->HashString( reinterpret_cast<GlowComponent*>(*it)->GetGlowID() );
			bool check = 
			RemoveGlowEffect(*it, gtype);

			/////////  Moved this code to RemoveGlowEffect()  /////////

			//int index = FindGlowIndex( (*it), gtype );
			//if ( index == -1 ) { std::cout << "Glow comp not found\n"; return; }	// Ugly error checking
			//m_GlowSizes[gtype]--;
			//BaseComponent *live;
			//live = m_GlowLive[gtype][m_GlowSizes[gtype]];
			//m_GlowLive[gtype][m_GlowSizes[gtype]] = m_GlowLive[gtype][index];
			//m_GlowLive[gtype][index] = live;
		}
	}

	// Render Component Handling
	else
	{

		int objType = (*gameObject)->GetType();

		int rtype;
		rtype = reinterpret_cast<RenderComp*>((*it))->GetRenderType();
		//(*it)->SetOwner(nullptr);

		if ( rtype != Opaque )
			objType = 0;

		//if(objType >= VFX_Lightning)
		reinterpret_cast<RenderComp*>(*it)->m_renderInfo = nullptr;


		// Swaps the newly dead item (pointer) with the last living item in the vector
		RenderComp *live;
		int index;
		index = FindRenderIndex( (*it), rtype, objType );

		if ( index == -1 )
		{
			std::cout << "Render comp not found\n";
			return;			// Find function returns -1 if somehow the thing wasn't in the list
		}

		m_RenderSizes[rtype][objType]--;
		live = (RenderComp*)(m_RenderBuckets[rtype][objType][m_RenderSizes[rtype][objType]]);
		m_RenderBuckets[rtype][objType][m_RenderSizes[rtype][objType]] = m_RenderBuckets[rtype][objType][index];
		m_RenderBuckets[rtype][objType][index] = live;
	}

	// Remove from ObjList
	(*it)->Reset();		// Should cleanup all data for each type of component, base comp data not included as of 4/10
	(*it)->SetOwner( nullptr );
	objList.remove( (*it) );

}

bool ObjectFactory::InitializePyonParts( GameObject* gameObject, int objectType, std::string render_ID )
{
	// I am not sure what he wanted with this function
	// Add more components

	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Move Component
	if ( !m_MoveDead.empty() )
	{
		m_MoveLive.splice( m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin() );
		toAdd = (*m_MoveLive.begin());
		objList.push_back( toAdd );
		(*m_MoveLive.begin())->SetOwner( gameObject );
	}
	else return false;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );
	}
	else return false;

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		toAdd->SetOwner( gameObject );
		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 5 );
		objList.push_back( toAdd );
	}
	else return false;

	return true;

}

int ObjectFactory::FindRenderIndex( BaseComponent *findme, int rendType, int objType )
{
	for ( int i = 0; i < m_RenderSizes[rendType][objType]; i++ )
	{
		if ( m_RenderBuckets[rendType][objType][i] == findme )
			return i;
	}

	return -1;		// Didn't find it (should never happen)
}

int ObjectFactory::FindGlowIndex( BaseComponent *findme, int glowType )
{
	for ( int i = 0; i < m_GlowSizes[glowType]; i++ )
	{
		if ( m_GlowLive[glowType][i] == findme )
			return i;
	}

	return -1;		// Didn't find it (should never happen)
}

void ObjectFactory::RenderInfoHelper()
{
	for ( unsigned int render = 0; render < m_RenderBuckets.size(); render++ )
	{
		for ( unsigned int object = 0; object < m_RenderBuckets[render].size(); object++ )
		{
			for ( unsigned int makeitso = 0; makeitso < m_RenderBuckets[render][object].size(); makeitso++ )
			{
				reinterpret_cast<RenderComp*>(m_RenderBuckets[render][object][makeitso])->m_renderInfo = nullptr;
			}
		}
	}
}


bool ObjectFactory::AddHealthComponent( GameObject* gameObject, int objType )
{
	//// Health Component
	//if ( !m_HealthDead.empty() )
	//{
	//	m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
	//	toAdd = (*m_HealthLive.begin());
	//	toAdd->SetOwner( gameObject );
	//	reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 2.5f );
	//	reinterpret_cast<HealthComponent*>(toAdd)->SetTimer( true );
	//	objList.push_back( toAdd );
	//}
	//else return false;


	return false;
}

// Returns Component pointer needing to be added to object
BaseComponent* ObjectFactory::AddRenderComponent( int rendType, int objType )
{
	// Increase size of Live items, return reference of component
	// Personal settings are set for this component in that function

	if ( rendType != Opaque )
		objType = 0;

	// RenderType, ObjType
	BaseComponent *r_val = nullptr;
	r_val = m_RenderBuckets[rendType][objType][m_RenderSizes[rendType][objType]];
	reinterpret_cast<RenderComp*>(r_val)->SetRenderType( rendType );
	reinterpret_cast<RenderComp*>(r_val)->color = XMFLOAT4( 1, 1, 1, 1 );
	m_RenderSizes[rendType][objType]++;

	return r_val;
}

bool ObjectFactory::InitializeLandingZone(GameObject *gameObject, int objectType, std::string render_ID)
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );		
		objList.push_back( toAdd );

	}
	else 
		return false;

	// Move Component
	if (!m_MoveDead.empty())
	{
		m_MoveLive.splice(m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin());
		toAdd = (*m_MoveLive.begin());
		(*m_MoveLive.begin())->SetOwner(gameObject);
		objList.push_back(toAdd);
	}
	else return false;

	// AI Component
	if (!m_AIDead.empty())
	{
		m_AILive.splice(m_AILive.begin(), m_AIDead, m_AIDead.begin());
		toAdd = (*m_AILive.begin());
		toAdd->SetOwner(gameObject);
		m_AIManager->RegisterMinion((AICompBase*)toAdd);
		objList.push_back(toAdd);
	}
	else 
		return false;

	return true;
}

bool ObjectFactory::IntializeSmartProjectile( GameObject* gameObject, int objectType, std::string render_ID /*= ""*/ )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;

	// Debug value no longer needed
	//num_Bullets++;

	// Health Component
	if ( !m_HealthDead.empty() )
	{
		m_HealthLive.splice( m_HealthLive.begin(), m_HealthDead, m_HealthDead.begin() );
		toAdd = (*m_HealthLive.begin());
		toAdd->SetOwner( gameObject );
		if ( objectType == eOBJECT_TYPE::EvilBullet_HeatSeek )
		{
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 8.0f );
			reinterpret_cast<HealthComponent*>(toAdd)->SetTimer( true );
		}

		else if ( objectType == Bullet_Whip )
		{
			reinterpret_cast<HealthComponent*>(toAdd)->SetTimer( true );
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 1.5f );
		}
		else if ( objectType == Bullet_Missile )
		{
			reinterpret_cast<HealthComponent*>(toAdd)->SetTimer( true );
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 2.5f );
		}
		else
			reinterpret_cast<HealthComponent*>(toAdd)->SetMaxHealth( 10 );
		//reinterpret_cast<HealthComponent*>(toAdd)->SetTimer(true);
		objList.push_back( toAdd );
	}
	else return false;

	// Move Component
	if ( !m_MoveDead.empty() )
	{
		m_MoveLive.splice( m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin() );
		toAdd = (*m_MoveLive.begin());
		(*m_MoveLive.begin())->SetOwner( gameObject );
		objList.push_back( toAdd );
	}
	else return false;

	//if (objectType != eOBJECT_TYPE::Bullet_Whip)
	//{
	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		// 		if(objectType != EvilBullet_Mine)
		//reinterpret_cast<RenderComp*>(toAdd)->SetGlowID(render_ID + " Glow");

		if (objectType != EvilBullet_Mine)
		{
			if (AddGlowEffect(gameObject, render_ID + " Glow"))
			{

			}
			else return false;
		}
		
		objList.push_back( toAdd );

	}
	else return false;

	//}

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		objList.push_back( toAdd );
		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 2 );
		reinterpret_cast<ColliderComp*>(toAdd)->SetAggroRadius( 15 );
		(*m_BVLive.begin())->SetOwner( gameObject );
	}
	else return false;


	// AI Component
	if ( !m_AIDead.empty() )
	{
		m_AILive.splice( m_AILive.begin(), m_AIDead, m_AIDead.begin() );
		toAdd = (*m_AILive.begin());
		toAdd->SetOwner( gameObject );
		m_AIManager->RegisterMinion( (AICompBase*)toAdd );
		objList.push_back( toAdd );
	}
	else return false;


	return true;
}

bool ObjectFactory::InitializeAOEProjectile( GameObject* gameObject, int objectType, std::string render_ID )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;
	//// Move Component
	//if (!m_MoveDead.empty())
	//{
	//	m_MoveLive.splice(m_MoveLive.begin(), m_MoveDead, m_MoveDead.begin());
	//	toAdd = (*m_MoveLive.begin());
	//	reinterpret_cast<GroundFollowComponent*>(toAdd)->SetSoft(10.0f);
	//	objList.push_back(toAdd);
	//	(*m_MoveLive.begin())->SetOwner(gameObject);
	//}
	//else return false;

	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );
	}
	else return false;

	// AI Component
	if ( !m_AIDead.empty() )
	{
		m_AILive.splice( m_AILive.begin(), m_AIDead, m_AIDead.begin() );
		toAdd = (*m_AILive.begin());
		toAdd->SetOwner( gameObject );
		m_AIManager->RegisterMinion( (AICompBase*)toAdd );
		objList.push_back( toAdd );
	}
	else return false;

	// Collision Component
	if ( !m_BVDead.empty() )
	{
		m_BVLive.splice( m_BVLive.begin(), m_BVDead, m_BVDead.begin() );
		toAdd = (*m_BVLive.begin());
		objList.push_back( toAdd );

		reinterpret_cast<ColliderComp*>(toAdd)->SetRadius( 5 );
		reinterpret_cast<ColliderComp*>(toAdd)->SetAggroRadius( 7 );
		(*m_BVLive.begin())->SetOwner( gameObject );
	}
	else return false;

	return true;
}

bool ObjectFactory::InitializeScenery( GameObject* gameObject, int objectType, std::string render_ID /*= ""*/ )
{
	std::list<BaseComponent*> &objList = gameObject->GetComponents();
	gameObject->SetType( (eOBJECT_TYPE)objectType );
	BaseComponent *toAdd;


	// Render Component
	if ( m_RenderSizes[Opaque][objectType] < NUM_RENDER_COMPONENTS )
	{
		toAdd = AddRenderComponent( Opaque, objectType );
		toAdd->SetOwner( gameObject );
		reinterpret_cast<RenderComp*>(toAdd)->SetID( render_ID );
		objList.push_back( toAdd );

	}
	else return false;

	return true;
}

bool ObjectFactory::IntializeDummyOneShot( GameObject* gameObject, int objectType, std::string render_ID /*= ""*/ )
{
	/*std::list<BaseComponent*> &objList = gameObject->GetComponents();*/
	gameObject->SetType( (eOBJECT_TYPE)objectType );

	return true;
}


