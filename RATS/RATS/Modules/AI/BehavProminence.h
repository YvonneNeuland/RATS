#pragma once
#include "BaseBehavior.h"
class BehavProminence : public BaseBehavior
{
	float m_fFireTimer = 1.0f;
	bool fireTest = false;
public:
	BehavProminence();
	~BehavProminence();
	void DoLogic(float _dt);
};

