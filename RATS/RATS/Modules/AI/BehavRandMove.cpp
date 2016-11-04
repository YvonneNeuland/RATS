#include "stdafx.h"
#include "BehavRandMove.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/Transform.h"

BehavRandMove::BehavRandMove()
{
	m_fTurnTimer = 0.0f;
	m_bNegateNext = false;
	m_logicType = bRandMove;
}


BehavRandMove::~BehavRandMove()
{
}

void BehavRandMove::DoLogic(float _dt)
{
	m_fTurnTimer -= _dt;
	if (m_fTurnTimer <= 0)
	{
		int randHelp = rand() % 20 + 50;

		float fTmp = (float)randHelp;

		fTmp *= .02f;

		m_fTurnTimer = fTmp;

		m_bNegateNext = !m_bNegateNext;
	}

	if (m_bNegateNext)
		m_Owner->GetOwner()->GetTransform().RotateBy(1, -1 * GetOwner()->GetTurnSpeed() * _dt);
	else
		m_Owner->GetOwner()->GetTransform().RotateBy(1,  GetOwner()->GetTurnSpeed() * _dt);
}
