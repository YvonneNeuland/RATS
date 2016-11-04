#pragma once
#include <vector>
#include "PlayerData.h"
#include "../States/BaseState.h"


class GameData
{

public:

	GameData();
	~GameData();

	/*****   PUBLIC METHODS   *****/

	// Loads relevant data from file, or setup default values
	int Initialize();

	// Adds score based on an enemy kill, resets multiplier timer, adds 1 to multiplier
	void KillScore(int enemy);

	// Adds val to current_score (for any other reason we might add score)
	void UpdateScore(unsigned int val);

	// Returns currently selected weapon's damage
	float GetDamage();

	// Readies scores for new game/level
	void ResetScore();

	// Save Information after winning Level
	void LevelSave();

	// Mostly for handling timer on multiplier
	// Returns true if tally score was reset
	bool Update(float dt);
	void LoadData(std::string file = "");
	void SaveGame(void);
	bool AchieveStar(int star);
	void SaveThread();

	// ONLY CALL IF YOU REALLY WANNA DO IT
	void ResetGame();

// private:

public:

	//Hack to let the option menu work in the pause screen
	STATE_TYPE m_OptionReturnState = STATE_TYPE::MAIN_MENU_STATE;

	// Number of levels in the game
	unsigned int m_num_levels = 1;

	/*****   Gameplay Variables   *****/
	//Reset Game
	bool m_bResetGame;

	// Current multiplier for score
	float m_score_multiplier;

	// Timer for current multiplier to last
	float m_mult_timer;

	// Current score actively being earned in the level
	int m_score_current;

	// Temp score earned on this killing spree
	int m_tally_score;

	// Total Score added up by all levels (probably want this)
	int m_score_total;

	// Upgrade Points
	int m_upgrade_points;

	// Pylons Alive?
	unsigned short m_pylons_alive = 3;

	// Timer for the match
	float m_level_timer = 0.0f;

	// Player Alive?
	bool m_bPlayerDead;

	// Keyboard/Mouse or GamePad Bool
	bool m_bUsingGamepad;

	bool m_bAllowVibration;

	std::vector<unsigned int> m_vScoreValues;

	// Level Completion Information for all levels
	std::vector<LevelData> m_Level_Information;

	// Current Level
	unsigned int m_cur_level = 0;
	std::string m_levelShape = "Sphere";
	
	// Ship Configuration Data
	Ship_Data m_PlayerShip;


	// Upgrade Information
	Ship_Weapon m_Whip;		// 1 of these per bullet pattern
	Ship_Weapon m_Spread;
	Ship_Weapon m_Missile;
	Ship_Weapon m_Microwave;

	Ship_Engine m_Engine;	// 1 of these per game (?)

	


private:

	/*****   PRIVATE METHODS   *****/

	// Loads upgrades and level status
	
	// ENGINE SETUP

	// Reads info on save game for Engine
	// TODO: Add file loading, not just default setup (Requires saving);
	int LoadEngine();

	void SetupScores();

	
	
};

