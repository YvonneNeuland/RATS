#include "stdafx.h"
#include "Level.h"

Level::Level()
{
}


Level::~Level()
{
}


void Level::Terminate(void)
{
	for (unsigned int i = 0; i < GetNumWaves(); i++)
	{
		m_vLoadedWaves[i]->Terminate();
		delete m_vLoadedWaves[i];
	}
}

Wave* Level::GetWave(unsigned int numWave)
{
	return m_vLoadedWaves[numWave];
}

unsigned int Level::GetNumWaves(void)
{
	return m_vLoadedWaves.size();
}





