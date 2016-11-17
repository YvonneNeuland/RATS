#pragma once

#include <unordered_map>
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

class GamePlayState;
class UpgradeShipState;
class HealthComponent;

enum DebugListenFor
{
	GodMode				= 0x01,		//0b 0000 0001
	FillEnergy			= 0x02,		//0b 0000 0010
	SkipWave			= 0x04,		//0b 0000 0100
	SpawnBad			= 0x08,		//0b 0000 1000
	SpawnGood			= 0x10,		//0b 0001 0000
	MaxCreds			= 0x20,		//0b 0010 0000
	AllLevels			= 0x40,		//0b 0100 0000
	ResetAchievements	= 0x80,		//0B 1000 0000

	PlayFlags			= 0x1F		//0b 0001 1111

};

class DebugListener
{
public:

	DebugListener();
	~DebugListener();

	void Update(float dt);

	void HandleKeyDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);

	void SetupClients();
	void ClearClients();

	void SetOwner(GamePlayState* newowner) { gameplayOwner = newowner; }
	void SetOwner(UpgradeShipState* newowner) { upgradeOwner = newowner; }

	void SetFlags(unsigned char _newFlags);
private:
	unsigned char m_flags = 0;

	GamePlayState* gameplayOwner;
	UpgradeShipState* upgradeOwner;

	unsigned char firstCtrlDown : 1, listening : 1, needsNumber : 1, pad : 5;
	unsigned char lastCharDown;

	bool inGodMode = false;
	float lastPlayerMaxHealth;

	//bool drainPylon = false;
	//HealthComponent* pyHealth = nullptr;

	typedef void(DebugListener::*DebugKeyFunc)(int);

	void ToggleGodmode(int dummy = 0);
	void FillEnergy(int dummy = 0);
	void NextWave(int dummy = 0);
	//void KillPylon(int pylon);
	void SpawnEnemy(int enemyType);
	void SpawnPowerup(int powerupType);
	void MaxCredits(int dummy = 0);
	void UnlockAllLevels(int dummy = 0);
	void ResetSteamAchievements(int dummy = 0);

	std::unordered_map<unsigned char, DebugKeyFunc> debugFuncs;

	void InitDebugFuncs();

	void ResetFlags() { firstCtrlDown = listening = needsNumber = 0; }
	void RunDebugFunc(unsigned char key, int args = 0) { (this->*debugFuncs[key])(args); }

};