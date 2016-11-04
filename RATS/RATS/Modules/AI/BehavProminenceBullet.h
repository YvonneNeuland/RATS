#pragma once
#include "BaseBehavior.h"
#include "../Renderer/D3DGraphics.h"
#include "../../Wwise files/Transform.h"

class BehavProminenceBullet : public BaseBehavior
{
	XMFLOAT3 startingPosition;
	XMFLOAT3 targetPosition;
	XMFLOAT3 currentPosition;
	XMFLOAT3 yPositive = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 yNegative = XMFLOAT3(0.0f, -1.0f, 0.0f);
	float r = 0.0f;

	bool targetSet = false;
	bool velocityIncreased = false;
	float m_fFrameTimer = 0.0f;

	void SetTargetPosition();
	void TranslateTargetUp(float offset);
	void LookAt();
public:
	
	BehavProminenceBullet();
	~BehavProminenceBullet();
	void DoLogic(float _dt);
};

