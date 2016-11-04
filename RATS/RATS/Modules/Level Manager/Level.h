#pragma once
#include "Wave.h"

#include <string>
#include <vector>
using namespace std;

class Level
{
public:
	Level();
	~Level();

	void Terminate(void);	

	void SetLevelNum(int numLevel){ m_nNumLevel = numLevel; }
	const int GetNumLevel(void){ return m_nNumLevel; }

	vector<Wave*>& GetLoadedWaveVector(void){ return m_vLoadedWaves; }
	Wave* GetWave(unsigned int numWave);
	unsigned int GetNumWaves(void);
	

	const string GetLevelShape(void) { return m_szLevelShape; }
	void SetLevelShape(string levelShape) { m_szLevelShape = levelShape; }

private:
	vector<Wave*> m_vLoadedWaves;
	unsigned int m_nNumLevel;
	string m_szLevelShape = "";
	
};

