#pragma once

// STATS AND ACHIEVEMENT MANAGER
// BASED OFF OF STEAM SDK DOCS


#include "../../Dependencies/Paramesium/Include/steam_api.h"

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }
#define _STAT_ID( id,type,name ) { id, type, name, 0, 0, 0, 0 }


enum EStatTypes
{
	STAT_INT = 0,
	STAT_FLOAT = 1,
	STAT_AVGRATE = 2,
};

struct Stat_t
{
	int m_ID;
	EStatTypes m_eStatType;
	const char *m_pchStatName;
	int m_iValue;
	float m_flValue;
	float m_flAvgNumerator;
	float m_flAvgDenominator;
};

struct Achievement_t
{
	int m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};


// Defining our achievements
enum EAchievements
{
	ACH_ONE_WEAPON_MAX = 0,
	ACH_BEAT_BOSS_ONE = 1,
	ACH_BEAT_ALL_LEVELS = 2,
	ACH_ACE_ALL_LEVELS = 3,
	ACH_DIE_FIRST_LEVEL,
	ACH_USE_DEM_HAZARDS,
	ACH_SPREADSHOT_BOMBER,
	ACH_HOMINGMISSILE_40,
	ACH_WHIP_100
};

enum EStats
{
	STAT_STARS_UNLOCKED = 0,
	STAT_BOSSES_ACED,
	STAT_ENV_HAZ_KILLS,
	STAT_BOMBER_SPREAD_KILLS,
	STAT_DODGER_MISSILE_KILLS,
	STAT_MINE_WHIP_KILLS
};

// Achievement array which will hold data about the achievements and their state
static Achievement_t g_Achievements[] =
{
	_ACH_ID(ACH_ONE_WEAPON_MAX, "Playin' Favorites"),								// done
	_ACH_ID(ACH_BEAT_BOSS_ONE, "This STAR ain't big enough for the two of us..."),	// done
	_ACH_ID(ACH_BEAT_ALL_LEVELS, "Guardian of the Galaxy"),							// done
	_ACH_ID(ACH_ACE_ALL_LEVELS, "Completionist"),									// done
	_ACH_ID(ACH_DIE_FIRST_LEVEL, "This is going to take a while..."),				// done
	_ACH_ID(ACH_USE_DEM_HAZARDS, "Master of the Elements"),							// done ?
	_ACH_ID(ACH_SPREADSHOT_BOMBER, "Spray N' Pray"),								// done	?
	_ACH_ID(ACH_HOMINGMISSILE_40, "Dodge this!"),									// done	?
	_ACH_ID(ACH_WHIP_100, "Whip it good")											// done	?
};

static Stat_t g_Stats[] =
{
	_STAT_ID(2, STAT_INT, "STAT_STARS_UNLOCKED"),
	_STAT_ID(3, STAT_INT, "STAT_BOSSES_ACED"),
	_STAT_ID(4, STAT_INT, "STAT_ENV_HAZ_KILLS"),
	_STAT_ID(5, STAT_INT, "STAT_BOMBER_SPREAD_KILLS"),
	_STAT_ID(6, STAT_INT, "STAT_DODGER_MISSILE_KILLS"),
	_STAT_ID(7, STAT_INT, "STAT_MINE_WHIP_KILLS")

};

#define ENV_HAZ_KILLS_TOTAL 150
#define BOMBER_KILLS_TOTAL 250
#define DODGER_KILLS_TOTAL 150
#define MINE_KILLS_TOTAL 200

class CAchManager
{

	// yolo hack this is due in a fucking week
public:
	int64 m_iAppID; // Our current AppID
	Achievement_t *m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	Stat_t *m_pStats; // Stats data
	int m_iNumStats; // The number of Stats
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	CAchManager(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats);
	~CAchManager();

	bool RequestStats();
	bool StoreStats();
	bool SetAchievement(const char* ID);

	void ResetAllValues();

	STEAM_CALLBACK(CAchManager, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CAchManager, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(CAchManager, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

};

