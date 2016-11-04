#include "stdafx.h"
#include "Wave.h"


Wave::Wave()
{
}


Wave::~Wave()
{
}

vector<EnemyTypeInfo*>& Wave::GetEnemyTypeInfo(void)
{
	
	return m_vLoadedEnemyInfos;
}

void Wave::Terminate(void)
{
	for (unsigned int i = 0; i < m_vLoadedEnemyInfos.size(); i++)
	{
		delete m_vLoadedEnemyInfos[i];
	}
}


