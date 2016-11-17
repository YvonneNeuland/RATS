#include "stdafx.h"
#include "AchManager.h"




CAchManager::~CAchManager()
{
}

// Constructor
// Parameters - The constructor takes a pointer to an array of achievements along with the length of the array.
//              The formating of that array will be covered in the main game code later.
// Returns - N / A
// What it does - The constructor initializes a number of members along with grabbing the AppID we are currently running as.
//                In addition it hooks up the call back methods to handle asynchronous calls made to Steam.
//                Finally it makes an initial call to RequestStats() to get stats and achievements for the current user.
CAchManager::CAchManager(Achievement_t *Achievements, int NumAchievements) :
m_iAppID(0),
m_bInitialized(false),
m_CallbackUserStatsReceived(this, &CAchManager::OnUserStatsReceived),
m_CallbackUserStatsStored(this, &CAchManager::OnUserStatsStored),
m_CallbackAchievementStored(this, &CAchManager::OnAchievementStored)
{
	m_iAppID = SteamUtils()->GetAppID();
	m_pAchievements = Achievements;
	m_iNumAchievements = NumAchievements;
	RequestStats();
}



//RequestStats()
// Parameters - None
// Returns - a bool representing if the call succeeded or not.
//           If the call failed then most likely Steam is not initialized.
//           Make sure you have a steam client open when you try to make this call and that SteamAPI_Init() has been called before it.
// What it does - This method basically wraps a call to SteamUserStats()->RequestCurrentStats() that is an asynchronous call to steam 
//                requesting the stats of the current user.  This call needs to be made before you can set any stats or achievements.
//                The initial call to this method is made in the constructor.
//                You can call it again any time after that if you want to check on updated stats or achievements.
bool CAchManager::RequestStats()
{
	// Is Steam loaded? If not we can't get stats.
	if (NULL == SteamUserStats() || NULL == SteamUser())
	{
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}


//SetAchievement()
//Parameters - The string identifier of the Achievement that you want to set(ie. "ACH_WIN_ONE_GAME")
//Returns - a bool representing if the call succeeded or not.
//          If the call failed then either Steam is not initialized or you still haven't processed the callback from the inital call to RequestStats().
//          You can't set any achievements until that callback has been received.
//What it does - This method sets a given achievement to achieved and sends the results to Steam.
//               You can set a given achievement multiple times so you don't need to worry about only setting achievements that aren't already set.
//               This is an asynchronous call which will trigger two callbacks : OnUserStatsStored() and OnAchievementStored().

bool CAchManager::SetAchievement(const char* ID)
{
	// Have we received a call back from Steam yet?
	if (m_bInitialized)
	{
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	// If not then we can't set achievements yet
	return false;
}



//OnUserStatsReceived()
//Parameters - N / A
//Returns - Nothing
//What it does - This method is a callback that is called anytime you attempt to request stats.
//               Stats are requested by using RequestStats().
//               The method updates the member variable m_Achievements to reflect the latest achievement data returned from Steam.

void CAchManager::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			std::cout << "Received stats and achievements from Steam\n";
			m_bInitialized = true;

			// load achievements
			for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
			{
				Achievement_t &ach = m_pAchievements[iAch];

				SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
				_snprintf(ach.m_rgchName, sizeof(ach.m_rgchName), "%s", SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "name"));
				_snprintf(ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s", SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "desc"));
			}
		}
		else
		{
			char buffer[128];
			_snprintf(buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult);
			std::cout << buffer;
		}
	}
}



//OnUserStatsStored()
//Parameters - N / A
//Returns - Nothing
//What it does - This method is a callback that is called anytime you attempt to store stats on Steam.

void CAchManager::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			std::cout << "Stored stats for Steam\n";
		}
		else
		{
			char buffer[128];
			_snprintf(buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult);
			std::cout << buffer;
		}
	}
}



//OnAchievementStored()
//Parameters - N / A
//Returns - Nothing
//What it does - This method is a callback that is called anytime Achievements are successfully stored on Steam.

void CAchManager::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == pCallback->m_nGameID)
	{
		std::cout << "Stored Achievement for Steam\n";
	}
}
