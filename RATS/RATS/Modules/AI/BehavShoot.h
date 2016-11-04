#pragma once
#include "BaseBehavior.h"
class BehavShoot :
	public BaseBehavior
{
	float m_fTimer;
public:
	BehavShoot();
	~BehavShoot();

	void DoLogic(float _dt);
};

