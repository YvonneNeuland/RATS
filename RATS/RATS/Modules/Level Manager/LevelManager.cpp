#include "stdafx.h"
#include "LevelManager.h"
#include "../TinyXml/tinystr.h"
#include "../TinyXml/tinyxml.h"
#include <cassert>
#include <fstream>

LevelManager::LevelManager()
{
}


LevelManager::~LevelManager()
{
}

bool LevelManager::LoadLevelFile(string filePath)
{
	assert(filePath.c_str() != nullptr);

	TiXmlDocument doc;

	if (doc.LoadFile(filePath.c_str()) == false)
	{
		return false;
	}

	TiXmlElement* pLevels = doc.RootElement();

	if (pLevels == nullptr)
	{
		return false;
	}

	TiXmlElement* pLevel = pLevels->FirstChildElement();

	while (pLevel != nullptr)
	{
		Level* newLevel = new Level;

		int numLevel;
		pLevel->Attribute("numLevel", &numLevel);
		newLevel->SetLevelNum(numLevel);

		string levelShape = "";
		levelShape = pLevel->Attribute("levelShape");
		newLevel->SetLevelShape(levelShape);

		TiXmlElement* pWave = pLevel->FirstChildElement();

		while (pWave != nullptr)
		{
			if (pWave == nullptr)
			{
				return false;
			}
			Wave* newWave = new Wave;

			int numWave;
			pWave->Attribute("numWave", &numWave);
			newWave->SetNumWave(numWave);

			TiXmlElement* pEnemyInfo = pWave->FirstChildElement();

			while (pEnemyInfo != nullptr)
			{
				if (pEnemyInfo == nullptr)
				{
					return false;
				}

				EnemyTypeInfo* newEnemyTypeInfo = new EnemyTypeInfo;

				int enemyType;
				pEnemyInfo->Attribute("enemyType", &enemyType);
				newEnemyTypeInfo->enemyType = (eOBJECT_TYPE)enemyType;

				int numEnemies;
				pEnemyInfo->Attribute("numEnemies", &numEnemies);
				newEnemyTypeInfo->numEnemy = numEnemies;

				newWave->GetLoadedEnemyInfoVector().push_back(newEnemyTypeInfo);
				pEnemyInfo = pEnemyInfo->NextSiblingElement();
			}

			newLevel->GetLoadedWaveVector().push_back(newWave);
			pWave = pWave->NextSiblingElement();
		}

		m_mLoadedLevels[newLevel->GetNumLevel()] = newLevel;
		pLevel = pLevel->NextSiblingElement();

	}

	return true;
}

void LevelManager::Terminate(void)
{
	auto iter = m_mLoadedLevels.begin();

	for (; iter != m_mLoadedLevels.end(); iter++)
	{
		m_mLoadedLevels[iter->first]->Terminate();
		delete m_mLoadedLevels[iter->first];
	}
}

Level* LevelManager::GetLevel(int numLevel)
{
	return m_mLoadedLevels[numLevel];
}

unsigned int LevelManager::GetNumLevels(void)
{
	return m_mLoadedLevels.size();
}