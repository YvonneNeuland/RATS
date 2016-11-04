#pragma once
#include "BaseObject.h"
#include "../../Wwise files/Transform.h"
#include "../Components/BaseComponent.h"
#include "fmod.hpp"
#include <list>


enum eOBJECT_TYPE { InnerStar2 = 0, Enemy_Merc = 1, Enemy_Bomber, Enemy_Koi,			// Enemies
					Enemy_Disruptor, Enemy_Dodger, Enemy_MineLayer, 
					Enemy_Goliath, Enemy_Boss, Enemy_FenceLayer,
					Player_Obj, Camera_Obj,	World_Obj,							// Single item things
					Bullet_Simple, Bullet_Whip, Bullet_Spread, Bullet_Missile,	// Player bullet types
					EvilBullet_Simple, EvilBullet_HeatSeek,						// Enemy Bullet types
					EvilBullet_Mine, EvilBullet_Disrupt,
					EvilBullet_FencePost,
					Arrow_Sigma, Arrow_Lambda, Arrow_Omega,						// Special Render Doodads
					Orb_Blue, Orb_Red, Orb_Yellow,								// Energy (colors probably deprecated)
					Pylon_Test, Pylon_Lambda, Pylon_Sigma, Pylon_Omega,			// Pylons
					Env_Tornado, Env_Prominence, Env_Prominence_Bullet,			// Environmental Effects
					Env_Singularity,
					Pickup_Health,	Pickup_Shield,								// Various PickUps
					VFX_Lightning, VFX_Particle, VFX_AnimTex, 					// Visual Effects
					VFX_Ribbon,	VFX_Distortion,
					HUD_2D,								
					Landing_Indicator,
					// Anything 2D
					InnerStar,															// The Inner Star Object
					Distort_Obj_A, Distort_Obj_B, Distort_Obj_C, Distort_Obj_D,	// Objects used by mesh distorters
					Scenery_Obj, Audio_OneShot, Audio_LoopHelp,
					MAX_obj
					
																};		

enum eSTATUS_FLAGS { Stunned = 1, Slow = 2, Spwaning = 4, Waiting = 8, Normal = 16, Dashing = 32,  MostlyDead = 64, AllDead = 128 };

class RibbonEffect;
class ObjectManager;
class MoleculeEmitter;

class GameObject : BaseObject
{
public:
	GameObject();
	~GameObject();

	Transform& GetTransform() { return m_transform; }

	eOBJECT_TYPE GetType() const { return m_type; }
	void SetType(eOBJECT_TYPE type) { m_type = type; }
	eOBJECT_TYPE GetSecondaryType() const { return m_SecondaryType; }
	void SetSecondaryType(eOBJECT_TYPE val) { m_SecondaryType = val; }

	DirectX::XMFLOAT3 GetVelocity() const { return m_Velocity; }
	void SetVelocity(DirectX::XMFLOAT3 v) { m_Velocity = v; }

	DirectX::XMFLOAT3 GetImpulse() const { return m_Impulse; }
	void SetImpulse(DirectX::XMFLOAT3 v) { m_Impulse = v; }

	std::list<BaseComponent*>& GetComponents() {return m_compList;}

	void Update(float dt);
	bool TakeDamage(float val);
	
	const bool IsVelocityInWorldSpace() const { return m_VelocityInWorldSpace; }
	void SetVelocityInWorldSpace(const bool b) { m_VelocityInWorldSpace = b; }
	void SetObjManager(ObjectManager *objman);

	bool isDead() { return GetFlag(MostlyDead); }
	void SetDead(bool dead = true);

	bool isAnEnemy();


	unsigned char GetStatusFlags() const { return m_StatusFlags; }
	void SetFlags(unsigned char val);
	void ClearFlags(unsigned char val);
	void ClearAllStatusFlags() { m_StatusFlags = 0; }
	bool GetFlag(unsigned char val);
	
	template<typename T> T* GetComponent();
	BaseComponent* GetComponent(char* type);

	void Reset();
	void DeathUpdate(float dt);
	void SetDeathTimer(float time) { m_fDeathTimer = time; }

	void FlashWhite(float _dt);

	RibbonEffect* GetTrail() { return m_trail; }
	void SetTrail(RibbonEffect* trail) { m_trail = trail; }

	void SetSteadyRotation(int axis, float rotation) { rotAx = axis; rotRad = rotation; }
	void SetSteadyOrbit(DirectX::XMFLOAT3 posToOrbit, DirectX::XMFLOAT3 axis, float radians) { pointToOrbit = posToOrbit; orbitAxis = axis; orbitSpeed = radians; isOrbiting = true; }
	void StopOrbiting() { isOrbiting = false; }

	FMOD::Channel* GetChannel() const { return m_pChannel; }
	void SetChannel(FMOD::Channel* val) { m_pChannel = val; }


	ObjectManager *m_ObjManager;

	bool IsShielded() const { return m_bShielded; }
	void SetShielded(bool val);

	void ResetPointers();

	void SpinOut();
	bool IsSpinningOut() { return m_fSpinTImer > 0 ? true : false; }

	void Ignite();

	void BomberFlag() { bomberSucceed = true; }
	bool DidBomberWin() { return bomberSucceed; }

protected:
	std::list<BaseComponent*> m_compList;
	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT3 m_Impulse;

	Transform m_transform;
	eOBJECT_TYPE m_type;
	eOBJECT_TYPE m_SecondaryType;
	
	unsigned char m_StatusFlags;
	bool m_VelocityInWorldSpace;
	bool m_Destroy;

	float m_fDeathTimer = 0.0f;
	float m_fDmgTimer;
	float m_fFlashHelp;

	RibbonEffect* m_trail;

	int rotAx = -1;
	float rotRad = 0;

	DirectX::XMFLOAT3 pointToOrbit = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 orbitAxis = DirectX::XMFLOAT3(0, 0, 0);
	float orbitSpeed = 0;
	bool isOrbiting = false;

	FMOD::Channel* m_pChannel;
	FMOD_VECTOR m_FmodVec;

	bool m_bShielded;

	float m_fShieldTimer;


	DirectX::XMFLOAT3 m_SpinOutDir;
	float m_fSpinTImer;

	float m_fFireTimer;
	MoleculeEmitter* m_pFireParticle;

	bool bomberSucceed;

private:
	GameObject(const GameObject&) = delete;
	GameObject(const GameObject&&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject& operator=(const GameObject&&) = delete;


	BaseComponent* healthComp;
	BaseComponent* moveComp;
	BaseComponent* colliderComp;
	BaseComponent* inputComp;

};

// GetComponent<BaseComponent>()
template<typename T>
T* GameObject::GetComponent()
{
	std::list<BaseComponent*>::iterator currComp = m_compList.begin();

	while (currComp != m_compList.end())
	{
		if (typeid(**currComp) == typeid(T))
			return (T*)(*currComp);
		++currComp;
	}

	return nullptr;
}