#pragma once
#include "BaseState.h"
#include "../Level Manager/LevelManager.h"
#include "../Collision/CollisionManager.h"

#include "../Debug Keys/DebugKeys.h"
#include <string>
#include <vector>
#include <random>
using namespace std;

class GameObject;
class AIManager;
class ElectricityEffect;

#define NUM_EMPS 4
#define NUM_EMP_ELECS 10

enum SCALAR_TYPE
{
	NONE = 0,
	SPEED = 1,
	RESILIENCE = 2,
	DAMAGE = 3
};

class GamePlayState : public BaseState
{
public:
	GamePlayState();
	~GamePlayState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

	void PauseGame(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void ReturnToMainMenu(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void ExitGameToMainMenu(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void SwitchWeapon(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void SetWeapon1(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void SetWeapon2(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void SetWeapon3(const Events::CGeneralEventArgs2<unsigned char, float>& args);

	void SetupClients();
	void ClearClients();

	void ResetGame();

private:

	friend class DebugListener;
	
	//Level Control functions
	void SpawnEnemiesInWave(void);
	void PauseWaves(float dt);
	void UpdateLevelInfo();
	void CheckForGameLost();
	bool CheckForPylonHealthFull();
	void UpdateGameOver(float dt);
	void UpdateLevelClear(float dt);
	void SpawnFireworks();

	//Environmental Hazard Controls
	float m_fHazardTimer = 0.0f;
	bool m_bHazardWarningPlayed = false;
	bool m_bHazardWarningSpawned = false;
	bool  m_bHazardsSpawned = false;
	Transform m_tEnvHazTransform;
	vector<GameObject*> m_vEnvironmentalHazards;
	void UpdateEnvironmentalHazards(float dt);


	//Save Game Data
	//void SaveGame(void);			// moved to gameData
	int m_unScore = 0;
	int m_unLastScore = 0;

	//HudFunctions
	void CreateHUD(void);
	
	void UpdateHud(float dt);
	void UpdateScore();
	void UpdateTally();
	void UpdatePylon1HUD(float dt);
	void UpdatePylon2HUD(float dt);
	void UpdatePylon3HUD(float dt);
	void UpdatePlayerHUD(float dt);
	void UpdateLevelTimer(float dt);
	void PlayFillPylonMessage(float dt);

	void UpdateBossHud(float dt);
	

	SCALAR_TYPE StatScalarChanged();
	void UpdateScalarMessageHUD(float dt);
	void CreateScalarMessageObjects(void);

	//Object Component Manipulation Functions
	void SetObjectColor(GameObject* obj, XMFLOAT4 color);
	void SetObjectInvincible(GameObject* obj, bool invincible);
	float GetHealth(GameObject* obj);
	float GetMaxHealth(GameObject* obj);
	float GetEnergy(GameObject* obj);
	float GetMaxEnergy(GameObject* obj);

	
	void CreatePylons();
	float SetPylonPosSphere();
	float SetPylonPosTorus();
	float SetPylonPosCube();

	void CreateTornado(Transform trans);
	void CreateProminence(Transform trans);
	void CreateSingularity(Transform trans);

	void FireEmp(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void SpawnEMP(GameObject* target);

	void CreateScenery();
	string GrabRandomAsteroidInfo();
	string GrabRandomPlanetInfo();

	XMFLOAT4X4 TranslateTargetBack(float offset, Transform trans);

	default_random_engine m_RandEngine;
	uniform_real_distribution<float> m_UniformDist;
	
	//Level Control Variables
	LevelManager m_LevelManager;
	string m_szLevelShape;
	Level* m_pCurrentLevel;
	float m_fLevelTimer						= 0.0f;
	unsigned int m_unCurrentLevel			= 0;
	unsigned int m_unCurrentWave			= 0;
public:
	unsigned int m_unNumEnemiesToKill		= 0;
private:
	unsigned int m_unNumEnemiesKilled		= 0;
	float m_fPauseTimer						= 0.0f;
	float m_fPauseTimeLeft					= 5.0f;
	float m_fLevelClearTimer				= 5.0f;
	bool m_bLevelPassed						= false;
	bool m_bWavesPaused						= true;
	bool m_bGameWon							= false;
	bool m_bGameLost						= false;


	string m_oldTimerNumber = "  ";

	float m_safetyTimer = 0;


	float m_empTimer = 0;


	bool m_bGameOver						= false;
	bool m_bResetGame						= false;

	unsigned int m_nEnemyMultiplier			= 1;

	float m_fOldDamageScalar				= 1.0f;
	float m_fOldSpeedScalar					= 1.0f;
	float m_fOldResilienceScalar			= 1.0f;
	float m_fScalarMessageTimer				= 0.0f;
	bool  m_bScalarMessageChanged			= false;
	GameObject* m_pStatScalarMessage		= nullptr;

	bool m_bFillPylonMessagePlayed			= false;
	bool m_bFillPylonMessageMade			= false;
	GameObject* m_pFillPylonMessage			= nullptr;
	float m_fFillPylonTimer					= 0.0f;



	//Managers
	BitmapFontManager* m_pBitmapFontManager;
	AIManager* m_AIManager;

	//Debug Listener
	DebugListener m_debugListener;

	//GameObjects
	GameObject* m_Pylon1				= nullptr;
	GameObject* m_Pylon2				= nullptr;
	GameObject* m_Pylon3				= nullptr;
	GameObject* m_Player				= nullptr;

	unsigned int m_unNumPylonsAlive		= 3;

	//Percent of Pylon health
	float m_fPylon1HealthPercentLeft	= 0.5f;
	float m_fPylon2HealthPercentLeft	= 0.5f;
	float m_fPylon3HealthPercentLeft	= 0.5f;
	bool  m_bPylonsAllFull				= false;

	bool m_bPylon1Full					= false;
	bool m_bPylon2Full					= false;
	bool m_bPylon3Full					= false;



	//HudObjects

	//Player HUD
	GameObject* m_PlayerHealthBack		= nullptr;
	GameObject* m_PlayerHealthFill		= nullptr;
	float m_fLastPlayerHealth			= 1.0f;
	float m_fPlayerHealthSize			= 0.0;
	float m_fFlashTimerPlayer			= 0.0f;
	XMFLOAT4 m_f4Color					= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	bool m_bPlayerHealthFlash			= false;

	GameObject* m_PlayerEnergyBack		= nullptr;
	GameObject* m_PlayerEnergyFill		= nullptr;
	float m_fPlayerEnergySize			= 0.0f;


	//Pylon HUD
	GameObject* m_Pylon1HealthBack		= nullptr;
	GameObject* m_Pylon2HealthBack		= nullptr;
	GameObject* m_Pylon3HealthBack		= nullptr;
	GameObject* m_Pylon1HealthFill		= nullptr;
	GameObject* m_Pylon2HealthFill		= nullptr;
	GameObject* m_Pylon3HealthFill		= nullptr;
	float m_fPylonHealthSize			= 0.0f;
	GameObject* m_pPylon1Icon			= nullptr;
	GameObject* m_pPylon2Icon			= nullptr;
	GameObject* m_pPylon3Icon			= nullptr;

	GameObject* m_empIcon = nullptr;
	GameObject* m_empIcon2 = nullptr;


	bool m_bFlashPylon1Health			= false;
	bool m_bFlashPylon2Health			= false;
	bool m_bFlashPylon3Health			= false;

	float m_fFlashTimer1				= 0.0f;
	float m_fFlashTimer2				= 0.0f;
	float m_fFlashTimer3				= 0.0f;
	float m_fLastHealth1Percent			= 1.0f;
	float m_fLastHealth2Percent			= 1.0f;
	float m_fLastHealth3Percent			= 1.0f;
	


	//Pylon Arrow HUD
	GameObject* m_pArrowSigma			= nullptr;
	GameObject* m_pArrowLamda			= nullptr;
	GameObject* m_pArrowOmega			= nullptr;
	bool m_bPylonOmegaAlive				= true;
	bool m_bPylonLamdaAlive				= true;
	bool m_bPylonSigmaAlive				= true;
	float m_fArrowOffset				= 10.0f;

	bool m_bArrowSigmaRedGlow			= false;
	bool m_bArrowLamdaRedGlow			= false;
	bool m_bArrowOmegaRedGlow			= false;

	//Electricity Effects
	ElectricityEffect* m_pLightning1	= nullptr;		//pylon1 a.k.a. Lambda	-> Orange
	ElectricityEffect* m_pLightning2	= nullptr;
	ElectricityEffect* m_pLightning3	= nullptr;		//pylon2 a.k.a. Sigma	-> Purple
	ElectricityEffect* m_pLightning4	= nullptr;
	ElectricityEffect* m_pLightning5	= nullptr;		//pylon3 a.k.a. Omega	-> Green
	ElectricityEffect* m_pLightning6	= nullptr;

	float m_fLatch1Timer = 0;
	float m_fLatch2Timer = 0;
	float m_fLatch3Timer = 0;

	//TextComponent HUD
	GameObject* m_pLevelTimerText = nullptr;
	GameObject* m_pLevelWinText = nullptr;
	GameObject* m_pLevelLoseText = nullptr;
	
	GameObject* m_pLevelTimerNumber		= nullptr;

	GameObject* m_pPauseTimeText		= nullptr;
	GameObject* m_pPauseTimeNumber		= nullptr;
	int m_nLastTime = 5;

	// Score numbers
	GameObject* m_pScoreText			= nullptr;
	GameObject* m_pScoreNumber			= nullptr;
	GameObject* m_pTallyText			= nullptr;
	GameObject* m_pTallyNumber			= nullptr;

	//Game won/lost HUD
	GameObject* m_GameWon				= nullptr;
	GameObject* m_GameLost				= nullptr;

	// CURRENT GUN HUD ICON
	GameObject* m_pCurrGunIcon			= 0;
	GameObject* m_pSpreadGunIcon		= nullptr;
	GameObject* m_pWhipGunIcon			= nullptr;
	GameObject* m_pMissileGunIcon		= nullptr;

	GameObject* m_pMeshDistFX			= nullptr;

	// Gamepad stuff
	DWORD dwPacketNo;
	float m_fGamepadBuff;
	void Handle360Input();

	float m_fTimeSlowHelper				= 0;

	GameObject* m_pWorldObject			= nullptr;
	GameObject* m_pInnerStar			= nullptr;

	// controller emp stuff
	WORD prevButtons = 0;

	GameObject* m_pHealthLowSound;

	bool bSongSwitch;

	GameObject* m_pBossHealthBack = nullptr;
	GameObject* m_pBossHealthFill = nullptr;
	float fBossHealthSize = 0;

	XMFLOAT4 bossHealthBarColor = XMFLOAT4(1, 0, 0, 1);

	// WATCH OUT GUYS HERE'S SOME QUICK EMP STUFF I AM EXTREMELY SORRY
	struct EMP
	{
	public:
		void Activate(Transform& epicenterTrans, ObjectManager* objMngr, int& toIncr);
		void Update(ObjectManager* objMngr, float dt, uniform_real_distribution<float>& URDist, default_random_engine& RandEng, int& numEMPsActive);
		void Deactivate(ObjectManager* objMngr, int& toDecr);

		const bool IsActive() const { return m_bActive; }

	private:
		bool m_bActive = false;
		const float m_fRadiusIncrSpeed = 20.0f;
		const float m_fMaxRadius = 50.0f;
		float m_fcurrRadius = 0;
		ObjectManager* m_pObjMngr = nullptr;

		XMFLOAT3 TransformAxes[3];
		XMFLOAT3 m_pos = XMFLOAT3(0,0,0);

		ElectricityEffect* allElecs[NUM_EMP_ELECS];
	};

	EMP allPossibleEMPs[NUM_EMPS];
	int numActiveEMPs = 0;

	float m_fBeatTimer;

	void Pulse();

};

