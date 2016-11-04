#include "stdafx.h"
#include "ObjectManager.h"
#include "../../Wwise files/Transform.h"
#include "../Input/InputManager.h"
#include "../Input/MessageSystem.h"
#include "../../Wwise files/EventManager.h"
#include "../VFX/ElectricityEffect/ElectricityEffect.h"
#include "../VFX/AnimTexture/AnimTexture.h"
#include "ObjectFactory.h"
#include "../AI/AIManager.h"
#include "CameraObject.h"
#include "GameObject.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../Components/InputComponent.h"
#include "../Upgrade System/GameData.h"
#include "../VFX/RibbonEffect/RibbonEffect.h"
#include "../VFX/MeshDistorter/MeshDistorter.h"

#include "../Molecules/MoleculeManager.h"

#include "../Components/Movement/GroundFollowComponent.h"



#define NUM_DISTORT 5
#define NUM_MAX_OBJECTS 2600
#define NUM_MAX_VFX 200

#define NUM_LESSER_MAX_OBJECTS 500

#define CAM_OFFSET (gameData->m_levelShape == "Torus" ? 130.0f : 190.0f)

#include "../Renderer/D3DGraphics.h"
extern D3DGraphics* globalGraphicsPointer;
extern 	FFmod* g_AudioSystem;
extern ThreeSixty* gamePad;
extern GameData* gameData;

ObjectManager::ObjectManager()
{

	m_pFactory = nullptr;
	m_pAIManager = nullptr;
	test = true;
	m_pCamera = nullptr;
	m_fShotTimer = 0;
	m_fCamOffset = 120.0f;
	//RegisterClients();
}

