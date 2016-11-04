#pragma once
#include "BaseBehavior.h"
class BehavEnvMove : public BaseBehavior
{

private:
	float m_fTurnTimer = 3.0f;

public:
	BehavEnvMove();
	~BehavEnvMove();
	void DoLogic(float _dt);
};

