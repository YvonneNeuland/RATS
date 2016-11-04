#pragma once
#include <list>

#include "BaseBehavior.h"
#include "BehavShoot.h"
#include "BehavTracking.h"
#include "BehavFlocking.h"
#include "BehavRandMove.h"
#include "BehavSpawning.h"
#include "Bullet Behaviors/BehavWhipBullet.h"
#include "BehavEnvMove.h"
#include "BehavProxMine.h"
#include "BehavProminence.h"
#include "BehavProminenceBullet.h"
#include "BehavDodge.h"
#include "BehavGoliath.h"
#include "BehavLandPoint.h"
#include "BehavBoss.h"
#include "../Molecules/MoleculeManager.h"
#include <random>

// FWRD DECL
class AICompBase;
class GameObject;
class ObjectManager;
// --------------

#define MAX_TRACK_BEHAV 150
#define MAX_SHOOT_BEHAV 200
#define MAX_FLOCK_BEHAV 200
#define MAX_RANDMOVE_BEHAV 100
#define MAX_SPWAN_BEHAV 100
#define MAX_WHIP_BEHAV 500
#define MAX_ENVMOVE_BEHV 20
#define MAX_PROMINENCE_BEHAV 30
#define MAX_PROMINENCEBULLET_BEHAV 1004
#define MAX_PROXMINE_BEHAV 100
#define MAX_DODGE_BEHAV 50
#define MAX_GROW_BEHAV 50
#define MAX_BOSS_BEHAV 5
#define MAX_LANDPOINT_BEHAV 100

#define KOI_BUFF_SPEED 40.0f
#define KOI_BUFF_SHOOT 0.7f

class AIManager
{
	GameObject* m_pPlayer;
	GameObject* m_pLastPlayerBullet;
	std::list<AICompBase*> m_Minions;
	Flock* m_MasterFlock;
	std::list<AICompBase*> m_vExtras;
	std::list<AICompBase*> m_vSpawning;
	std::list<AICompBase*> m_vProminencePieces;
	std::list<AICompBase*> m_vFencePosts;
	Flock* m_KoiAttack;
	Flock* m_KoiDisrupt;
	Whip* m_pWhip;
	std::list<AICompBase*>::iterator m_Iter;
	float m_phDelta;

	std::list<BaseBehavior*> m_vTrackUnused;
	std::list<BaseBehavior*> m_vShootUnused;
	std::list<BaseBehavior*> m_vFlockUnused;
	std::list<BaseBehavior*> m_vRandMoveUnused;
	std::list<BaseBehavior*> m_vSpawnUnused;
	std::list<BaseBehavior*> m_vWhipUnused;
	std::list<BaseBehavior*> m_vEnvMoveUnused;
	std::list<BaseBehavior*> m_vProminenceUnused;
	std::list<BaseBehavior*> m_vProminenceBulletsUnused;
	std::list<BaseBehavior*> m_vProxMineUnused;
	std::list<BaseBehavior*> m_vDodgeUnused;
	std::list<BaseBehavior*> m_vGrowUnused;
	std::list<BaseBehavior*> m_vLandPointUnused;
	std::list<BaseBehavior*> m_vBossUnused;

	ObjectManager* m_pObjManager;

	std::default_random_engine m_RandEngine;
	std::uniform_real_distribution<float> m_UniformDist;

	float m_fNumTotalKoiA;
	int m_fNumSpawnKoiD;
	int m_fNumSpawnKoiA;

	bool m_bBossActive;

public:
	AIManager();
	~AIManager();

	void Init();
	void Shutdown();
	void UpdateMinions(float _dt);
	void RegisterMinion(AICompBase* _newGuy);
	void RegisterMinionReal(AICompBase* _newbie, bool _skip = false);
	void CreateKoiAttack(int _numKoi);
	void CreateKoiDisrupt(int _numKoi);
	void KillMinion(AICompBase* _Fired);
	void TurnTowardsPlayer(AICompBase* _in);
	void TrunTowardTarget(AICompBase* _this, GameObject* _that);
	void Shoot(AICompBase* _in);
	void SetObjManager(ObjectManager* _instance) { m_pObjManager = _instance; }
	//void FindWork(AICompBase* _in);
	void SetupBehaviors(AICompBase* _in);
	
	void SetPlayer(GameObject* _player) { m_pPlayer = _player; }
	void SetLastPlayerBullet(GameObject* _newBullet) { m_pLastPlayerBullet = _newBullet; }
	void HandlePylonDeath(GameObject* _pylon);
	void DisbandSwarm(Flock* _group);
	void BuffSwarm(Flock* _group);

	MoleculeEmitter* AttachTrail(AICompBase* _in);

	void StopWhip() { m_pWhip->Stop(); }
	void StartWhip() { m_pWhip->Start(); }



	void CalculateSpawnPosition(AICompBase* _in);

	DirectX::XMFLOAT3 CalcSpherePos(AICompBase* _in);
	DirectX::XMFLOAT3 CalcCubePos(AICompBase* _in);
	DirectX::XMFLOAT3 CalcTorusPos(AICompBase* _in);

	DirectX::XMFLOAT4 GetEnemyColor(AICompBase* _in);

};