ObjectManager::~ObjectManager()
{
	// Cleanup any memory in live list(s)
	for ( unsigned int i = 0; i < m_ObjectBuckets.size(); i++ )
	{
		// Get list of Objects from each bucket
		auto it = m_ObjectBuckets[i].begin();

		// Iterate list and delete objects
		while ( it != m_ObjectBuckets[i].end() )
		{
			//if ((*it)->GetType() == VFX_Particle)
			//	reinterpret_cast<ParticleSystem*>(*it)->Shutdown();
			if ( (*it)->GetType() == VFX_Ribbon )
				reinterpret_cast<RibbonEffect*>(*it)->ShutDown();
			delete (*it);
			(*it) = nullptr;
			it++;
		}
	}

	// Clean up memory in dead list(s)
	auto it = m_DeadList.begin();
	while ( it != m_DeadList.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	//it = m_ParticleVFX.begin();
	//while (it != m_ParticleVFX.end())
	//{
	//	((ParticleSystem*)(*it))->Shutdown();
	//	delete (*it);
	//	(*it) = nullptr;
	//	it++;
	//}

	it = m_LightningVFX.begin();
	while ( it != m_LightningVFX.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_AnimTexVFX.begin();
	while ( it != m_AnimTexVFX.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_RibbonVFX.begin();
	while ( it != m_RibbonVFX.end() )
	{
		reinterpret_cast<RibbonEffect*>(*it)->ShutDown();
		delete reinterpret_cast<RibbonEffect*>(*it);
		(*it) = nullptr;
		it++;
	}

	it = m_DistortionVFX.begin();
	while ( it != m_DistortionVFX.end() )
	{
		// THis is called in destructor, therefore don't need to do it here
		//reinterpret_cast<MeshDistorter*>(*it)->MeshDistorter::Shutdown();
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_DyingObjects.begin();
	while ( it != m_DyingObjects.end() )
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	//if(m_pPartExplode != nullptr)
	//	delete m_pPartExplode;

	if ( m_Molecules != nullptr )
		delete m_Molecules;
}

void ObjectManager::ResetAll()
{
	// Clear active items from live buckets
	for ( unsigned int i = 1; i < eOBJECT_TYPE::MAX_obj; i++ )
	{
		// Get list of Objects from each bucket
		auto it = m_ObjectBuckets[i].begin();

		if ( i == Camera_Obj )
		{
			m_pCamera->GetTransform().ResetTransform();
			(reinterpret_cast<CameraObject*>(m_pCamera))->GrabMoveSnapSpeed() = 2;
			(reinterpret_cast<CameraObject*>(m_pCamera))->GrabOffset() = DirectX::XMFLOAT3( 0, m_fCamOffset, 0 );
			continue;
		}

		// Iterate list and "delete" objects
		while ( it != m_ObjectBuckets[i].end() )
		{
			auto remove = it;
			it++;
			DestroyObject( i, remove );
		}
	}

	// VFX not getting cleaned up, oops

	

	// If VFX components ever get added to the Dying list, refactor KillTheDying
	auto it = m_DyingObjects.begin();
	while ( it != m_DyingObjects.end() )
	{
		auto remove = it;
		it++;
		KillTheDying( remove );
	}

	// Now reset the Molecule Manager
	m_Molecules->ResetEmitters();

	test = true;
}

void ObjectManager::Update( float dt )
{
	//std::list<GameObject*> todelete;

	unsigned int temp_size = 0;
	//if(m_pFactory)
	//temp_size = m_pFactory->m_HealthDead.size();



	for ( unsigned int i = 1; i < eOBJECT_TYPE::MAX_obj; i++ )
	{
		// Get list of Objects from each bucket
		//std::list<GameObject*>::reverse_iterator it;
		auto it = m_ObjectBuckets[i].begin();

		// Iterate list and update objects
		while ( it != m_ObjectBuckets[i].end() )
		{

			// If this object has expired
			unsigned char flags = (*it)->GetStatusFlags();
			if ( flags & MostlyDead || flags & AllDead )
			{
				auto remove = it;
				it++;
				//todelete.splice(todelete.begin(), m_ObjectBuckets[i], remove);
				DestroyObject( (*remove)->GetType(), remove );
			}

			else
			{
				(*it)->Update( dt );
				it++;
			}
		}
	}

	// Iterate Dying List and recycle things there
	auto it = m_DyingObjects.begin();
	while ( it != m_DyingObjects.end() )
	{
		(*it)->DeathUpdate( dt );


		if ( (*it)->GetFlag( AllDead ) )
		{
			auto remove = it;
			it++;
			DestroyObject( (*remove)->GetType(), remove );
		}
		else
			it++;
	}

	m_CollisionManager.CheckCollisions( &m_ObjectBuckets, dt );

	if ( test )
		TestCreateThings();

	if ( m_Molecules != nullptr )
		m_Molecules->Update( dt );

	m_fShotTimer += dt;
	Shoot360();

}

void ObjectManager::TestCreateThings()
{
	// Test creating things in here
	if ( test )
	{
		test = false;

		//m_Molecules->BuildPosEmitter(XMFLOAT3(0, 0, 0), 100, "Inner Star Effect");


		/*MoleculeEmitter *fireboom = m_Molecules->BuildPosEmitter(XMFLOAT3(70,70,70), 5, "Health Pickup");
		fireboom->loop = true;
		m_Molecules->BuildPosEmitter(XMFLOAT3(70,70,70), 50, "Health Pickup Aura");

		fireboom = m_Molecules->BuildPosEmitter(XMFLOAT3(70,70,79), 5, "Energy Pickup");
		fireboom->loop = true;
		m_Molecules->BuildPosEmitter(XMFLOAT3(70,70,79), 50, "Energy Pickup Aura");*/

		//MoleculeEmitter *blah = m_Molecules->BuildEmitter(GetPlayer(), 100, "Pulse");

		/*CreateObject(1, XMFLOAT3(1.0f, 1.0f, 85.0f));
		unsigned int blah = NumEnemiesInBuckets();
		blah = blah;*/
		//ProminenceMolecule

		//GameObject* Prominence = CreateObject(eOBJECT_TYPE::Env_Prominence, XMFLOAT3(1.0f, 1.0f, 80.0f), "Prominence");
		//MoleculeEmitter *change = m_Molecules->BuildPosEmitter(XMFLOAT3(1.0f, 1.0f, 80.0f), 200, "ProminenceMolecule");

		//GameObject* ProminenceBullet = CreateObject(eOBJECT_TYPE::Env_Prominence_Bullet, XMFLOAT3(1.0f, 1.0f, 85.0f), "ProminenceBullet");

		//tornado->GetTransform().RotateBy(0, 180, 1.0f, true);

		//GameObject* singularity = CreateObject(eOBJECT_TYPE::Env_Singularity, XMFLOAT3(1.0f, 1.0f, 80.0f), "Singularity");


		//MoleculeEmitter *single =
		//m_Molecules->BuildEmitter(singularity, 250, "Singularity Effect");
		//single->m_targetPosition = *singularity->GetTransform().GetPosition();*/
		//single->m_targetPosition = float3(0,0,0);
	}

}

GameObject* ObjectManager::CreateObject( int objType, DirectX::XMFLOAT3 pos, std::string render_ID, eRENDERCOMP_TYPE rendType )
// Returns true if object was created successfully, false if there was a problem
{
	// Factory was not initialized
	if ( m_pFactory == nullptr )
	{
		return nullptr;
	}

	if ( render_ID == "" )
		render_ID = m_vRenderIDs[objType];

	std::list<GameObject*>::iterator it;

	// Some special case scenarios
	if ( objType == VFX_Lightning )
	{
		it = m_LightningVFX.begin();
		m_pFactory->InitializeObject( (*it), objType, render_ID );

		// Splice from dead to live
		m_ObjectBuckets[objType].splice( m_ObjectBuckets[objType].begin(), m_LightningVFX, it );
		(*it)->GetTransform().SetPosition( pos );
		//(*it)->GetTransform().SetLocalMatrix();
		(*it)->SetType( VFX_Lightning );
		return (*it);
	}

	else if ( objType == VFX_AnimTex )
	{
		it = m_AnimTexVFX.begin();
		reinterpret_cast<AnimTexture*>(*it)->Reset();
		m_pFactory->InitializeObject( *it, objType, render_ID );
		m_ObjectBuckets[objType].splice( m_ObjectBuckets[objType].begin(), m_AnimTexVFX, it );
		(*it)->GetTransform().SetPosition( pos );
		reinterpret_cast<AnimTexture*>((*it))->SetCamera( m_pCamera );
		//std::cout << "Created a Texture \n";
		(*it)->SetType( VFX_AnimTex );
		return (*it);
	}

	else if ( objType == VFX_Ribbon )
	{
		it = m_RibbonVFX.begin();
		//reinterpret_cast<ParticleSystem*>(*it)->
		m_pFactory->InitializeObject( *it, objType, render_ID );
		reinterpret_cast<RibbonEffect*>(*it)->ResetRibbon();
		m_ObjectBuckets[objType].splice( m_ObjectBuckets[objType].begin(), m_RibbonVFX, it );
		(*it)->GetTransform().SetPosition( pos );
		(*it)->SetType( VFX_Ribbon );
		return (*it);
	}

	//else if( objType == VFX_Particle)
	//{
	//	it = m_ParticleVFX.begin();
	//	m_pFactory->InitializeObject( *it, objType, render_ID);
	//	m_ObjectBuckets[objType].splice( m_ObjectBuckets[objType].begin(), m_ParticleVFX, it);
	//	(*it)->GetTransform().SetPosition(pos);
	//	(*it)->SetType(VFX_Particle);
	//	return (*it);
	//}

	else if ( objType == VFX_Distortion )
	{
		int index;
		Mesh* mesh;
		mesh = globalGraphicsPointer->GetDistortionMesh( index, gameData->m_levelShape );
		it = m_DistortionVFX.begin();
		// No factory init needed here!
		m_ObjectBuckets[Distort_Obj_A + index].splice( m_ObjectBuckets[Distort_Obj_A + index].begin(), m_DistortionVFX, it );
		(*it)->GetTransform().SetPosition( pos );
		index += (int)Distort_Obj_A;
		(*it)->SetType( (eOBJECT_TYPE)index );
		render_ID = m_vRenderIDs[(eOBJECT_TYPE)index] + gameData->m_levelShape;
		m_pFactory->InitializeObject( (*it), index, render_ID, rendType );
		reinterpret_cast<MeshDistorter*>((*it))->Reset( mesh );
		return (*it);
	}

	// Get Object* from dead list
	else
		it = m_DeadList.begin();

	// If it crashes here, check it and DeadList.  If DeadList is size 0, we ran out of objects
	// This should not be able to happen in GamePlay, but in menus there are limited number of objects, and ObjectManager may not be updating
	// Which means SetDead() will not cleanup the object, as that happens in Update()
	// Updating ObjectManager in menus isn't a problem, it just normally isn't necessary.
	// Alternatively instead of SetDead(), use DestroyObject();  It's public still

	// Build with factory
	if ( m_pFactory->InitializeObject( (*it), objType, render_ID, rendType ) )
	{
		// Splice from dead to live
		m_ObjectBuckets[objType].splice( m_ObjectBuckets[objType].begin(), m_DeadList, it );
		if ( objType > 0 && objType < Player_Obj )	// Enemy
		{
			// Could be enemy things in here man!  Like spawn trails.
		}
		else
		{
			(*it)->GetTransform().SetPosition( pos );
		}

		(*it)->SetType( (eOBJECT_TYPE)objType );

		if ( objType == eOBJECT_TYPE::Player_Obj )
		{
			if ( (*m_ObjectBuckets[Camera_Obj].begin()) == nullptr )
				return nullptr;
			else
				reinterpret_cast<CameraObject*>(m_pCamera)->SetTarget( (*it) );

			m_pPlayer = (*it);
		}
	}

	// Recycles components that were added and returns null (often broken, debug printout to help detect that)
	else
	{
		std::cout<<"Factory Init Failure\n";
		m_pFactory->RecycleComponents(it);
		(*it)->Reset();

		return nullptr;
	}

	if ( objType == Player_Obj )
	{
		//GameObject* thing;
		//thing = CreateObject( VFX_Ribbon, DirectX::XMFLOAT3( 0, 0, 0 ), "Ribbon" );
		//reinterpret_cast<RibbonEffect*>(thing)->SetParent( (*it) );
		//(*it)->SetTrail( (RibbonEffect*)thing );

		m_Molecules->BuildEmitter((*it), 100, "Player Trail");


	}


	if ( objType == Orb_Blue )
	{
		m_Molecules->BuildEmitter( (*it), 5, "Energy Pickup" );
		m_Molecules->BuildEmitter( (*it), 50, "Energy Pickup Aura");
	}

	else if ( objType == Pickup_Health )
	{
		m_Molecules->BuildEmitter( (*it), 5, "Health Pickup" );
		m_Molecules->BuildEmitter( (*it), 50, "Health Pickup Aura");

	}

	if (objType == Enemy_Boss)
	{
		(*it)->GetComponent<GroundFollowComponent>()->GrabFineness() = 0.001f;
	}

	//	g_AudioSystem->RegisterEntity(&(*it)->GetTransform(), "");
	(*it)->SetFlags( Normal );
	return (*it);
}


void ObjectManager::DestroyObject( int objType, std::list<GameObject*>::iterator object_it )
{

	if ( objType < -10 )
		return;

	GameObject *object = (*object_it);
	//(*object)->ClearAllStatusFlags();
	object->SetVelocity( DirectX::XMFLOAT3( 0, 0, 0 ) );
	object->GetTransform().DirtyTransform();
	object->SetSteadyRotation( -1, 0 );
	object->ResetPointers();

	if ( objType == eOBJECT_TYPE::Bullet_Whip )
	{
		MoleculeEmitter *explosion = m_Molecules->BuildPosEmitter( *object->GetTransform().GetPosition(), 30, "Whip Bullet Death" );
	}
	else if ( objType == Bullet_Spread )
	{
		MoleculeEmitter* explosion = m_Molecules->BuildPosEmitter( *object->GetTransform().GetPosition(), 30, "Spread Bullet Death" );
	}
	else if ( objType == EvilBullet_Disrupt )
	{
		MoleculeEmitter* explosion = m_Molecules->BuildPosEmitter( *object->GetTransform().GetPosition(), 30, "Enemy Simple Bullet Death");

	}
	else if ( objType == Bullet_Missile )
	{
		MoleculeEmitter* explosion = m_Molecules->BuildPosEmitter( *object->GetTransform().GetPosition(), 150, "Missile Bullet Death" );

	}

	if ( objType == Player_Obj )
	{
		if ( (*object_it)->GetFlag( MostlyDead ) )
		{
			GoodbyeClients();
			PlayerDeath();
			m_pPlayer->ClearAllStatusFlags();

			// Splice to dying list
			m_DyingObjects.splice( m_DyingObjects.begin(), m_ObjectBuckets[Player_Obj], object_it );
			return;
		}

		else if ( m_pPlayer->GetFlag( AllDead ) )
		{
			// Do other things!
			gameData->m_bPlayerDead = true;
			KillTheDying( object_it );
			return;
		}
	}

	if ( objType == EvilBullet_HeatSeek || objType == Enemy_Disruptor )
	{
		if ( (*object_it)->GetChannel() )
		{
			(*object_it)->GetChannel()->stop();
		}

		//MoleculeEmitter* explosion = m_Molecules->BuildPosEmitter( *object->GetTransform().GetPosition(), 200, "Orange Fire Explosion" );
	}

	// If object is enemy type
	if ( objType > 0 && objType < Player_Obj )	// Object is enemy
	{

		if ( object->GetFlag( AllDead ) )
		{
			HandleEnemyDeath( object_it );
			return;
		}


		else if ( object->GetFlag( MostlyDead ) )
		{
			object->ClearAllStatusFlags();
			GameObject* tmpAudio = CreateObject( Audio_OneShot, *object->GetTransform().GetPosition() );
			g_AudioSystem->PlaySound( "RATS_EnemyDeath", tmpAudio );
			//m_pFactory->RecycleComponents(object_it, true);
			object->SetDeathTimer( 0.25f );
			m_DyingObjects.splice( m_DyingObjects.begin(), m_ObjectBuckets[objType], object_it );

			return;
		}
	}


	//g_AudioSystem->UnRegisterEntity(&(*object_it)->GetTransform());

	// Allow factory to take back it's components
	m_pFactory->RecycleComponents( object_it );

	// Clear all status flags, including death
	object->ClearAllStatusFlags();
	DirectX::XMFLOAT4X4 reset;

	object->GetTransform().ResetTransform();
	object->GetComponents().clear();

	if ( objType == VFX_Lightning )
	{
		// Splice into VFX list instead of normal list
		reinterpret_cast<ElectricityEffect*>(*object_it)->ResetElectricity();
		m_LightningVFX.splice( m_LightningVFX.begin(), m_ObjectBuckets[objType], object_it );
	}

	else if ( objType == VFX_AnimTex )
	{
		m_AnimTexVFX.splice( m_AnimTexVFX.begin(), m_ObjectBuckets[objType], object_it );
	}

	//else if(objType == VFX_Particle)
	//{
	//	m_ParticleVFX.splice(m_ParticleVFX.begin(), m_ObjectBuckets[objType], object_it);
	//}

	else if ( objType == VFX_Ribbon )
	{
		reinterpret_cast<RibbonEffect*>(*object_it)->ResetRibbon();
		m_RibbonVFX.splice( m_RibbonVFX.begin(), m_ObjectBuckets[objType], object_it );
	}

	else if ( objType >= Distort_Obj_A && objType <= Distort_Obj_D )
	{
		// Reset here
		//reinterpret_cast<>
		m_DistortionVFX.splice( m_DistortionVFX.begin(), m_ObjectBuckets[objType], object_it );
	}

	else
	{
		// Splice it back into normal dead list
		m_DeadList.splice( m_DeadList.begin(), m_ObjectBuckets[objType], object_it );
	}
}

GameObject* ObjectManager::GetPlayer()
{
	if ( m_ObjectBuckets[Player_Obj].empty() )
	{
		return nullptr;
	}

	return (*m_ObjectBuckets[Player_Obj].begin());
}

void ObjectManager::Initialize()
{
	// TODO: Initialize less memory for non gameplay states.
	m_ObjectBuckets.resize( eOBJECT_TYPE::MAX_obj );
	m_DeadList.resize( NUM_LESSER_MAX_OBJECTS );

	m_vRenderIDs.resize( eOBJECT_TYPE::MAX_obj );
	for ( int i = 0; i < eOBJECT_TYPE::MAX_obj; i++ )
	{
		m_vRenderIDs[i] = "";
	}
	EstablishRenderInfos();

	auto it = m_DeadList.begin();
	while ( it != m_DeadList.end() )
	{
		(*it) = new GameObject;
		DirectX::XMFLOAT4X4 FFFFF;
		DirectX::XMStoreFloat4x4( &FFFFF, DirectX::XMMatrixIdentity() );
		(*it)->GetTransform().SetLocalMatrix( FFFFF );
		it++;
	}

	m_pCamera = new CameraObject;
	m_ObjectBuckets[Camera_Obj].push_back( m_pCamera );
	//g_AudioSystem->RegisterListener(&m_pCamera->GetTransform(), "Camera");
	(reinterpret_cast<CameraObject*>(m_pCamera))->GrabMoveSnapSpeed() = 2;
	(reinterpret_cast<CameraObject*>(m_pCamera))->GrabOffset() = DirectX::XMFLOAT3( 0, 80, 0 );

	m_pFactory = nullptr;
	m_pPlayer = nullptr;
	m_pAIManager = nullptr;
	m_Molecules = nullptr;

	// Temp
	m_Molecules = new MoleculeManager();
}

void ObjectManager::InitializeGamePlay()
{
	m_ObjectBuckets.resize( eOBJECT_TYPE::MAX_obj );
	m_LightningVFX.resize( NUM_MAX_VFX );
	m_AnimTexVFX.resize( NUM_MAX_VFX );
	//m_ParticleVFX.resize(NUM_MAX_VFX);
	m_RibbonVFX.resize( NUM_MAX_VFX );
	m_DistortionVFX.resize( NUM_DISTORT );


	m_vRenderIDs.resize( eOBJECT_TYPE::MAX_obj );
	for ( int i = 0; i < eOBJECT_TYPE::MAX_obj; i++ )
	{
		m_vRenderIDs[i] = "";
	}
	EstablishRenderInfos();
	// Setup Dead list with valid memory
	m_DeadList.resize( NUM_MAX_OBJECTS );
	auto it = m_DeadList.begin();
	while ( it != m_DeadList.end() )
	{
		(*it) = new GameObject;
		DirectX::XMFLOAT4X4 FFFFF;
		DirectX::XMStoreFloat4x4( &FFFFF, DirectX::XMMatrixIdentity() );
		(*it)->GetTransform().SetLocalMatrix( FFFFF );
		(*it)->SetObjManager( this );
		it++;
	}

	it = m_LightningVFX.begin();
	while ( it != m_LightningVFX.end() )
	{
		(*it) = new ElectricityEffect();
		(*it)->SetObjManager( this );
		it++;
	}

	it = m_AnimTexVFX.begin();
	while ( it != m_AnimTexVFX.end() )
	{
		(*it) = new AnimTexture( 4, 4, 1 );
		(*it)->SetObjManager( this );
		it++;
	}


	//it = m_ParticleVFX.begin();
	//ParticleSystemDescriptor default;
	//while( it != m_ParticleVFX.end()) 
	//{
	//	(*it) = new ParticleSystem(default);
	//	(*it)->SetObjManager(this);
	//	it++;
	//}

	it = m_RibbonVFX.begin();
	while ( it != m_RibbonVFX.end() )
	{
		(*it) = new RibbonEffect();
		(*it)->SetObjManager( this );
		it++;
	}

	it = m_DistortionVFX.begin();
	while ( it != m_DistortionVFX.end() )
	{
		(*it) = new MeshDistorter();
		(*it)->SetObjManager( this );
		it++;
	}


	m_pCamera = new CameraObject;
	m_ObjectBuckets[Camera_Obj].push_back( m_pCamera );
	//g_AudioSystem->RegisterListener(&m_pCamera->GetTransform(), "Camera");
	(reinterpret_cast<CameraObject*>(m_pCamera))->GrabMoveSnapSpeed() = 2;
	(reinterpret_cast<CameraObject*>(m_pCamera))->GrabOffset() = DirectX::XMFLOAT3( 0, m_fCamOffset, 0 );
	m_pCamera->SetObjManager( this );
	m_pFactory = nullptr;
	m_pPlayer = nullptr;
	m_pAIManager = nullptr;
	m_Molecules = new MoleculeManager();
	m_CollisionManager.SetOBJMNGR( this );
}


// Spawn Orb, Update Score
void ObjectManager::HandleEnemyDeath( std::list<GameObject*>::iterator enemy )
{

	//GameObject* orb = CreateObject(Orb_Blue, DirectX::XMFLOAT3(25,45,0), "Cube");
	// Health
	// 	if ((*enemy)->GetType() == eOBJECT_TYPE::Enemy_Merc)
	// 	{

	if (!(*enemy)->DidBomberWin())
	{
		if (rand() % 3 == 0)
		{

			if (rand() % 3 != 0)
			{
				GameObject* health = CreateObject(Pickup_Health, *((*enemy)->GetTransform().GetPosition()));
				health->GetTransform().GetWorldMatrix();
			}
			else
			{
				GameObject* shield = CreateObject(Pickup_Shield, *((*enemy)->GetTransform().GetPosition()));
				shield->GetTransform().GetWorldMatrix();
			}


		}
		else
		{
			GameObject* orb = CreateObject(Orb_Blue, *((*enemy)->GetTransform().GetPosition()), "Energy");
			orb->GetTransform().GetWorldMatrix();
		}
	}
	

	// 	}
	// 	else
	// 	{
	// 		GameObject* orb = CreateObject(Orb_Blue, *((*enemy)->GetTransform().GetPosition()), "Energy");
	// 		orb->GetTransform().GetWorldMatrix();
	// 	}


	// TODO: Update Score now please
	gameData->KillScore( (*enemy)->GetType() );

	KillTheDying( enemy );
}

void ObjectManager::KillTheDying( std::list<GameObject*>::iterator dead )
{
	m_pFactory->RecycleComponents( dead );
	(*dead)->GetComponents().clear();

	(*dead)->ClearAllStatusFlags();
	(*dead)->GetTransform().ResetTransform();
	(*dead)->Reset();
	// Splice Dead object out of Dying List instead of Buckets, since that's where it was living now
	m_DeadList.splice( m_DeadList.begin(), m_DyingObjects, dead );
}


unsigned int ObjectManager::NumEnemiesInBuckets()
{
	// Enemies are 1 ~ Player_obj-1
	unsigned int num = 0;
	for(int i=1; i<Player_Obj; i++)
	{
		num += m_ObjectBuckets[i].size();
	}

	return num;
}


void ObjectManager::CreatePlayerBullet( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	bool tmpBool;	m_pWhipChannel->getPaused( &tmpBool );

	if (gameData->m_bUsingGamepad)
		return;

	//using namespace DirectX;
	Ship_Weapon* currGun = gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select];

	int currBulletType = currGun->bullet_type;//Bullet_Whip;


	globalGraphicsPointer->SetCrosshairShowing( true );
	float x, y;
	Events::MessageSystem::GetInstance()->Call<void, float&, float&>( "GetMousePos", x, y );
	//std::cout << x << " " << y << "\n";

	if ( m_fShotTimer < currGun->myFireRate )
		return;

	POINT mousePos;
	GetCursorPos( &mousePos );

	ScreenToClient( globalGraphicsPointer->GetWindow(), &mousePos );

	RECT clientRect;
	GetClientRect( globalGraphicsPointer->GetWindow(), &clientRect );

	float NDCx = (((2.0f * mousePos.x) / (clientRect.right - clientRect.left)) - 1) * 0.02f,
		NDCy = (-(((2.0f * mousePos.y) / (clientRect.bottom - clientRect.top)) - 1)) * 0.02f;

	XMFLOAT3 NDCHolder = XMFLOAT3( NDCx, NDCy, 0 );
	XMFLOAT3 start = XMFLOAT3( NDCx, NDCy, -1 ), end = XMFLOAT3( NDCx, NDCy, 1 );

	XMStoreFloat3( &start, globalGraphicsPointer->Unproject( XMLoadFloat3( &start ) ) );
	XMStoreFloat3( &end, globalGraphicsPointer->Unproject( XMLoadFloat3( &end ) ) );

	XMFLOAT3 intersect;
	int tridummy;

	m_pPlayer = (*m_ObjectBuckets[Player_Obj].begin());

	XMFLOAT3 firingDir;
	if ( CollisionManager::IntersectLine2Triangle( start, end, tridummy, &intersect ) )
		XMStoreFloat3( &firingDir, XMVector3Normalize( XMLoadFloat3( &intersect ) - XMLoadFloat3( m_pPlayer->GetTransform().GetPosition() ) ) );

	GameObject* newBull;
	if ( currBulletType == Bullet_Whip )
	{
		if ( m_pAIManager != nullptr )
			m_pAIManager->StartWhip();

		newBull = CreatePlayerWhipBullet( firingDir );

	}
	else if ( currBulletType == Bullet_Spread )
	{
		newBull = CreatePlayerSpreadBullet( firingDir );

	}
	else if ( currBulletType == Bullet_Missile )
	{
		newBull = CreatePlayerMissileBullet( firingDir );

	}


	if ( m_pAIManager )
		m_pAIManager->SetLastPlayerBullet( newBull );
	m_fShotTimer = 0;



	if ( currBulletType == eOBJECT_TYPE::Bullet_Spread )
		g_AudioSystem->PlaySound( "RATS_SpreadBulletFire", newBull );

	if ( currBulletType == eOBJECT_TYPE::Bullet_Missile )
		g_AudioSystem->PlaySound( "RATS_MissilePlayer", newBull );

	if ( currBulletType == Bullet_Whip )
	{
		if ( m_ObjectBuckets[Bullet_Whip].size() < 2 )
			return;

		if ( tmpBool )
			m_pWhipChannel->setPaused( false );

		m_Iter = m_ObjectBuckets[Bullet_Whip].begin();
		m_Iter++;
		float dotRes = DirectX::XMVector3AngleBetweenVectors( DirectX::XMLoadFloat3( (*m_ObjectBuckets[Bullet_Whip].begin())->GetTransform().GetZAxis() ), DirectX::XMLoadFloat3( (*m_Iter)->GetTransform().GetZAxis() ) ).m128_f32[0];

		g_AudioSystem->ModulateWhip( m_pWhipChannel, dotRes );

	}
	else
	{
		if ( !tmpBool )
			m_pWhipChannel->setPaused( true );
	}

}

GameObject* ObjectManager::CreatePlayerWhipBullet( XMFLOAT3 firingDir )
{
	m_pPlayer = GetPlayer();
	GameObject* newBullet;
	this->CreateObject( Bullet_Whip, *(m_pPlayer)->GetTransform().GetPosition() );
	newBullet = (*m_ObjectBuckets[Bullet_Whip].begin());
	newBullet->SetVelocityInWorldSpace( false );

	XMFLOAT3 newZ = firingDir, newY, newX;
	XMStoreFloat3( &newX, XMVector3Normalize( XMVector3Cross( XMLoadFloat3( m_pPlayer->GetTransform().GetYAxis() ), XMLoadFloat3( &newZ ) ) ) );
	XMStoreFloat3( &newY, XMVector3Normalize( XMVector3Cross( XMLoadFloat3( &newZ ), XMLoadFloat3( &newX ) ) ) );

	newBullet->GetTransform().SetXAxis( newX );
	newBullet->GetTransform().SetYAxis( newY );
	newBullet->GetTransform().SetZAxis( newZ );

	float playerVeloc = GetPlayer()->GetVelocity().z;
	if ( playerVeloc < 60 )
		newBullet->SetVelocity( DirectX::XMFLOAT3( 0, 0, 70 ) );
	else
		newBullet->SetVelocity( DirectX::XMFLOAT3( 0, 0, GetPlayer()->GetVelocity().z * 1.3f ) );

	//m_Molecules->BuildEmitter(newBullet, 30, "Whip Bullet Trail");

	return newBullet;
}

GameObject* ObjectManager::CreatePlayerSpreadBullet( XMFLOAT3 firingDir )
{
	int numBullets = gameData->m_Spread.fire_rate_upgrade_num + 2;
	float angle = (5 + (10 * gameData->m_Spread.fire_rate_upgrade_num)) * (XM_PI / 180.0f);
	m_pPlayer = GetPlayer();

	XMVECTOR startingDir =
		XMVector3Rotate( XMLoadFloat3( &firingDir ), XMQuaternionRotationAxis( XMLoadFloat3( m_pPlayer->GetTransform().GetYAxis() ), -(angle*0.5f) ) );

	XMVECTOR quat = XMQuaternionRotationAxis( XMLoadFloat3( m_pPlayer->GetTransform().GetYAxis() ), angle / (numBullets - 1) );

	GameObject* newBullet;
	XMFLOAT3 newZ, newY, newX;

	for ( int currBullet = 0; currBullet < numBullets; ++currBullet )
	{
		this->CreateObject( Bullet_Spread, *(m_pPlayer)->GetTransform().GetPosition() );
		newBullet = (*m_ObjectBuckets[Bullet_Spread].begin());
		newBullet->SetVelocityInWorldSpace( false );
		reinterpret_cast<RenderComp*>(newBullet->GetComponent( "RenderComp" ));

		XMStoreFloat3( &newZ, startingDir );
		XMStoreFloat3( &newX, XMVector3Normalize( XMVector3Cross( XMLoadFloat3( m_pPlayer->GetTransform().GetYAxis() ), XMLoadFloat3( &newZ ) ) ) );
		XMStoreFloat3( &newY, XMVector3Normalize( XMVector3Cross( XMLoadFloat3( &newZ ), XMLoadFloat3( &newX ) ) ) );

		newBullet->GetTransform().SetXAxis( newX );
		newBullet->GetTransform().SetYAxis( newY );
		newBullet->GetTransform().SetZAxis( newZ );

		startingDir = XMVector3Rotate( startingDir, quat );

		float playerVeloc = GetPlayer()->GetVelocity().z;
	//	if ( playerVeloc < 70 )
			newBullet->SetVelocity( DirectX::XMFLOAT3( 0, 0, 100 ) );
	//	else
	//		newBullet->SetVelocity( DirectX::XMFLOAT3( 0, 0, GetPlayer()->GetVelocity().z * 1.4f ) );


		m_Molecules->BuildEmitter( newBullet, 30, "Spread Bullet Trail" );

	}

	// Experimental bullet rotation
	//newBullet->SetSteadyRotation(2, 1.0f);

	return newBullet;
}

GameObject* ObjectManager::CreatePlayerMissileBullet( XMFLOAT3 firingDir )
{
	m_pPlayer = GetPlayer();
	GameObject* newBullet;

	int numBullets = gameData->m_Missile.fire_rate_upgrade_num + 1;
	float angle = (float)((10 + (20 * gameData->m_Missile.fire_rate_upgrade_num)));
	angle *= (XM_PI / 180.0f);
	if(numBullets == 1)
		angle = (XM_PI / 180.0f);	// Straight fire

	XMVECTOR startingDir =
		XMVector3Rotate( XMLoadFloat3( &firingDir ), XMQuaternionRotationAxis( XMLoadFloat3( m_pPlayer->GetTransform().GetYAxis() ), -(angle*0.5f) ) );

	XMVECTOR quat = XMQuaternionRotationAxis( XMLoadFloat3( m_pPlayer->GetTransform().GetYAxis() ), angle / (numBullets - 1) );

	// Multi-Missile Barrage
	for ( int i = 0; i < numBullets; i++ )
	{
		newBullet = CreateObject( Bullet_Missile, *(m_pPlayer)->GetTransform().GetPosition() );
		newBullet->SetVelocityInWorldSpace( false );

		XMFLOAT3 newZ = firingDir, newY, newX;
		XMStoreFloat3( &newZ, startingDir );		// Mimicing spread bullet math things
		XMStoreFloat3( &newX, XMVector3Normalize( XMVector3Cross( XMLoadFloat3( m_pPlayer->GetTransform().GetYAxis() ), XMLoadFloat3( &newZ ) ) ) );
		XMStoreFloat3( &newY, XMVector3Normalize( XMVector3Cross( XMLoadFloat3( &newZ ), XMLoadFloat3( &newX ) ) ) );

		newBullet->GetTransform().SetXAxis( newX );
		newBullet->GetTransform().SetYAxis( newY );
		newBullet->GetTransform().SetZAxis( newZ );

		float playerVeloc = GetPlayer()->GetVelocity().z;
		if ( playerVeloc != 60 )
			newBullet->SetVelocity( DirectX::XMFLOAT3( 0, 0, 70 ) );
		else
			newBullet->SetVelocity( DirectX::XMFLOAT3( 0, 0, GetPlayer()->GetVelocity().z * 1.4f ) );

		startingDir = XMVector3Rotate( startingDir, quat );

		m_Molecules->BuildEmitter( newBullet, 30, "Missile Bullet Trail" );
	}
	return newBullet;
}

void ObjectManager::SetupClients()
{
	using namespace Events;
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyDown", "MouseLButton" ), this, &ObjectManager::CreatePlayerBullet );
	EventManager()->RegisterClient( MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>( "GetKeyUp", "MouseLButtonUp" ), this, &ObjectManager::MouseUp );


	InputComponent* pComp = reinterpret_cast<InputComponent*>(GetPlayer()->GetComponent( "InputComponent" ));

	if ( gamePad )
		gamePad->RegisterListnener( 0, pComp );
	pComp->SetupClients();


}

void ObjectManager::GoodbyeClients()
{
	using namespace Events;

	EventManager()->UnregisterClient( "MouseLButton", this, &ObjectManager::CreatePlayerBullet );
	EventManager()->UnregisterClient( "MouseLButtonUp", this, &ObjectManager::MouseUp );

	InputComponent* pComp = reinterpret_cast<InputComponent*>(GetPlayer()->GetComponent( "InputComponent" ));

	if ( gamePad )
		gamePad->UnregisterListener( 0 );
	pComp->RemoveClients();

}

void ObjectManager::Enter()
{
	SetupClients();
}

void ObjectManager::Exit()
{
	GoodbyeClients();
}

void ObjectManager::Shoot360()
{
	GamePad tmpPad = gamePad->GetState();
	Ship_Weapon* currGun = gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select];
	int currBulletType = currGun->bullet_type;//Bullet_Whip;
	bool tmpBool;	m_pWhipChannel->getPaused( &tmpBool );

	float magnitude = (abs( tmpPad.normRS[0] ) + abs( tmpPad.normRS[1] )) * 0.5f;

	if ( globalGraphicsPointer->GetCrosshairShowing() )
		return;

	if ( magnitude < 0.4f )
	{
		if ( !tmpBool )
			m_pWhipChannel->setPaused( true );

		if ( m_pAIManager )
			m_pAIManager->StopWhip();

		return;
	}

	if ( m_fShotTimer < currGun->myFireRate )
		return;

	if ( !GetPlayer() )
		return;

	float currentAngle = GetPlayer()->GetTransform().GetRotation( 1 );

	XMVECTOR moveVec, scaledX, scaledY, currX = XMLoadFloat3( GetPlayer()->GetTransform().GetXAxis() ), currY = XMLoadFloat3( GetPlayer()->GetTransform().GetYAxis() ), currZ = XMLoadFloat3( GetPlayer()->GetTransform().GetZAxis() );
	scaledX = DirectX::XMVectorMultiply( currX, XMLoadFloat3( &XMFLOAT3( tmpPad.normRS[0], tmpPad.normRS[0], tmpPad.normRS[0] ) ) );
	scaledY = DirectX::XMVectorMultiply( currY, XMLoadFloat3( &XMFLOAT3( tmpPad.normRS[1], tmpPad.normRS[1], tmpPad.normRS[1] ) ) );

	moveVec = DirectX::XMVector3Normalize( DirectX::XMVectorAdd( scaledX, scaledY ) );

	float signage = DirectX::XMVector3Dot( currX, moveVec ).m128_f32[0];

	if ( signage != 0 )
		signage /= abs( signage );
	// 		else if (m_cntrollerState.normLS[1] < 0)
	// 			signage = 1;

	float ang = DirectX::XMVector3AngleBetweenVectors( currY, moveVec ).m128_f32[0] * signage;

	//XMVECTOR rotVal = DirectX::XMVectorATan2(XMLoadFloat3(&XMFLOAT3(m_cntrollerState.normLS[1], m_cntrollerState.normLS[1], m_cntrollerState.normLS[1])), XMLoadFloat3(&XMFLOAT3(m_cntrollerState.normLS[0], m_cntrollerState.normLS[0], m_cntrollerState.normLS[0])));

	DirectX::XMFLOAT3 firingDir;

	//XMVECTOR cameraX, cameraY;

	DirectX::XMVECTOR tmpCev = (XMLoadFloat3( GetCamera()->GetTransform().GetXAxis() ) * tmpPad.normRS[0]) + (XMLoadFloat3( GetCamera()->GetTransform().GetYAxis() ) * tmpPad.normRS[1]);


	DirectX::XMStoreFloat3( &firingDir, DirectX::XMVector3Normalize( tmpCev ) );
	//firingDir = DirectX::XMFLOAT3(tmpPad.normRS[0], 0, tmpPad.normRS[1]);

	//m_pOwner->GetTransform().RotateBy(1, ang - currentAngle);

	GameObject* newBull;
	if ( currBulletType == Bullet_Whip )
	{
		if ( m_pAIManager )
			m_pAIManager->StartWhip();

		newBull = CreatePlayerWhipBullet( firingDir );
	}
	else if ( currBulletType == Bullet_Spread )
	{
		newBull = CreatePlayerSpreadBullet( firingDir );
	}
	else if ( currBulletType == Bullet_Missile )
	{
		newBull = CreatePlayerMissileBullet( firingDir );

	}

	//std::cout << m_cntrollerState.normLS[0] << ", " << m_cntrollerState.normLS[1] << "\n";



	globalGraphicsPointer->SetCrosshairShowing( false );

	if ( m_pAIManager )
	{
		m_pAIManager->SetLastPlayerBullet( newBull );
	}
	m_fShotTimer = 0;

	if ( currBulletType == eOBJECT_TYPE::Bullet_Spread )
		g_AudioSystem->PlaySound( "RATS_SpreadBulletFire", newBull );// , *m_pPlayer->GetTransform().GetPosition());


	if ( currBulletType == eOBJECT_TYPE::Bullet_Missile )
		g_AudioSystem->PlaySound( "RATS_MissilePlayer", newBull );// , *m_pPlayer->GetTransform().GetPosition());

	if ( currBulletType == Bullet_Whip )
	{
		if ( m_ObjectBuckets[Bullet_Whip].size() < 2 )
			return;

		if ( tmpBool )
			m_pWhipChannel->setPaused( false );

		m_Iter = m_ObjectBuckets[Bullet_Whip].begin();
		m_Iter++;
		float dotRes = DirectX::XMVector3AngleBetweenVectors( DirectX::XMLoadFloat3( (*m_ObjectBuckets[Bullet_Whip].begin())->GetTransform().GetZAxis() ), DirectX::XMLoadFloat3( (*m_Iter)->GetTransform().GetZAxis() ) ).m128_f32[0];

		g_AudioSystem->ModulateWhip( m_pWhipChannel, dotRes );

	}
	else
	{
		if ( !tmpBool )
			m_pWhipChannel->setPaused( true );
	}

}

void ObjectManager::Terminate()
{

}

void ObjectManager::PlayerDeath()
{
	m_pPlayer->SetDeathTimer( 4 );

	globalGraphicsPointer->renderPlayer = false;

	if (globalGraphicsPointer->m_loseText)
	{
		globalGraphicsPointer->m_loseText->color = XMFLOAT4(1, 1, 1, 1);

	}


	//// Explode le player!
	//MoleculeEmitter *setup =
	m_Molecules->BuildPosEmitter(*m_pPlayer->GetTransform().GetPosition(), 400, "Player Explosion");
	m_Molecules->BuildPosEmitter(*m_pPlayer->GetTransform().GetPosition(), 400, "Player Explosion");
	m_Molecules->BuildPosEmitter(*m_pPlayer->GetTransform().GetPosition(), 400, "Player Explosion");
	m_Molecules->BuildPosEmitter(*m_pPlayer->GetTransform().GetPosition(), 400, "Player Explosion");
	//setup->loop = false;
	//setup->m_spawnCount = 35;
	//setup->m_spawnTime = 0.2f;
	//setup->m_data.timer = 3.0f;
	////gameData->m_bPlayerDead = true;

	g_AudioSystem->StopBGM();

	XMFLOAT3 expPos = XMFLOAT3(m_pPlayer->GetTransform().GetPosition()->x, m_pPlayer->GetTransform().GetPosition()->y, m_pPlayer->GetTransform().GetPosition()->z);
	GameObject* tmpAudio = CreateObject(Audio_OneShot, expPos);
	g_AudioSystem->PlaySound("RATS_PlayerDeath", tmpAudio);

}


void ObjectManager::EstablishRenderInfos()
{
	m_vRenderIDs[Enemy_Merc] = "Enemy";
	m_vRenderIDs[Enemy_Koi] = "Koi A";
	m_vRenderIDs[Enemy_Bomber] = "Bomber";
	m_vRenderIDs[Enemy_Boss] = "Boss";
	m_vRenderIDs[Enemy_Disruptor] = "Koi D";
	m_vRenderIDs[Player_Obj] = "Player";
	m_vRenderIDs[Pylon_Test] = "";
	m_vRenderIDs[Pylon_Lambda] = "Pylon";
	m_vRenderIDs[Pylon_Sigma] = "Pylon Sigma";
	m_vRenderIDs[Pylon_Omega] = "Pylon Omega";
	m_vRenderIDs[World_Obj] = "Sphere";
	m_vRenderIDs[Arrow_Sigma] = "arrowSigma";
	m_vRenderIDs[Arrow_Omega] = "arrowOmega";
	m_vRenderIDs[Arrow_Lambda] = "arrowLambda";
	m_vRenderIDs[Enemy_Dodger] = "Dodger";
	m_vRenderIDs[Enemy_MineLayer] = "Mine Layer";
	m_vRenderIDs[Enemy_FenceLayer] = "Fence Layer";
	m_vRenderIDs[Enemy_Goliath] = "Goliath";
	m_vRenderIDs[Bullet_Simple] = "Player Whip Bullet";
	m_vRenderIDs[Bullet_Whip] = "Player Whip Bullet";
	m_vRenderIDs[Bullet_Spread] = "Player Spread Bullet";
	m_vRenderIDs[Bullet_Missile] = "Player Missile Bullet";
	m_vRenderIDs[EvilBullet_Simple] = "Enemy Simple Bullet";
	m_vRenderIDs[EvilBullet_Disrupt] = "Enemy Simple Bullet";
	m_vRenderIDs[EvilBullet_HeatSeek] = "Enemy Missile";
	m_vRenderIDs[EvilBullet_Mine] = "Mine";
	m_vRenderIDs[EvilBullet_FencePost] = "Fence Post";
	m_vRenderIDs[Pickup_Health] = "Pickup Health";
	m_vRenderIDs[Pickup_Shield] = "Pickup Shield";
	m_vRenderIDs[VFX_Ribbon] = "Ribbon";
	m_vRenderIDs[VFX_Lightning] = "Lightning";
	m_vRenderIDs[InnerStar] = "InnerStar";
	m_vRenderIDs[Distort_Obj_A] = "Distortion A ";
	m_vRenderIDs[Distort_Obj_B] = "Distortion B ";
	m_vRenderIDs[Distort_Obj_C] = "Distortion C ";
	m_vRenderIDs[Distort_Obj_D] = "Distortion D ";
	m_vRenderIDs[Landing_Indicator] = "Landing Indicator";

}

void ObjectManager::SetupWhipSound()
{
	g_AudioSystem->PlaySound( "RATS_Whip", GetPlayer(), true, true );
	m_pWhipChannel = GetPlayer()->GetChannel();
	m_pWhipChannel->setPaused( true );
	g_AudioSystem->ConnectWhipDSP(m_pWhipChannel);
}

void ObjectManager::PauseWhipSound()
{
	if ( m_pWhipChannel )
		m_pWhipChannel->stop();
}

void ObjectManager::MouseUp( const Events::CGeneralEventArgs2<unsigned char, float>& args )
{
	if ( gameData->m_PlayerShip.myWeapon[gameData->m_PlayerShip.weapon_select]->bullet_type == Bullet_Whip )
	{
		if ( m_pAIManager != nullptr )
			m_pAIManager->StopWhip();
		m_pWhipChannel->setPaused( true );
	}
}


// Clears live objects on the game world
void ObjectManager::ClearLevel()
{
	// Maybe not.
}

void ObjectManager::SetCamOffset()
{
	std::string tmpString = gameData->m_levelShape;

	if (tmpString == "RoundedCube")
		m_fCamOffset = 155.0f;
	else if (tmpString == "Torus")
		m_fCamOffset = 130.0f;
	else
		m_fCamOffset = 120.0f;
	
	(reinterpret_cast<CameraObject*>(m_pCamera))->GrabOffset() = DirectX::XMFLOAT3(0, m_fCamOffset, 0);
	g_AudioSystem->SetCameraDistance(m_fCamOffset);


}
