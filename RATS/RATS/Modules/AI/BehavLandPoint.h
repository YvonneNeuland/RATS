#pragma once
#include "BaseBehavior.h"

class AIManager;

class BehavLandPoint :
	public BaseBehavior
{
	friend AIManager;


public:
	BehavLandPoint();
	~BehavLandPoint();
	void DoLogic(float _dt);

	float scale;
	bool goingUp;

	float m_StartDist;
	float m_currDist;

};

