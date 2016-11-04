#include "stdafx.h"
#include <fstream>
#include <cassert>
#include "GameData.h"
#include "../Object Manager/GameObject.h"
#include "../TinyXml/tinyxml.h"
#include "../TinyXml/tinystr.h"

#include <thread>

GameData::GameData()
{
}


GameData::~GameData()
{
}



// RETURN VALUE TABLE
//  0 : Nothing out of the ordinary
int GameData::Initialize()
{

	// Gameplay items
	//LoadWeaponWhip();
	//LoadEngine();

	//// Score
	m_score_current = 0;
	m_score_total = 0;
	m_pylons_alive = 3;
	//m_score_multiplier = 1.0f;

	//m_PlayerShip.myEngine = m_Engine;
	//m_PlayerShip.myWeapon[0] = m_Whip;
	m_Whip.bullet_type = eOBJECT_TYPE::Bullet_Whip;
	m_Whip.dmg_base = 8.0f;
	m_Whip.dmg_upgrade_max = 5;
	m_Whip.dmg_upgrade_rate = 2.0f;
	m_Whip.myDamage = m_Whip.dmg_base;
	m_Whip.fire_rate_base = 0.055f;
	m_Whip.fire_rate_upgrade_max = 5;
	m_Whip.fire_rate_upgrade = 0.01f;
	m_Whip.myFireRate = m_Whip.fire_rate_base;
	m_Whip.iconRenderStr = "iconWhipGun";

	m_Spread.bullet_type = eOBJECT_TYPE::Bullet_Spread;
	m_Spread.dmg_base = 8.0f;
	m_Spread.dmg_upgrade_max = 5;
	m_Spread.dmg_upgrade_rate = 1.0f;
	m_Spread.myDamage = m_Spread.dmg_base;
	m_Spread.fire_rate_base = 0.2f;
	m_Spread.fire_rate_upgrade_max = 5;
	m_Spread.fire_rate_upgrade = 0.005f;
	m_Spread.myFireRate = m_Spread.fire_rate_base;
	m_Spread.iconRenderStr = "iconSpreadGun";

	m_Missile.bullet_type = eOBJECT_TYPE::Bullet_Missile;
	m_Missile.dmg_base = 20.0f;
	m_Missile.dmg_upgrade_max = 5;
	m_Missile.dmg_upgrade_rate = 5.0f;
	m_Missile.myDamage = m_Missile.dmg_base;
	m_Missile.fire_rate_base = 0.4f;
	m_Missile.fire_rate_upgrade_max = 5;
	m_Missile.fire_rate_upgrade = 0.01f;
	m_Missile.myFireRate = m_Missile.fire_rate_base;
	m_Missile.iconRenderStr = "iconMissileGun";

	m_PlayerShip.myWeapon[0] = &m_Spread;
	m_PlayerShip.myWeapon[1] = &m_Whip;
	m_PlayerShip.myWeapon[2] = &m_Missile;

	m_bResetGame = false;
	m_bPlayerDead = false;
	m_bUsingGamepad = false;
	m_bAllowVibration = true;

	//LoadData(); // Called when levels are loaded

	// Temp upgrade points
	m_upgrade_points = 1;
	m_vScoreValues.resize( eOBJECT_TYPE::Player_Obj );
	SetupScores();
	ResetScore();


	return 0;
}

void GameData::SetupScores()
{
	m_vScoreValues[Enemy_Bomber] = 10;
	m_vScoreValues[Enemy_Merc] = 15;
	m_vScoreValues[Enemy_Koi] = 5;
	m_vScoreValues[Enemy_Disruptor] = 5;
	m_vScoreValues[Enemy_Dodger] = 10;

	// Goliath will probably want special handling for how strong it is
	m_vScoreValues[Enemy_Goliath] = 40;
	m_vScoreValues[Enemy_MineLayer] = 25;
}


// int return type for information on what happens in here
//  0 : Default Settings, no save to load
//  1 : Successful load from file
// -1 : Failed data read from file
int GameData::LoadEngine()
{
	// TODO: Check for save data to load
	if ( false )
	{
		// Open and read
	}

	// Default Engine Values
	else
	{
		m_Engine.emp_base_recharge_time = 20.0f;
		m_Engine.emp_recharge_time_upgrade = -1.5f;
		m_Engine.emp_upgrade_max = 10;
		m_Engine.emp_upgrade_num = 0;

		m_Engine.speed_base = 1.0f;
		m_Engine.speed_upgrade_value = 0.15f;
		m_Engine.speed_upgrade_max = 10;
		m_Engine.speed_upgrade_num = 0;

		return 0;
	}
}

