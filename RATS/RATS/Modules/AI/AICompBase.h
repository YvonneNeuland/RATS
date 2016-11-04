#pragma once
#include "../Components/BaseComponent.h"
#include "AIManager.h"
#include "BaseBehavior.h"
#include <list>

class GameObject;
class BehavFlocking;
class RibbonEffect;
class BehavGoliath;
class BehavSpawning;

class AICompBase : public BaseComponent
{
	friend AIManager;
	friend BaseBehavior;
	friend BehavFlocking;
	friend BehavGoliath;
	friend BehavSpawning;

public:
	AICompBase();
	~AICompBase();

	enum eAITYPE {AI_NONE = -1, AI_MERC = 0, AI_MISSILE, AI_KOI_A, AI_KOI_D,
		AI_BOMBER, AI_MINELAYER, AI_MINE, AI_WHIP, AI_PLAYER_MISSILE, AI_TORNADO, AI_PROMINENCE, AI_PROMINENCE_BULLET,
		AI_DODGE, AI_SINGULARITY, AI_FENCELAYER, AI_GOLIATH, AI_FENCEPOST, AI_LANDINGICON, AI_BOSS, AI_PICKUP};
	//enum eAILOGIC {LOGIC_NONE = -1, LOGIC_FOLLOW_PLAYER, LOGIC_FLOCK, LOGIC_PYLON};

protected:
	eAITYPE m_AIType;
	bool m_bReadyAim;
	bool m_bMissileOut;
	AICompBase* m_pShooter;
	

	//eAILOGIC m_Logic;
	float m_fTurnSpeed;
	float m_fStartVelocity;

	float m_fAggroRadius;
	float m_fShotTimerMax;
	std::list<BaseBehavior*> m_vBehaviors;
	std::list<BaseBehavior*>::iterator m_Iter;
	GameObject* m_pTarget;
	GameObject* m_pTrail;
	bool m_bRotate;
	bool m_bProxTrigger;
	bool m_bAssimilate;
	bool m_bNeedShootComp;

public:
	AICompBase::eAITYPE GetAIType() const { return m_AIType; }
	void SetAIType(AICompBase::eAITYPE val) { m_AIType = val; }
	float GetTurnSpeed() const { return m_fTurnSpeed; }
	void SetTurnSpeed(float val) { m_fTurnSpeed = val; }
	float GetShotTimer() const { return m_fShotTimerMax; }
	void SetShotTimer(float _val) { m_fShotTimerMax = _val; }
	GameObject* GetTarget() const { return m_pTarget; }
	void SetTarget(GameObject* val) { m_pTarget = val; }
	//bool ReadyAim() const { return m_bReadyAim; }
	void ReadyAim(bool val) { m_bReadyAim = val; }
	float GetStartVelocity() const { return m_fStartVelocity; }
	void SetStartVelocity(float val) { m_fStartVelocity = val; }
	void Update(float dt);
	bool IsMissileOut() const { return m_bMissileOut; }
	void SetMissileOut(bool val) { m_bMissileOut = val; }
	AICompBase* GetShooter() const { return m_pShooter; }
	void SetShooter(AICompBase* val) { m_pShooter = val; }
	RibbonEffect* GetTrail() { return (RibbonEffect*)m_pTrail; }
	bool GetProxTrigger() const { return m_bProxTrigger; }
	void SetProxTrigger(bool val) { m_bProxTrigger = val; }

	bool GetAssimilate() const { return m_bAssimilate; }
	void SetAssimilate(bool val) { m_bAssimilate = val; }

	void Reset();

};

