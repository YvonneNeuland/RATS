#pragma once
#include "BaseBehavior.h"
#include <DirectXMath.h>
#include <list>
#include "../VFX/ElectricityEffect/ElectricityEffect.h"

class AIManager;
class BehavFlocking;
class AICompBase;

#define DRAINDELAY 0.8f

class Flock
{
	friend BehavFlocking;
	friend AIManager;

	float m_fAlignmentWait;
	float m_fCohesionWait;
	float m_fSeparationWait;
	DirectX::XMFLOAT3 m_vAvgPos;
	DirectX::XMFLOAT3 m_vAvgForward;
	float m_fAvgSpeed;
	float m_fFlockRadius;
	std::list<AICompBase*> m_vFlock;
	std::list<AICompBase*>::iterator m_Iter;
	bool m_bDisband;
	bool m_bRotate;

	//AICompBase* m_pLeader;

	void Update(float _dt);
	void CalcAverages();

};


class BehavFlocking :
	public BaseBehavior
{
	friend Flock;
	friend AICompBase;
	friend AIManager;

	float m_fSafeRadius;
	
	std::list<AICompBase*>::iterator m_Iter;
	Flock* m_pStats;
	bool bSlowDownBuddy;
	
	DirectX::XMVECTOR CalcAlignment();
	DirectX::XMVECTOR CalcCohesion();
	DirectX::XMVECTOR CalcSeparation(); 
	DirectX::XMVECTOR CalcTracking();

	void Detach();

	bool m_bLatch;
	float m_fDrainTImer;
	ElectricityEffect* m_pLatchVFX[2];


public:
	BehavFlocking();
	~BehavFlocking();
	void DoLogic(float _dt);

	float GetSafeRadius() const { return m_fSafeRadius; }
	void SetSafeRadius(float val) { m_fSafeRadius = val; }
	Flock* GetStats() const { return m_pStats; }
	void SetStats(Flock* val) { m_pStats = val; }

	
};

