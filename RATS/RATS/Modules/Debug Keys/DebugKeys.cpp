#include "stdafx.h"
#include "DebugKeys.h"

#include "../States/GamePlayState.h"
#include "../States/UpgradeShipState.h"

#include "../Components/HealthComponent.h"

#include "../Upgrade System/GameData.h"
#include "../Audio/FFmod.h"
#include "../Achievements/AchManager.h"
#include "../../Dependencies/Paramesium/Include/steam_api.h"
extern GameData* gameData;
extern	FFmod* g_AudioSystem;
extern CAchManager* g_SteamAchievements;


using namespace Events;
DebugListener::DebugListener()
{
	InitDebugFuncs();
	ResetFlags();
}

DebugListener::~DebugListener()
{
	debugFuncs.clear();
}

void DebugListener::SetFlags(unsigned char _newFlags)
{
	if (_newFlags == m_flags) return;

	if (m_flags != 0)
		ClearClients();

	m_flags = _newFlags;

	SetupClients();
}

void DebugListener::HandleKeyDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	unsigned char key = toupper(args.GetData());

	if (key == 0x11) // VK_CONTROL
	{
		if (firstCtrlDown == 0)
			firstCtrlDown = 1;
		else
			ResetFlags();
	}
	else if (isalpha(key))
	{
		if (key == 'D')
		{
			if (firstCtrlDown == 1 && listening == 0)
				listening = 1;
			else
				ResetFlags();
		}
		else
		{
			if (listening == 1)
			{
				if (/*key == 'P' ||*/ key == 'E' || key == 'B')
				{
					lastCharDown = key;
					needsNumber = 1;
				}
				else if (debugFuncs.find(key) != debugFuncs.end())
				{
					RunDebugFunc(key);
					ResetFlags();
				}
			}
		}
	}
	else if (isalnum(key))
	{
		if (needsNumber == 1)
		{
			RunDebugFunc(lastCharDown, atoi((char*)&key));
		}

		ResetFlags();
	}
}


#pragma region Debug Functions

void DebugListener::ToggleGodmode(int dummy)
{
	if (!(m_flags & DebugListenFor::GodMode)) return;

	if (gameplayOwner->m_ObjectManager.GetPlayer() == nullptr) return;

	HealthComponent* playerHealth = gameplayOwner->m_ObjectManager.GetPlayer()->GetComponent<HealthComponent>();

	if (playerHealth == nullptr) return;

	inGodMode = !inGodMode;
	if (inGodMode)
	{
		lastPlayerMaxHealth = playerHealth->GetMaxHealth();
		playerHealth->SetMaxHealth(10000000);
	}
	else
		playerHealth->SetMaxHealth(lastPlayerMaxHealth);

	g_AudioSystem->PlaySound("MENU_Accept");
}

void DebugListener::FillEnergy(int dummy)
{
	if (!(m_flags & DebugListenFor::FillEnergy)) return;

	if (gameplayOwner->m_ObjectManager.GetPlayer() == nullptr) return;

	HealthComponent* playerHealth = gameplayOwner->m_ObjectManager.GetPlayer()->GetComponent<HealthComponent>();

	if (playerHealth == nullptr) return;

	playerHealth->SetEnergy(playerHealth->GetMaxEnergy());

	g_AudioSystem->PlaySound("MENU_Accept");
}

void DebugListener::NextWave(int dummy)
{
	if (!(m_flags & DebugListenFor::SkipWave)) return;

	vector<list<GameObject*>>& buckets = gameplayOwner->m_ObjectManager.PassBuckets();

	int currbucket = Enemy_Merc;
	list<GameObject*>::iterator currEnemy;

	while (currbucket != Player_Obj)
	{
		currEnemy = buckets[currbucket].begin();

		while (currEnemy != buckets[currbucket].end())
		{
			(*currEnemy)->SetDead();
			++currEnemy;
		}

		++currbucket;
	}

	g_AudioSystem->PlaySound("MENU_Accept");
}

