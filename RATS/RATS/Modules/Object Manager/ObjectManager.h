#pragma once
//#define NUM_GAME_OBJECTS 8

#include <list>
#include <vector>
#include "../../Wwise files/EventManager.h"
#include "../Collision/CollisionManager.h"
#include "../Components/BaseComponent.h"
#include "../Audio/FFmod.h"


//struct ParticleSystemDescriptor;
class AIManager;
class ObjectFactory;
class GameObject;

class MoleculeManager;
class GamePlayState;

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void Update(float dt);		

	// Grabs an object from the deadlist for use in the living buckets
	GameObject* CreateObject(int type,																// Game Object type
							DirectX::XMFLOAT3 pos,													// 3D space location, when relevant
							std::string render_ID = "",												// Render ID based on Object type if not passed in; indicates mesh to use
							eRENDERCOMP_TYPE rendType = eRENDERCOMP_TYPE::Render2D_Back);			// RenderType is for specialized buckets of items so renderer can handle them appropriately
	
	
	void DestroyObject(int type, std::list<GameObject*>::iterator object);					// When objects are removed from the scene, this sets them back into the deadlist
																		// Returns components to Factory
	void Initialize();													// Sets up allocations for non gameplay (smaller allotment)
	void InitializeGamePlay();											// Sets up buckets and lists for gameplay allocation
	void SetFactory(ObjectFactory* _handle) { m_pFactory = _handle; }
	void SetAIManager(AIManager* _handle) { m_pAIManager = _handle; }

	void ResetAll();
	void ClearLevel();

	void Terminate();
	unsigned int NumEnemiesInBuckets();


	void Enter();
	void Exit();

	// INPUT HANDLERS
	void CreatePlayerBullet(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void MouseUp(const Events::CGeneralEventArgs2<unsigned char, float>& args);


	
	GameObject* GetCamera() {return m_pCamera; }

	GameObject* GetPlayer();
	std::vector<std::list<GameObject*>>& PassBuckets() { return m_ObjectBuckets; }

	//static DirectX::XMFLOAT3 GetCameraPosition();

	void SetupWhipSound();
	void PauseWhipSound();

	// Debug Testing Function(s)
	void TestCreateThings();
	void SetupClients();
	void GoodbyeClients();
	ObjectFactory* m_pFactory;
	MoleculeManager* m_Molecules;
	GamePlayState* m_pGPS = nullptr;
	void SetCamOffset();

private:


	// Normal Objects
	std::vector<std::list<GameObject*>> m_ObjectBuckets;
	std::list<GameObject*> m_DeadList;

	// Inherited Objects (VFX)
	std::list<GameObject*> m_LightningVFX;
	std::list<GameObject*> m_AnimTexVFX;
	//std::list<GameObject*> m_ParticleVFX;
	std::list<GameObject*> m_RibbonVFX;
	std::list<GameObject*> m_DistortionVFX;

	// List for items that are still on screen but don't update normally (death animations, etc)
	std::list<GameObject*> m_DyingObjects;		// Temporary list that does not get updated
	
	// Useful pointers to important objects and systems
	GameObject* m_pPlayer;
	GameObject* m_pCamera;
	GameObject* m_pMeshDis;

	AIManager* m_pAIManager;
	CollisionManager m_CollisionManager;

	// Particle Descriptors
	//ParticleSystemDescriptor *m_pPartExplode = nullptr;

	// Default RenderIDs per game object
	std::vector<std::string> m_vRenderIDs;
	void EstablishRenderInfos();

	// TESTING VARS
	float m_fShotTimer;
	bool test;

	// 360 controller stuff
	DWORD dwPackNo;
	void Shoot360();

	// Special Happenings
	void HandleEnemyDeath(std::list<GameObject*>::iterator enemy);
	void KillTheDying(std::list<GameObject*>::iterator dead);
	void PlayerDeath();
	
	// Player Bullet Creation Helpers
	GameObject* CreatePlayerWhipBullet(DirectX::XMFLOAT3 firingDir);
	GameObject* CreatePlayerSpreadBullet(DirectX::XMFLOAT3 firingDir);
	GameObject* CreatePlayerMissileBullet(DirectX::XMFLOAT3 firingDir);

	// Whip Audio
	FMOD::Channel* m_pWhipChannel = 0;
	std::list<GameObject*>::iterator m_Iter;
	
	// bug fixing
	float m_fCamOffset;

	unsigned int nTest = 0;
};