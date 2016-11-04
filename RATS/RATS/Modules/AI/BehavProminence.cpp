#include "stdafx.h"
#include "BehavProminence.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/Transform.h"

BehavProminence::BehavProminence()
{
	m_logicType = bProminenceFire;
}


BehavProminence::~BehavProminence()
{
}

void BehavProminence::DoLogic(float _dt)
{
	if (m_fFireTimer >= 0.0f)
	{
		m_fFireTimer -= _dt;		
	}
	else
	{
		m_fFireTimer = 1.0f;
		m_Owner->ReadyAim(true);
	}

	/*if (!fireTest)
	{
		m_Owner->ReadyAim(true);
		fireTest = true;
	}*/

}