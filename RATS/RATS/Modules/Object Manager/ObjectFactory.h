#pragma once

/*
	An interface class to the factory functionality.
	Allocates or Assigns Components as requested, using one of two sub modules.
	Warehouses the components that get attached to objects.

	It's really more of a Component Manager than a factory
	(Maybe both)
	
*/

//#include "GameObject.h"
#include <vector>
#include <list>
#include "../Components/BaseComponent.h"
#include "../Renderer/RenderComp.h"

//#include "../Components/HealthComponent.h"
//#include "../Components/InputComponent.h"

class HealthComponent;
class InputComponent;
class GameObject;
class BaseComponent;
class AIManager;


class ObjectFactory
{
public:
	ObjectFactory();
	~ObjectFactory();

	void Initialize();					// Startup allocation
	bool InitializeGamePlay();			// Gameplay allocation

	// Sorts item to be created by type and calls relevant creation function
	bool InitializeObject(GameObject *gameObject, int objectType, std::string render_ID = "", eRENDERCOMP_TYPE render_type = eRENDERCOMP_TYPE::Render2D_Back);

	// Cleans up an object's list of components, or a singular one
	void RecycleComponents(std::list<GameObject*>::iterator &gameObject, bool keepRender = false);
	void RecycleComponent(std::list<GameObject*>::iterator &gameObject, int compType);

	// Bucket passing for render-engine
	std::vector<std::vector<std::vector<BaseComponent*>>> *GetRenderBuckets();
	std::vector<std::vector<int>> *GetRenderSizes() { return &m_RenderSizes; }

	std::vector<std::vector<BaseComponent*>> *GetGlowBuckets() { return &m_GlowLive; }
	std::vector<int> *GetGlowSizes() { return &m_GlowSizes; }

	static bool Register(int Comp_Type, int Sub_Type, void());

	void SetAIManager(AIManager *aim);

	// Glow Effect Handlers
	void SortGlowEffects();
	bool AddGlowEffect(GameObject *obj, std::string glowID);
	bool RemoveGlowEffect(BaseComponent *obj, int gtype);

	// RenderInfo Cleanup Helper
	void RenderInfoHelper();

private:
	// Never did get around to this
	struct Registration
	{
		unsigned int max, alloc;
		void (*MakeMe)();
	};


	unsigned int num_Bullets = 0;	// Old debug thing

	AIManager *m_AIManager;

	std::vector<std::vector<Registration>> m_Registry;

	std::list<BaseComponent*> m_HealthLive;
	std::list<BaseComponent*> m_HealthDead;

	std::list<BaseComponent*> m_InputLive;
	std::list<BaseComponent*> m_InputDead;

	std::list<BaseComponent*> m_AILive;
	std::list<BaseComponent*> m_AIDead;

	std::list<BaseComponent*> m_MoveDead;
	std::list<BaseComponent*> m_MoveLive;

	std::list<BaseComponent*> m_BVDead;
	std::list<BaseComponent*> m_BVLive;

	
	//std::vector<std::vector<std::list<BaseComponent*>*>> m_DeadBuckets;  
	//std::vector<std::vector<std::list<BaseComponent*>*>> m_LiveBuckets;	

	// Vector Refactoring for Rendering
	std::vector<std::vector<std::vector<BaseComponent*>>> m_RenderBuckets;
	std::vector<std::vector<int>> m_RenderSizes;


	// Glow Effects for Instancing, using same format as RenderComps
	std::vector<std::vector<BaseComponent*>> m_GlowLive;
	std::vector<int> m_GlowSizes;
	//std::list<BaseComponent*> m_GlowDead;




	// Helper Functions
	bool FindIterator(BaseComponent *findme, std::list<BaseComponent*> &list, std::list<BaseComponent*>::iterator &store);
	int FindRenderIndex(BaseComponent *findme, int rendType, int objType);
	int FindGlowIndex(BaseComponent *findme, int glowType);
	std::list<BaseComponent*>& ChooseList(std::list<BaseComponent*>::iterator it);



	// Initialization Functions
	bool IntializeDummyOneShot(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool IntializeProjectile(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool IntializeSmartProjectile(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeAOEProjectile(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializePowerUp(GameObject* gameObject, int objectType,  std::string render_ID = "");
	bool InitializePlayer(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeEnemy(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializePylon(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeOrb(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeArrows(GameObject* gameObject, int objectType, std::string render_ID);
	bool InitializeWorld(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeScenery(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeInnerStar(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeLandingZone(GameObject* gameObject, int objectType, std::string render_ID = "");

	bool InitializeCamera(GameObject* gameObject, int objectType, std::string render_ID = "");
	
	bool InitializeVFXAnimTex(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeVFXLightning(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeVFXParticle(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializeVFXPRibbon(GameObject* gameObject, int objectType, std::string render_ID = "");
	bool InitializePyonParts(GameObject* gameObject, int objectType, std::string render_ID);
	
	bool InitializeRender2D(GameObject* gameObject, int objectType, std::string render_ID = "", eRENDERCOMP_TYPE type = eRENDERCOMP_TYPE::Render2D_Back);
	bool InitializeEnvironmentalHazard(GameObject* gameObject, int objectType, std::string render_ID = "");

	bool InitializeDistortable(GameObject* gameObject, int objectType, std::string render_ID = "");



	bool AddHealthComponent(GameObject* gameObject, int objType);

	BaseComponent* AddRenderComponent(int rendType, int objType);

};

