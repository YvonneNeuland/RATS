#pragma once
#include "BaseBehavior.h"
#include "../../Wwise files/Transform.h"

class AIManager;

class BehavBoss :
	public BaseBehavior
{

	friend AIManager;

	float m_fDmgTaken;

	int nStage;

	float m_fLastHealth;

	bool bFirst;

	float m_fShieldTimer;

	Transform m_shootTransform;

	float m_fShootTimer;
	float m_fMissileTimer;


public:
	BehavBoss();
	~BehavBoss();

	void DoLogic(float _dt);
	void Reset();
	
	void ShootSpread(int numBullets, DirectX::XMFLOAT3 firingDir);
	void ShootHSM();

	void SendMinions(int numEmemies);

	void UpdateFiringDirection(float _dt);

	void SwitchShield(std::string _old, std::string _new);
	
};

