#pragma once
#include "BaseBehavior.h"
class BehavRandMove :
	public BaseBehavior
{

	float m_fTurnTimer;
	bool m_bNegateNext;

public:
	BehavRandMove();
	~BehavRandMove();
	void DoLogic(float _dt);
};

