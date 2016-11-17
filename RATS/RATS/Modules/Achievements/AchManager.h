#pragma once

#include "../../Dependencies/Paramesium/Include/steam_api.h"

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }
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
	ACH_TAKE_NO_DMG,
	ACH_USE_DEM_HAZARDS,
	ACH_SPREADSHOT_BOMBER,
	ACH_HOMINGMISSILE_40,
	ACH_WHIP_75
};

// Achievement array which will hold data about the achievements and their state
static Achievement_t g_Achievements[] =
{
	_ACH_ID(ACH_ONE_WEAPON_MAX, "Playin' Favorites"),
	_ACH_ID(ACH_BEAT_BOSS_ONE, "This STAR ain't big enough for the two of us..."),
	_ACH_ID(ACH_BEAT_ALL_LEVELS, "Guardian of the Galaxy"),
	_ACH_ID(ACH_ACE_ALL_LEVELS, "Completionist"),
	_ACH_ID(ACH_DIE_FIRST_LEVEL, "This is going to take a while..."),
	_ACH_ID(ACH_TAKE_NO_DMG, "Artful Dodger"),
	_ACH_ID(ACH_USE_DEM_HAZARDS, "Master of the Elements"),
	_ACH_ID(ACH_SPREADSHOT_BOMBER, "Spray N' Pray"),
	_ACH_ID(ACH_HOMINGMISSILE_40, "Dodge this!"),
	_ACH_ID(ACH_WHIP_75, "Whip it good")
};


class CAchManager
{

private:
	int64 m_iAppID; // Our current AppID
	Achievement_t *m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	CAchManager(Achievement_t *Achievements, int NumAchievements);
	~CAchManager();

	bool RequestStats();
	bool SetAchievement(const char* ID);

	STEAM_CALLBACK(CAchManager, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CAchManager, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(CAchManager, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

};