//void DebugListener::KillPylon(int pylon)
//{
//#define NUMPYLONS 3
//	if (pylon > NUMPYLONS || drainPylon) return;
//
//	GameObject* allPylons[3] = { owner->m_Pylon1, owner->m_Pylon2, owner->m_Pylon3 };
//
//	pyHealth = allPylons[pylon - 1]->GetComponent<HealthComponent>();
//
//	drainPylon = true;
//}

void DebugListener::SpawnEnemy(int enemyType)
{
	if (!(m_flags & DebugListenFor::SpawnBad)) return;

#define NUMENEMIES 9
	if (enemyType > NUMENEMIES) return;

	string enemyIDs[] =
	{
		/*m_vRenderIDs[Enemy_Merc] =		*/	"Enemy",
		/*m_vRenderIDs[Enemy_Bomber] =	*/		"Bomber",
		/*m_vRenderIDs[Enemy_Koi] =		*/		"Koi A",
		/*m_vRenderIDs[Enemy_Disruptor] =	*/	"Koi D",
		/*m_vRenderIDs[Enemy_Dodger] =	*/		"Dodger",
		/*m_vRenderIDs[Enemy_MineLayer] =	*/	"Mine Layer",
		/*m_vRenderIDs[Enemy_Goliath] =	*/		"Goliath",
		/*m_vRenderIDs[Enemy_Boss] =		*/	"Boss",
		/*m_vRenderIDs[Enemy_FenceLayer] =*/	"Fence Layer"
	};


	++gameplayOwner->m_unNumEnemiesToKill;

	gameplayOwner->m_ObjectManager.CreateObject((eOBJECT_TYPE)enemyType, XMFLOAT3(0, 0, 0), enemyIDs[enemyType - 1], eRENDERCOMP_TYPE::Opaque);

	g_AudioSystem->PlaySound("MENU_Accept");
}

void DebugListener::SpawnPowerup(int powerupType)
{
	if (!(m_flags & DebugListenFor::SpawnGood)) return;

#define NUMPOWERUPS 3
	if (powerupType > NUMPOWERUPS) return;

	struct powerupInfo
	{
		eOBJECT_TYPE objType;
		string renderID;
	};

	powerupInfo powerupTypes[] = 
	{
		{ eOBJECT_TYPE::Orb_Blue, "Energy" },
		{ eOBJECT_TYPE::Pickup_Health, "Pickup Health" },
		{ eOBJECT_TYPE::Pickup_Shield, "Pickup Shield" }

	};

	default_random_engine randEng;
	uniform_real_distribution<float> randDistrib(-1,1);

	XMFLOAT3 finalpos;

	XMStoreFloat3(&finalpos, XMVector3Normalize(XMLoadFloat3(&XMFLOAT3(randDistrib(randEng), randDistrib(randEng), randDistrib(randEng)))) * 75);

	gameplayOwner->m_ObjectManager.CreateObject(powerupTypes[powerupType - 1].objType, finalpos, powerupTypes[powerupType - 1].renderID, eRENDERCOMP_TYPE::Opaque);

	g_AudioSystem->PlaySound("MENU_Accept");
}

void DebugListener::MaxCredits(int dummy)
{
	if (!(m_flags & DebugListenFor::MaxCreds)) return;

#define NUMWEPS 3
	upgradeOwner->starting_points = gameData->m_upgrade_points = 8 * NUMWEPS;
	upgradeOwner->UpdatePoints();

	g_AudioSystem->PlaySound("MENU_Accept");
}


void DebugListener::UnlockAllLevels(int dummy /*= 0*/)
{
	if (!(m_flags & DebugListenFor::AllLevels))
		return;

	for (UINT i = 0; i < gameData->m_Level_Information.size(); i++)
	{
		gameData->m_Level_Information[i].unlocked = true;
	}

	g_AudioSystem->PlaySound("MENU_Accept");
}


void DebugListener::ResetSteamAchievements(int dummy /*= 0*/)
{
	if (!(m_flags & DebugListenFor::ResetAchievements))
		return;

	SteamUserStats()->ResetAllStats(true);

	g_SteamAchievements->RequestStats();

	std::cout << "User Achievements Reset\n";

	g_AudioSystem->PlaySound("MENU_Accept");

}

