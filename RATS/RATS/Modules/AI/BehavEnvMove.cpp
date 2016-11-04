#include "stdafx.h"
#include "BehavEnvMove.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/Transform.h"

BehavEnvMove::BehavEnvMove()
{
	m_logicType = bEnvMove;
}


BehavEnvMove::~BehavEnvMove()
{
}

void BehavEnvMove::DoLogic(float _dt)
{

	if (m_fTurnTimer >= 0.0f)
	{
		m_fTurnTimer -= _dt;
	}
	else
	{
		m_fTurnTimer = 3.0f;

		int degrees = rand() % 359 + 1;
		m_Owner->GetOwner()->GetTransform().RotateBy(1, (float)degrees, 1.0f, true);
	}

	
}