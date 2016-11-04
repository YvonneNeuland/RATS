#pragma once
#include "BaseBehavior.h"

class AIManager;


class BehavDodge :
	public BaseBehavior
{

	friend AIManager;

public:
	BehavDodge();
	~BehavDodge();

	void DoLogic(float _dt);

};

