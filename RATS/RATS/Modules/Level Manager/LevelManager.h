#pragma once

#include "Level.h"

#include <map>
using namespace std;

class LevelManager
{
public:
	LevelManager();
	~LevelManager();

	bool LoadLevelFile(string filePath);
	void Terminate(void);
	Level* GetLevel(int numLevel);	
	unsigned int GetNumLevels(void);

private:
	map<int, Level*> m_mLoadedLevels;

};

