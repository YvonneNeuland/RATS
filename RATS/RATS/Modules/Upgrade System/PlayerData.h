#pragma once
#include <string>

enum eBULLET_PATTERN { bp_Whip, bp_Spread, bp_Missile, bp_MAX };

struct LevelData
{
	// Which level this actually is
	unsigned short index;

	// The highest score achieved on this level
	int high_score;

	// Timer for some day
	float best_time = -1.0f;
	
	// Levels are planned to have a 3-star ranking system, this holds those values
	// Also, if level is unlocked (playable)
	bool star_1, star_2, star_3, unlocked;
};

struct Ship_Weapon
{
	// Number of bullet patterns than this gun can fire
	// Not Done Here Probably
	//unsigned char num_patterns = 1;		// Default 1, maybe always 1?  As many as purchased?

	// Type of bullet_pattern this weapon currently fires 
	// (replace with eOBJECT_TYPE bullet?  Or translate between the two?)
	int bullet_type;
	std::string iconRenderStr;


	/*****   FIRE RATE DATA   *****/

	// Number of bullets fired per second (default)
	float fire_rate_base; 

	// Total number of upgrades possible for fire rate
	int fire_rate_upgrade_max;

	// Amount of fire rate increase
	float fire_rate_upgrade;

	// Number of upgrades to fire rate
	int fire_rate_upgrade_num = 0;

	// Current Fire Rate
	float myFireRate;


	/*****   DAMAGE DATA   *****/

	// Damage per bullet (default)
	float dmg_base;

	// Number of upgrades for damage
	int dmg_upgrade_max = 10;

	// Amount of damage per upgrade
	float dmg_upgrade_rate;

	// Number of upgrades to damage rate
	int dmg_upgrade_num = 0;

	// Current Damage Rate
	float myDamage;



	/*****   METHODS   *****/
	void UpdateMembers()
	{
		myDamage = dmg_base + dmg_upgrade_rate * dmg_upgrade_num;
		myFireRate = fire_rate_base - fire_rate_upgrade_num * fire_rate_upgrade;
	}
	
	// Didn't turn out the way I intended, these are unused.
	// Called when player upgrades damage to a weapon; Protects self from over-upgrade
	bool UpgradeDamage()
	{
		if(dmg_upgrade_num < dmg_upgrade_max)
		{
			dmg_upgrade_num++;
			myDamage = dmg_base + dmg_upgrade_rate * dmg_upgrade_num; 
			return true;
		}
		else
			return false;
	}

	// Upgrades fire rate for this weapon
	bool UpgradeRate()
	{
		if(fire_rate_upgrade_num < fire_rate_upgrade_max)
		{
			fire_rate_upgrade_num++;
			myFireRate = fire_rate_base - fire_rate_upgrade_num * fire_rate_upgrade;
			return true;
		}
		else
			return false;
	}

	

};

struct Ship_Engine
{
	/*****   EMP DATA   *****/

	// EMP type (will we have different ones?)  for now, no
	//int emp_type;

	// Recharge time
	float emp_base_recharge_time;

	// Upgrade value for timer
	float emp_recharge_time_upgrade;

	// Max Upgrades Possible
	int emp_upgrade_max;

	// Number upgrades currently purchased
	int emp_upgrade_num;

	
	/*****   SPEED DATA   *****/

	// Base Speed
	float speed_base;

	// Upgrade Value
	float speed_upgrade_value;

	// Max Upgrades
	int speed_upgrade_max;

	// Current Upgrades
	int speed_upgrade_num;

};


struct Ship_Data
{
	// Ship's currently equipped Weapon(s), as reference
	// Only 1 copy of REAL weapons exist, in GameData
	// TODO: Change to flags or pointers
	 Ship_Weapon *myWeapon[3];

	 // Index of current weapon
	 unsigned short weapon_select = 0;

	 // Num Weapons Possible
	 unsigned short weapon_max = 3;


	 // Health Upgrades
	 int health_upgrade_count = 0;

	 int health_upgrade_max = 10;

	 float health_upgrade_amount = 25.0f;

	 // Possibly starter health to replace in player_obj too


	// Ship's currently installed Engine
	// Probably a pointer to the Engine, or the only place the engine lives
	// Part of Design timeframe for Upgrade System
	Ship_Engine *myEngine;

	float dmgScalar = 1;
	float resilienceScalar = 1;
	float speedScaler = 1;

	/*****   METHODS   *****/

	// Cycles available bullet patterns
	void SwitchWeapon()		// TODO: Write this function after setting up multi-types	
	{
		weapon_select++;

		if(weapon_select == weapon_max)
			weapon_select = 0;
	}

	void SwitchWeapon(int index)
	{
		if (index < 0 || index >= weapon_max)
			return;

		weapon_select = index;
	}

	float GetDamage() { return myWeapon[weapon_select]->myDamage; };



};