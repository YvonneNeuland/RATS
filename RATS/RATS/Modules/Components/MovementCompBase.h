#pragma once

#include "BaseComponent.h"

#include "../Collision/CollisionManager.h"

using namespace DirectX;

class MovementCompBase :
	public BaseComponent
{
public:
	MovementCompBase();
	~MovementCompBase();
	void Reset() {return;}
};

