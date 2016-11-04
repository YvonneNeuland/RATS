#pragma once

#include <vector>
using namespace std;

#include "../Object Manager/GameObject.h"

struct EnemyTypeInfo
{
	int numEnemy;
	eOBJECT_TYPE enemyType;
};

class Wave
{
public:
	Wave();
	~Wave();

	vector<EnemyTypeInfo*>& GetEnemyTypeInfo(void);
	void Terminate(void);
	vector<EnemyTypeInfo*>& GetLoadedEnemyInfoVector(void){ return m_vLoadedEnemyInfos; }
	void SetNumWave(int numWave){ m_nNumWave = numWave; }

private:
	vector<EnemyTypeInfo*> m_vLoadedEnemyInfos;
	int m_nNumWave;

};

