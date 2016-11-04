#pragma once
#include "BaseBehavior.h"

class AIManager;

class BehavProxMine :
	public BaseBehavior
{

	friend AIManager;

	float m_fPauseTimer;

public:
	BehavProxMine();
	~BehavProxMine();

	void DoLogic(float _dt);

	void TrackTarget(float _dt);
	
};

