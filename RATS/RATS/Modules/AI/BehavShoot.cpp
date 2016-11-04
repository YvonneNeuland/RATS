#include "stdafx.h"
#include "BehavShoot.h"
#include "AICompBase.h"

BehavShoot::BehavShoot()
{
	m_fTimer = -2;
	m_logicType = bShoot;
}


BehavShoot::~BehavShoot()
{
}

void BehavShoot::DoLogic(float _dt)
{
	if (m_fTimer == -2)
		m_fTimer = m_Owner->GetShotTimer();

	if (m_fTimer == -1)
		return;

	if (!m_Owner->IsMissileOut())
		m_fTimer -= _dt;

	if (m_fTimer <= 0)
	{
		m_Owner->ReadyAim(true);

		m_fTimer = m_Owner->GetShotTimer();
			
	 
	}
}
