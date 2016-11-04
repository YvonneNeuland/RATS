#pragma once
#include "BaseBehavior.h"
#include <list>

class AIManager;
class GameObject;

class BehavGoliath :
	public BaseBehavior
{

	friend AIManager;

	float m_fStrength;
	float m_fScale;

	//bool m_bNeedShootComponent;
	std::list<GameObject*>::iterator m_Iter;

	bool bHitOmega;
	bool bHitSigma;
	bool bHitLambda;

	bool bDoneGrowing;

public:
	BehavGoliath();
	~BehavGoliath();

	void Reset();

	void DoLogic(float _dt);
	void TrackTarget(float _dt);
	void ChooseTarget();
	void LevelUp();
};

