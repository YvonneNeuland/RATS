#include "stdafx.h"
#include "AICompBase.h"


AICompBase::AICompBase()
{
	Reset();
}


AICompBase::~AICompBase()
{
}

void AICompBase::Update(float dt)
{
	for (m_Iter = m_vBehaviors.begin(); m_Iter != m_vBehaviors.end(); m_Iter++)
	{
		(*m_Iter)->DoLogic(dt);
	}
}

void AICompBase::Reset()
{
	m_AIType = AI_NONE;
	m_fTurnSpeed = 0.1f;
	m_fShotTimerMax = 1.0f;
	//m_fShotTimer = 0.0f;
	m_pTarget = 0;
	m_bReadyAim = false;
	m_fStartVelocity = 0;
	m_bMissileOut = false;
	m_pShooter = 0;
	m_pTrail = 0;
	m_bProxTrigger = false;
	m_bRotate = false;
	m_bAssimilate = false;
	m_bNeedShootComp = false;
}