#pragma endregion


void DebugListener::InitDebugFuncs()
{
	debugFuncs['G'] = &DebugListener::ToggleGodmode;
	debugFuncs['F'] = &DebugListener::FillEnergy;
	debugFuncs['N'] = &DebugListener::NextWave;
	debugFuncs['O'] = &DebugListener::ResetSteamAchievements;
	debugFuncs['E'] = &DebugListener::SpawnEnemy;   // These two 
	debugFuncs['B'] = &DebugListener::SpawnPowerup; // need a number
	debugFuncs['C'] = &DebugListener::MaxCredits;
	debugFuncs['L'] = &DebugListener::UnlockAllLevels;
}

void DebugListener::SetupClients()
{
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebugCtrl"), this, &DebugListener::HandleKeyDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebugConfirm"), this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::GodMode)
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_G"), this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::FillEnergy)
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_F"), this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::SkipWave)
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_N"), this, &DebugListener::HandleKeyDown);

	//EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_P"), this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::SpawnBad)
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_E"), this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::SpawnGood)
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_B"), this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::MaxCreds)
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_C"), this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::AllLevels)
	{
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_L"), this, &DebugListener::HandleKeyDown);
	}

	if (m_flags & (DebugListenFor::SpawnBad | DebugListenFor::SpawnGood))
	{
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_1"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_2"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_3"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_4"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_5"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_6"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_7"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_8"), this, &DebugListener::HandleKeyDown);
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_9"), this, &DebugListener::HandleKeyDown);
		//EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_0"), this, &DebugListener::HandleKeyDown);
	}

	if (m_flags & DebugListenFor::ResetAchievements)
		EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DownDebug_O"), this, &DebugListener::HandleKeyDown);
}

void DebugListener::ClearClients()
{
	EventManager()->UnregisterClient("DownDebugCtrl", this, &DebugListener::HandleKeyDown);
	EventManager()->UnregisterClient("DownDebugConfirm", this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::GodMode)
		EventManager()->UnregisterClient("DownDebug_G", this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::FillEnergy)
		EventManager()->UnregisterClient("DownDebug_F", this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::SkipWave)
		EventManager()->UnregisterClient("DownDebug_N", this, &DebugListener::HandleKeyDown);

	//EventManager()->UnregisterClient("DownDebug_P", this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::SpawnBad)
		EventManager()->UnregisterClient("DownDebug_E", this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::SpawnGood)
		EventManager()->UnregisterClient("DownDebug_B", this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::MaxCreds)
		EventManager()->UnregisterClient("DownDebug_C", this, &DebugListener::HandleKeyDown);

	if (m_flags & DebugListenFor::AllLevels)
		EventManager()->UnregisterClient("DownDebug_L", this, &DebugListener::HandleKeyDown);

	if (m_flags & (DebugListenFor::SpawnBad | DebugListenFor::SpawnGood))
	{
		EventManager()->UnregisterClient("DownDebug_1", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_2", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_3", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_4", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_5", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_6", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_7", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_8", this, &DebugListener::HandleKeyDown);
		EventManager()->UnregisterClient("DownDebug_9", this, &DebugListener::HandleKeyDown);
		//EventManager()->UnregisterClient("DownDebug_0", this, &DebugListener::HandleKeyDown);
	}

	if (m_flags & DebugListenFor::ResetAchievements)
		EventManager()->UnregisterClient("DownDebug_O", this, &DebugListener::HandleKeyDown);

	m_flags = 0;
}

void DebugListener::Update(float dt)
{

	//if (drainPylon)
	//{
	//	if (pyHealth != nullptr)
	//	{
	//		pyHealth->SetHealth(pyHealth->GetHealth() - (100 * dt));

	//		if (pyHealth->GetHealth() <= 0)
	//		{
	//			pyHealth->SetHealth(0);
	//			pyHealth = nullptr;
	//			drainPylon = false;
	//		}
	//	}
	//	else
	//		drainPylon = false;
	//}

}


