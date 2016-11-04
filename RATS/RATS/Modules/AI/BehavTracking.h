#pragma once
#include "BaseBehavior.h"

class AIManager;

class BehavTracking :
	public BaseBehavior
{
	friend AIManager;


	float m_fMissilePauseTimer;
	float m_fRotSpeed;

	float m_targetTimer;

public:
	BehavTracking();
	~BehavTracking();
	void DoLogic(float _dt);
};