void GameData::LoadData( std::string file )
{
	m_Level_Information.resize( m_num_levels );

	// Load Save File (...)
	if(file != "")
		assert( file.c_str() != nullptr && file[0] != '\0' && "LevelXML Load File Failed" );

	TiXmlDocument doc;

	if ( doc.LoadFile( file.c_str() ) != false )
	{
		TiXmlElement *pRoot = doc.RootElement();

		TiXmlElement *data = pRoot->FirstChildElement();

		// Need to get all attributes per element => element is a GameData (weapon info)
		if ( data != nullptr )
		{
			data->Attribute( "WhipDamageUpgradePoints", &m_Whip.dmg_upgrade_num );
			data->Attribute( "WhipFireRateUpgradePoints", &m_Whip.fire_rate_upgrade_num );
			data->Attribute( "SpreadDamageUpgradePoints", &m_Spread.dmg_upgrade_num );
			data->Attribute( "SpreadFireRateUpgradePoints", &m_Spread.fire_rate_upgrade_num );
			data->Attribute( "MissileDamageUpgradePoints", &m_Missile.dmg_upgrade_num );
			data->Attribute( "MissileFireRateUpgradePoints", &m_Missile.fire_rate_upgrade_num );
			data->Attribute( "NumUpgradePoints", &m_upgrade_points);

			TiXmlElement *levelData = data->NextSiblingElement();
			TiXmlElement *level = levelData->FirstChildElement();

			unsigned int index = 0;

			while ( level != nullptr )
			{
				// Do the levely things
				int buul;
				level->Attribute( "Score", &m_Level_Information[index].high_score );
				level->Attribute( "Star1", &buul );

				// Screw you tinyxml
				if ( buul )
				{
					m_Level_Information[index].star_1 = true;
				}
				else
				{
					m_Level_Information[index].star_1 = false;
				}

				level->Attribute( "Star2", &buul );
				if ( buul )
				{
					m_Level_Information[index].star_2 = true;
				}
				else
				{
					m_Level_Information[index].star_2 = false;
				}

				level->Attribute( "Star3", &buul );
				if ( buul )
				{
					m_Level_Information[index].star_3 = true;
				}
				else
				{
					m_Level_Information[index].star_3 = false;
				}

				level->Attribute( "Unlocked", &buul );
				if ( buul )
				{
					m_Level_Information[index].unlocked = true;
				}
				else
				{
					m_Level_Information[index].unlocked = false;
				}

				index++;
				level = level->NextSiblingElement();
			}
		}

	}

	else
		// Default information (testing until save is finished)
	{
		for ( unsigned int i = 0; i < m_num_levels; i++ )
		{
			m_Level_Information[i].high_score = 0;
			m_Level_Information[i].index = i;
			m_Level_Information[i].star_1 = false;
			m_Level_Information[i].star_2 = false;
			m_Level_Information[i].star_3 = false;
			m_Level_Information[i].unlocked = false;
		}
	}

	// First level always unlocked
	m_Level_Information[0].unlocked = true;
}

bool GameData::Update( float dt )
{
	// Only tick down if there is a current timer
	if ( m_mult_timer > 0.0f )
	{
		m_mult_timer -= dt;
		return false;
	}

	// Check to see if the timer dropped below zero
	// Reset multiplier, add temp tally to score
	if ( m_mult_timer < 0.0f )
	{
		m_mult_timer = 0.0f;
		m_score_current += m_tally_score;
		m_tally_score = 0;
		m_score_multiplier = 1.0f;
		return true;	// Returns true to let gameplay know to update score
	}

	// Default case where timer == 0.0f (do nothing on return)
	return false;
}

void GameData::UpdateScore( unsigned int val )
{
	m_tally_score += (unsigned int)(val * m_score_multiplier);
}

float GameData::GetDamage()
{
	// dmg was made for debug purposes, no need for it really
	float dmg;
	dmg = m_PlayerShip.GetDamage() * m_PlayerShip.dmgScalar;
	return dmg;
}

void GameData::ResetScore()
{
	m_score_multiplier = 1.0f;
	m_score_current = 0;
	m_mult_timer = 0.0f;
	m_tally_score = 0;
	m_level_timer = 0;
	m_pylons_alive = 3;
}

void GameData::KillScore( int enemy )
{
	// Potentially change this to add seconds to timer rather than a flat 5 second set?
	UpdateScore( m_vScoreValues[enemy] );
	m_score_multiplier += 1.0f;
	m_mult_timer += 2.0f;

	if (m_mult_timer > 4.0f)
	{
		m_mult_timer = 4.0f;
	}
}

void GameData::LevelSave()
{
	// Update Score
	if(m_score_current > m_Level_Information[m_cur_level].high_score)
	{
		m_Level_Information[m_cur_level].high_score = m_score_current;
	}

	// TODO: Update Time (add this to things)
	if(m_cur_level+1 < m_num_levels)
		m_Level_Information[m_cur_level+1].unlocked = true;
}

bool GameData::AchieveStar(int star)
{
	// No such star
	if(star < 1 || star > 3) return false;
	
	if(star == 1)
	{
		// Already earned this star
		if( m_Level_Information[m_cur_level].star_1 == true)
			return false;

		// Else yay new star!
		m_Level_Information[m_cur_level].star_1 = true;
		return true;
	}

	else if(star == 2)
	{
		if( m_Level_Information[m_cur_level].star_2 == true)
			return false;

		m_Level_Information[m_cur_level].star_2 = true;
		return true;
	}

	else if(star == 3)
	{
		if( m_Level_Information[m_cur_level].star_3 == true)
			return false;

		m_Level_Information[m_cur_level].star_3 = true;
		return true;
	}

	// Probably shouldn't be able to hit this return
	return false;
}

void GameData::ResetGame()
{
	m_Whip.dmg_upgrade_num = 0;
	m_Spread.dmg_upgrade_num = 0;
	m_Whip.fire_rate_upgrade_num = 0;
	m_Spread.fire_rate_upgrade_num = 0;
	m_Missile.dmg_upgrade_num = 0;
	m_Missile.fire_rate_upgrade_num = 0;
	m_upgrade_points = 1;

	LoadData("");
}

void GameData::SaveGame(void)
{
	std::thread save(&GameData::SaveThread, this);
	save.detach();
	
}

void GameData::SaveThread()
{
	TiXmlDocument doc;

	TiXmlDeclaration* pDel = new TiXmlDeclaration{ "1.0", "utf-8", "" };

	doc.LinkEndChild(pDel);

	TiXmlElement* pRoot = new TiXmlElement{ "Game" };

	doc.LinkEndChild(pRoot);

	TiXmlElement* pGameData = new TiXmlElement{ "GameData" };

	pRoot->LinkEndChild(pGameData);
	
	pGameData->SetAttribute("WhipDamageUpgradePoints", m_Whip.dmg_upgrade_num);
	pGameData->SetAttribute("WhipFireRateUpgradePoints", m_Whip.fire_rate_upgrade_num);
	pGameData->SetAttribute("SpreadDamageUpgradePoints", m_Spread.dmg_upgrade_num);
	pGameData->SetAttribute("SpreadFireRateUpgradePoints", m_Spread.fire_rate_upgrade_num);
	pGameData->SetAttribute("MissileDamageUpgradePoints", m_Missile.dmg_upgrade_num);
	pGameData->SetAttribute("MissileFireRateUpgradePoints", m_Missile.fire_rate_upgrade_num);
	pGameData->SetAttribute("NumUpgradePoints", m_upgrade_points);

	TiXmlElement* pLevelData = new TiXmlElement{ "LevelData" };
	pRoot->LinkEndChild(pLevelData);


	for (unsigned int i = 0; i < m_Level_Information.size(); i++)
	{
		TiXmlElement* pLevel = new TiXmlElement{ "Level" };
		pLevelData->LinkEndChild(pLevel);
		pLevel->SetAttribute("Score", m_Level_Information[i].high_score);
		pLevel->SetAttribute("Star1", m_Level_Information[i].star_1);
		pLevel->SetAttribute("Star2", m_Level_Information[i].star_2);
		pLevel->SetAttribute("Star3", m_Level_Information[i].star_3);
		pLevel->SetAttribute("Unlocked", m_Level_Information[i].unlocked);

	}

	doc.SaveFile("Assets/XML/Slot1Save.xml");
}