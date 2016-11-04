#include "stdafx.h"
#include "AIManager.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/Transform.h"
#include "../Object Manager/ObjectManager.h"
#include "../Collision/CollisionManager.h"
#include "../Audio/FFmod.h"
#include "../VFX/RibbonEffect/RibbonEffect.h"
#include "../Upgrade System/GameData.h"

extern GameData* gameData;
extern 	FFmod*		g_AudioSystem;

AIManager::AIManager()
{
}


AIManager::~AIManager()
{
}

void AIManager::Init()
{

	//m_RandEngine;
	m_UniformDist = std::uniform_real_distribution<float>(-1.0f);


	m_pPlayer = 0;
	m_pObjManager = 0;
	m_vTrackUnused.resize(MAX_TRACK_BEHAV);
	//for (int i = 0; i < MAX_TRACK_BEHAV; i++)
	auto it = m_vTrackUnused.begin();
	while (it != m_vTrackUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavTracking;
		it++;
	}

	m_vShootUnused.resize(MAX_SHOOT_BEHAV);
	it = m_vShootUnused.begin();
	while (it != m_vShootUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavShoot;
		it++;
	}

	m_vFlockUnused.resize(MAX_FLOCK_BEHAV);
	it = m_vFlockUnused.begin();
	while (it != m_vFlockUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavFlocking;
		it++;
	}

	m_vRandMoveUnused.resize(MAX_RANDMOVE_BEHAV);
	it = m_vRandMoveUnused.begin();
	while (it != m_vRandMoveUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavRandMove;
		it++;
	}

	m_vEnvMoveUnused.resize(MAX_ENVMOVE_BEHV);
	it = m_vEnvMoveUnused.begin();
	while (it != m_vEnvMoveUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavEnvMove;
		it++;
	}

	m_vProminenceUnused.resize(MAX_PROMINENCE_BEHAV);
	it = m_vProminenceUnused.begin();
	while (it != m_vProminenceUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavProminence;
		it++;
	}

	m_vProminenceBulletsUnused.resize(MAX_PROMINENCEBULLET_BEHAV);
	it = m_vProminenceBulletsUnused.begin();
	while (it != m_vProminenceBulletsUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavProminenceBullet;
		it++;
	}

	m_vSpawnUnused.resize(MAX_SPWAN_BEHAV);
	it = m_vSpawnUnused.begin();
	while (it != m_vSpawnUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavSpawning;
		it++;
	}

	m_vWhipUnused.resize(MAX_WHIP_BEHAV);
	it = m_vWhipUnused.begin();
	while (it != m_vWhipUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavWhipBullet;
		it++;
	}

	m_vProxMineUnused.resize(MAX_PROXMINE_BEHAV);
	it = m_vProxMineUnused.begin();
	while (it != m_vProxMineUnused.end())
	{
		//m_vTrackUnused.push_back(new BehavTracking);
		(*it) = new BehavProxMine;
		it++;
	}

	m_vDodgeUnused.resize(MAX_DODGE_BEHAV);
	it = m_vDodgeUnused.begin();
	while (it != m_vDodgeUnused.end())
	{
		(*it) = new BehavDodge;
		it++;
	}

	m_vGrowUnused.resize(MAX_GROW_BEHAV);
	it = m_vGrowUnused.begin();
	while (it != m_vGrowUnused.end())
	{
		(*it) = new BehavGoliath;
		it++;
	}

	m_vBossUnused.resize(MAX_BOSS_BEHAV);
	it = m_vBossUnused.begin();
	while (it != m_vBossUnused.end())
	{
		(*it) = new BehavBoss;
		it++;
	}

	m_vLandPointUnused.resize(MAX_LANDPOINT_BEHAV);
	it = m_vLandPointUnused.begin();
	while (it != m_vLandPointUnused.end())
	{
		(*it) = new BehavLandPoint;
		it++;
	}

	m_KoiAttack = new Flock;
	m_KoiDisrupt = new Flock;
	m_pWhip = new Whip;
	m_MasterFlock = new Flock;

	m_MasterFlock->m_fAlignmentWait = 0;
	m_MasterFlock->m_fCohesionWait = 0;
	m_MasterFlock->m_fSeparationWait = 10;

	m_fNumTotalKoiA = 0;
	m_bBossActive = false;
	//m_KoiAttack->m_vFlock.resize(15);
}

void AIManager::RegisterMinion(AICompBase* _newGuy)
{

	//return RegisterMinionReal(_newGuy);
	int leType = _newGuy->GetOwner()->GetType();

	if (leType == EvilBullet_Mine ||
		leType == EvilBullet_HeatSeek ||
		leType == Bullet_Whip ||
		leType == Bullet_Missile ||
		leType == Env_Tornado ||
		leType == Env_Prominence ||
		leType == Env_Prominence_Bullet || 
		leType == EvilBullet_FencePost ||
		leType == Env_Singularity || 
		leType == Landing_Indicator ||
		leType == Orb_Blue ||
		leType == Pickup_Health ||
		leType == Pickup_Shield ||
		m_bBossActive == true)
		return RegisterMinionReal(_newGuy, true);



	// pushback a spawn behavior
	_newGuy->m_vBehaviors.splice(_newGuy->m_vBehaviors.begin(), m_vSpawnUnused, m_vSpawnUnused.begin());
	(*_newGuy->m_vBehaviors.begin())->SetOwner(_newGuy);

	m_vSpawning.push_front(_newGuy);

	_newGuy->GetOwner()->SetFlags(eSTATUS_FLAGS::Spwaning);


	float stagger;
	if(leType == Enemy_Koi || leType == Enemy_Disruptor)
	{
		stagger = 12.5f;
	}

	else
	{
		stagger = (float)(rand() % 10) + 7.0f;
	}
	
	
	reinterpret_cast<BehavSpawning*>((*_newGuy->m_vBehaviors.begin()))->m_fSpeeed = stagger;
	
	

	CalculateSpawnPosition(_newGuy);

	reinterpret_cast<BehavSpawning*>((*_newGuy->m_vBehaviors.begin()))->m_pTrailFX = AttachTrail(_newGuy);

}


void AIManager::RegisterMinionReal(AICompBase* _newbie, bool _skip)
{

	// remove only current component: spawning behavior

	int leType = _newbie->GetOwner()->GetType();

	if (!_skip)
	{
		reinterpret_cast<BehavSpawning*>(*_newbie->m_vBehaviors.begin())->m_bFirst = true;
		reinterpret_cast<BehavSpawning*>(*_newbie->m_vBehaviors.begin())->m_bClose = false;
		reinterpret_cast<BehavSpawning*>(*_newbie->m_vBehaviors.begin())->m_bLanded = false;
		reinterpret_cast<BehavSpawning*>(*_newbie->m_vBehaviors.begin())->m_fTotalTimer = 0.0f;
		m_vSpawnUnused.splice(m_vSpawnUnused.begin(), _newbie->m_vBehaviors, _newbie->m_vBehaviors.begin());
		_newbie->GetOwner()->ClearFlags(eSTATUS_FLAGS::Spwaning);
		m_vSpawning.remove(_newbie);


		if (leType == Enemy_Disruptor)
		{
			m_fNumSpawnKoiD--;
		}
		if (leType == Enemy_Koi)
		{
			m_fNumSpawnKoiA--;
		}


	}

	SetupBehaviors(_newbie);

// 	if (_newbie->m_AIType == AICompBase::eAITYPE::AI_KOI_A)
// 		m_KoiAttack->m_vFlock.push_back(_newbie);
// 	else if (_newbie->m_AIType == AICompBase::eAITYPE::AI_KOI_D)
// 		m_KoiDisrupt->m_vFlock.push_back(_newbie);
// 	else if (_newbie->m_AIType == AICompBase::eAITYPE::AI_WHIP)
// 		m_pWhip->m_cBullets.push_front(_newbie);
// 	else if (_newbie->m_AIType == AICompBase::eAITYPE::AI_PROMINENCE_BULLET 
// 		|| _newbie->m_AIType == AICompBase::AI_PROMINENCE)
// 		m_vProminencePieces.push_back(_newbie);
// 	else
// 		m_MasterFlock->m_vFlock.push_back(_newbie);

	switch (_newbie->m_AIType)
	{
	case AICompBase::eAITYPE::AI_KOI_A:
		m_KoiAttack->m_vFlock.push_back(_newbie);
		break;

	case AICompBase::eAITYPE::AI_KOI_D:
		m_KoiDisrupt->m_vFlock.push_back(_newbie);
		break;

	case AICompBase::eAITYPE::AI_WHIP:
		m_pWhip->m_cBullets.push_front(_newbie);
		break;

	case AICompBase::eAITYPE::AI_PROMINENCE_BULLET:
	case AICompBase::AI_PROMINENCE:
		m_vProminencePieces.push_back(_newbie);
		break;

	case AICompBase::eAITYPE::AI_FENCEPOST:
		m_vFencePosts.push_back(_newbie);
		break;

	case AICompBase::eAITYPE::AI_LANDINGICON:
	case AICompBase::eAITYPE::AI_SINGULARITY:
	case AICompBase::eAITYPE::AI_TORNADO:
	case AICompBase::eAITYPE::AI_PICKUP:
		m_vExtras.push_back(_newbie);
		break;

	case AICompBase::eAITYPE::AI_BOSS:
		m_bBossActive = true;

	default:
		m_MasterFlock->m_vFlock.push_back(_newbie);
		break;
	}


}


void AIManager::UpdateMinions(float _dt)
{
	m_phDelta = _dt;
	AICompBase* transferHelp = nullptr;
	//AICompBase* pComp;

	// SPWANING IN

	if (m_vSpawning.size() > 0)
	{
		for (m_Iter = m_vSpawning.begin(); m_Iter != m_vSpawning.end(); /*m_Iter++*/)
		{
			(*(*m_Iter)->m_vBehaviors.begin())->DoLogic(_dt);

			if (reinterpret_cast<BehavSpawning*>((*(*m_Iter)->m_vBehaviors.begin()))->m_bLanded)
			{
				transferHelp = (*m_Iter);
				m_Iter++;
				RegisterMinionReal(transferHelp);
				/*m_Iter = m_vSpawning.begin();*/
			}
			else
			{
				m_Iter++;
			}


		}
	}


	if (m_vProminencePieces.size() > 0)
	{
		for (m_Iter = m_vProminencePieces.begin(); m_Iter != m_vProminencePieces.end(); m_Iter++)
		{
			(*m_Iter)->Update(_dt);

			if ((*m_Iter)->m_bReadyAim)
			{
				Shoot((*m_Iter));
				(*m_Iter)->m_bReadyAim = false;
			}
		}
	}

	if (m_vFencePosts.size() > 0)
	{
		for (m_Iter = m_vFencePosts.begin(); m_Iter != m_vFencePosts.end(); m_Iter++)
		{
			(*m_Iter)->Update(_dt);
		}
	}

	if (m_vExtras.size() > 0)
	{
		for (m_Iter = m_vExtras.begin(); m_Iter != m_vExtras.end(); m_Iter++)
		{
			(*m_Iter)->Update(_dt);
		}
	}
	

	if (m_MasterFlock->m_vFlock.size() > 0)
	{
		for (m_Iter = m_MasterFlock->m_vFlock.begin(); m_Iter != m_MasterFlock->m_vFlock.end(); m_Iter++)
		{
			if ((*m_Iter)->GetOwner()->IsSpinningOut())
				continue;

			(*m_Iter)->Update(_dt);

			if ((*m_Iter)->m_bReadyAim)
			{
				Shoot((*m_Iter));
				(*m_Iter)->m_bReadyAim = false;
			}

			if ((*m_Iter)->m_AIType == AICompBase::AI_GOLIATH)
			{
				if ((*m_Iter)->m_bNeedShootComp)
				{
					(*m_Iter)->m_vBehaviors.splice((*m_Iter)->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
					(*(*m_Iter)->m_vBehaviors.begin())->SetOwner((*m_Iter));
					(*m_Iter)->m_bNeedShootComp = false;
				}
			}
		}
	}


	if (m_KoiAttack->m_vFlock.size() > 0 && m_fNumSpawnKoiA == 0)
	{
// 		if (m_KoiAttack->m_vFlock.size() < 5 && !m_KoiAttack->m_bDisband)
// 			DisbandSwarm(m_KoiAttack);

		m_KoiAttack->Update(_dt);
		for (m_Iter = m_KoiAttack->m_vFlock.begin(); m_Iter != m_KoiAttack->m_vFlock.end(); m_Iter++)
		{
			//(*m_Iter)->Update(_dt);


			if ((*m_Iter)->m_bReadyAim)
			{
				Shoot((*m_Iter));
				(*m_Iter)->m_bReadyAim = false;
			}
		}
	}

	if (m_KoiDisrupt->m_vFlock.size() > 0 && m_fNumSpawnKoiD == 0)
	{
// 		if (m_KoiDisrupt->m_vFlock.size() < 5 && !m_KoiDisrupt->m_bDisband)
// 			DisbandSwarm(m_KoiDisrupt);

		m_KoiDisrupt->Update(_dt);

		

		for (m_Iter = m_KoiDisrupt->m_vFlock.begin(); m_Iter != m_KoiDisrupt->m_vFlock.end(); m_Iter++)
		{
			//(*m_Iter)->Update(_dt);

// 
// 			if ((*m_Iter)->m_bReadyAim /*&& (m_KoiDisrupt->m_bRotate || m_KoiDisrupt->m_bDisband)*/)
// 			{
// 				Shoot((*m_Iter));
// 				(*m_Iter)->m_bReadyAim = false;
// 			}

			

		}
	}

	if (m_pWhip->m_cBullets.size() > 0)
	{
		m_pWhip->Update(_dt);
	}

}

void AIManager::Shutdown()
{

	auto LiveIt = m_Minions.begin();
	while (LiveIt != m_Minions.end())
	{
		while (!(*LiveIt)->m_vBehaviors.empty())
		{
			delete (*(*LiveIt)->m_vBehaviors.begin());
			(*LiveIt)->m_vBehaviors.pop_front();
		}
		LiveIt++;
	}

	LiveIt = m_vSpawning.begin();
	while (LiveIt != m_vSpawning.end())
	{
		while (!(*LiveIt)->m_vBehaviors.empty())
		{
			delete (*(*LiveIt)->m_vBehaviors.begin());
			(*LiveIt)->m_vBehaviors.pop_front();
		}
		LiveIt++;
	}

	auto it = m_vTrackUnused.begin();
	while (it != m_vTrackUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vShootUnused.begin();
	while (it != m_vShootUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vFlockUnused.begin();
	while (it != m_vFlockUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vRandMoveUnused.begin();
	while (it != m_vRandMoveUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vEnvMoveUnused.begin();
	while (it != m_vEnvMoveUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vProminenceUnused.begin();
	while (it != m_vProminenceUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vProminenceBulletsUnused.begin();
	while (it != m_vProminenceBulletsUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vSpawnUnused.begin();
	while (it != m_vSpawnUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vWhipUnused.begin();
	while (it != m_vWhipUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vProxMineUnused.begin();
	while (it != m_vProxMineUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vDodgeUnused.begin();
	while (it != m_vDodgeUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vGrowUnused.begin();
	while (it != m_vGrowUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}


	it = m_vBossUnused.begin();
	while (it != m_vBossUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	it = m_vLandPointUnused.begin();
	while (it != m_vLandPointUnused.end())
	{
		delete (*it);
		(*it) = nullptr;
		it++;
	}

	m_KoiDisrupt->m_vFlock.clear();
	delete m_KoiDisrupt;
	m_KoiAttack->m_vFlock.clear();
	m_MasterFlock->m_vFlock.clear();
	delete m_KoiAttack;
	delete m_pWhip;
	delete m_MasterFlock;

	m_vSpawning.clear();
	m_Minions.clear();
}

void AIManager::KillMinion(AICompBase* _Fired)
{
	if (_Fired->GetAIType() == AICompBase::AI_MINE)
	{

		m_pObjManager->m_Molecules->BuildPosEmitter(*_Fired->GetOwner()->GetTransform().GetPosition(), 250, "Small Orange Fire Explosion");
	}

	if (_Fired->GetAIType() == AICompBase::AI_MISSILE)
	{
		_Fired->GetShooter()->SetMissileOut(false);
	}
// 
	if (_Fired->m_pTrail)
	{
		_Fired->m_pTrail->SetDead(true);
		reinterpret_cast<RibbonEffect*>(_Fired->m_pTrail)->SetParent(0);
		_Fired->m_pTrail = 0;
	}

	BehavFlocking* tmpFlockBehav = 0;

	while (!_Fired->m_vBehaviors.empty())
	{
		switch ((*_Fired->m_vBehaviors.begin())->GetLogicType())
		{
		case eBEHAVIORS::bFollow:
			reinterpret_cast<BehavTracking*>(*_Fired->m_vBehaviors.begin())->m_fMissilePauseTimer = 0;
			reinterpret_cast<BehavTracking*>(*_Fired->m_vBehaviors.begin())->m_fRotSpeed = 0;
			reinterpret_cast<BehavTracking*>(*_Fired->m_vBehaviors.begin())->m_targetTimer = 0;
			m_vTrackUnused.splice(m_vTrackUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bShoot:
			m_vShootUnused.splice(m_vShootUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bFlock:
			tmpFlockBehav = reinterpret_cast<BehavFlocking*>(*_Fired->m_vBehaviors.begin());

			tmpFlockBehav->m_bLatch = false;
			if (tmpFlockBehav->m_pLatchVFX[0] != nullptr)
			{
				reinterpret_cast<ElectricityEffect*>(tmpFlockBehav->m_pLatchVFX[0])->ResetTimer();
				tmpFlockBehav->m_pLatchVFX[0]->SetDead(true);
				tmpFlockBehav->m_pLatchVFX[0] = nullptr;

				reinterpret_cast<ElectricityEffect*>(tmpFlockBehav->m_pLatchVFX[1])->ResetTimer();
				tmpFlockBehav->m_pLatchVFX[1]->SetDead(true);
				tmpFlockBehav->m_pLatchVFX[1] = nullptr;
			}
			
			m_vFlockUnused.splice(m_vFlockUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bRandMove:
			m_vRandMoveUnused.splice(m_vRandMoveUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bEnvMove:
			m_vEnvMoveUnused.splice(m_vEnvMoveUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bWhipBullet:
			m_vWhipUnused.splice(m_vWhipUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bProxMine:
			reinterpret_cast<BehavProxMine*>(*_Fired->m_vBehaviors.begin())->m_fPauseTimer = -1;
			m_vProxMineUnused.splice(m_vProxMineUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			_Fired->m_bProxTrigger = false;
			break;

		case eBEHAVIORS::bSpawn:
			reinterpret_cast<BehavSpawning*>(*_Fired->m_vBehaviors.begin())->Reset();
			
			m_vSpawnUnused.splice(m_vSpawnUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			_Fired->GetOwner()->ClearFlags(eSTATUS_FLAGS::Spwaning);
			break;

		case eBEHAVIORS::bProminenceFire:
			m_vProminenceUnused.splice(m_vProminenceUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bProminenceBullet:
			m_vProminenceBulletsUnused.splice(m_vProminenceBulletsUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bDodge:
			m_vDodgeUnused.splice(m_vDodgeUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bGrow:
			reinterpret_cast<BehavGoliath*>(*_Fired->m_vBehaviors.begin())->Reset();
			m_vGrowUnused.splice(m_vGrowUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;

		case eBEHAVIORS::bBOSS:
			reinterpret_cast<BehavBoss*>(*_Fired->m_vBehaviors.begin())->Reset();
			m_vBossUnused.splice(m_vBossUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			m_bBossActive = false;
			break;

		case eBEHAVIORS::bLandPoint:
			m_vLandPointUnused.splice(m_vLandPointUnused.begin(), _Fired->m_vBehaviors, _Fired->m_vBehaviors.begin());
			break;


		default:
			break;
		}
	}


	if (_Fired->m_AIType == AICompBase::AI_KOI_A)
	{
		m_KoiAttack->m_vFlock.remove(_Fired);
		BuffSwarm(m_KoiAttack);
	}


// 	if (m_KoiAttack->m_vFlock.size() > 0)
// 	{
// 		
// 		// 		if (m_KoiAttack->m_vFlock.size() == 0)
// 		// 		{
// 		// 			GameObject* tmpHealth = m_pObjManager->CreateObject(eOBJECT_TYPE::Pickup_Health, m_KoiAttack->m_vAvgPos);
// 		// 			tmpHealth->GetTransform().GetWorldMatrix();
// 		// 		}
// 
// 	}

	if (m_KoiDisrupt->m_vFlock.size() > 0)
	{
		m_KoiDisrupt->m_vFlock.remove(_Fired);
	}

	if (m_vExtras.size() > 0)
	{
		m_vExtras.remove(_Fired);
	}

	if (m_pWhip->m_cBullets.size() > 0)
	{
		m_pWhip->m_cBullets.remove(_Fired);
	}

	if (m_vSpawning.size() > 0)
	{
		m_vSpawning.remove(_Fired);
	}

	if (m_vProminencePieces.size() > 0)
	{
		m_vProminencePieces.remove(_Fired);
	}

	if (m_vFencePosts.size() > 0)
	{
		m_vFencePosts.remove(_Fired);
	}

	m_MasterFlock->m_vFlock.remove(_Fired);

	//m_Minions.remove(_Fired);
}

void AIManager::TurnTowardsPlayer(AICompBase* _in)
{
	Transform lhs_transform = _in->GetOwner()->GetTransform();
	Transform player_transform;

	if (m_pPlayer != nullptr)
	{
		player_transform = m_pPlayer->GetTransform();
		DirectX::XMMATRIX mat = XMLoadFloat4x4(&lhs_transform.GetLocalMatrix());

		// Get WORLD-SPACE positions
		DirectX::XMFLOAT3 lhs_pos = *lhs_transform.GetPosition();
		DirectX::XMFLOAT3 player_pos = *player_transform.GetPosition();
		DirectX::XMVECTOR to_target = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&player_pos), DirectX::XMLoadFloat3(&lhs_pos));

		//float up_down = DotProduct(mat.YAxis, to_target);
		float left_right = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(lhs_transform.GetXAxis()), to_target).m128_f32[0];

		//if (up_down > DEAD_ZONE || up_down < -DEAD_ZONE)
		//	mat.rotateLocalX(up_down * turn_speed * Game::GetDeltaTime());
		if (left_right > FLT_EPSILON || left_right < -FLT_EPSILON)
			_in->GetOwner()->GetTransform().RotateBy(1, left_right * _in->GetTurnSpeed() * m_phDelta);//mat.rotateLocalY(left_right * -turn_speed * Game::GetDeltaTime());

		// Recalculate the object-space matrix for the "looker"
		//	CrossProduct(mat.XAxis, Float3(0.0f, 1.0f, 0.0f), mat.ZAxis);
		//	CrossProduct(mat.YAxis, mat.ZAxis, mat.XAxis);

		//	mat.XAxis.normalize();
		//	mat.YAxis.normalize();
		//	mat.ZAxis.normalize();

		//lhs_transform->SetLocalMatrix(mat);
	}
}

void AIManager::TrunTowardTarget(AICompBase* _this, GameObject* _that)
{
	Transform lhs_transform = _this->GetOwner()->GetTransform();
	Transform rhs_transform;

	if (_that != nullptr)
	{
		rhs_transform = _that->GetTransform();
		DirectX::XMMATRIX mat = XMLoadFloat4x4(&lhs_transform.GetLocalMatrix());

		// Get WORLD-SPACE positions
		DirectX::XMFLOAT3 lhs_pos = *lhs_transform.GetPosition();
		DirectX::XMFLOAT3 rhs_pos = *rhs_transform.GetPosition();
		DirectX::XMVECTOR to_target = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&rhs_pos), DirectX::XMLoadFloat3(&lhs_pos));

		//float up_down = DotProduct(mat.YAxis, to_target);
		float left_right = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(lhs_transform.GetXAxis()), to_target).m128_f32[0];

		//if (up_down > DEAD_ZONE || up_down < -DEAD_ZONE)
		//	mat.rotateLocalX(up_down * turn_speed * Game::GetDeltaTime());
		if (left_right > FLT_EPSILON || left_right < -FLT_EPSILON)
			_this->GetOwner()->GetTransform().RotateBy(1, left_right * _this->GetTurnSpeed() * m_phDelta);//mat.rotateLocalY(left_right * -turn_speed * Game::GetDeltaTime());

	}
}

void AttachGreenParticles(GameObject* object, ObjectManager* objManager)
{
	MoleculeEmitter* emitter = objManager->m_Molecules->BuildEmitter(object, 30, "Enemy Simple Bullet Trail");

	//MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(XMFLOAT3(80, 0, 0), 100);
}

void AIManager::Shoot(AICompBase* _in)
{
	if (_in->GetOwner()->GetFlag(eSTATUS_FLAGS::MostlyDead))
		return;



	GameObject* newBullet;
	switch (_in->m_AIType)
	{
	case AICompBase::eAITYPE::AI_NONE:
		std::cout << "NO AI Type Shoot()\n";
		break;

	case AICompBase::eAITYPE::AI_MERC:
		// send msg to shoot at my transform
		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::EvilBullet_HeatSeek, *_in->GetOwner()->GetTransform().GetPosition());
		if (!newBullet)
			return;
		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
		newBullet->SetVelocityInWorldSpace(false);
		//newBullet->SetVelocity(DirectX::XMFLOAT3(0, 0, 60));
		//newBullet->
		reinterpret_cast<AICompBase*>(newBullet->GetComponent("AICompBase"))->SetShooter(_in);
		_in->SetMissileOut(true);
		g_AudioSystem->PlaySound("RATS_MissileLoop", newBullet, false, true);// , *newBullet->GetTransform().GetPosition());



		break;

	case AICompBase::eAITYPE::AI_KOI_A:
		// send msg to shoot at my transform
		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::EvilBullet_Disrupt, *_in->GetOwner()->GetTransform().GetPosition());
		if (!newBullet)
			return;
		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
		newBullet->SetVelocityInWorldSpace(false);
		newBullet->SetVelocity(DirectX::XMFLOAT3(0, 0, _in->GetStartVelocity() * 2));
		//newBullet->
		g_AudioSystem->PlaySound("RATS_EvilBullet", newBullet);// , *newBullet->GetTransform().GetPosition());

		AttachGreenParticles(newBullet, m_pObjManager);

		break;

	case AICompBase::eAITYPE::AI_DODGE:
		// send msg to shoot at my transform
		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::EvilBullet_Disrupt, *_in->GetOwner()->GetTransform().GetPosition());
		if (!newBullet)
			return;
		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
		newBullet->SetVelocityInWorldSpace(false);
		newBullet->SetVelocity(DirectX::XMFLOAT3(0, 0, _in->GetStartVelocity() * 1.5f));
		//newBullet->
		g_AudioSystem->PlaySound("RATS_EvilBullet", newBullet);// , *newBullet->GetTransform().GetPosition());

		AttachGreenParticles(newBullet, m_pObjManager);

		break;

// 	case AICompBase::eAITYPE::AI_KOI_D:
// 		// send msg to shoot at my transform
// 		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::EvilBullet_Disrupt, *_in->GetOwner()->GetTransform().GetPosition());
// 		if (!newBullet)
// 			return;
// 		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
//  		if (_in->m_bRotate)
//  		{
//  			newBullet->GetTransform().RotateBy(1, 1.5f);
//  		}
// 			
// 		newBullet->SetVelocityInWorldSpace(false);
// 		newBullet->SetVelocity(DirectX::XMFLOAT3(0, 0, 50));
// 		//newBullet->
// 		g_AudioSystem->PlaySound("RATS_EvilBullet", newBullet);// , *newBullet->GetTransform().GetPosition());
// 
// 		AttachGreenParticles(newBullet, m_pObjManager);
// 
// 		break;

	case AICompBase::eAITYPE::AI_GOLIATH:

		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::EvilBullet_Disrupt, *_in->GetOwner()->GetTransform().GetPosition());
		if (!newBullet)
			return;
		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
		newBullet->SetVelocityInWorldSpace(false);
		newBullet->SetVelocity(DirectX::XMFLOAT3(0, 0, _in->GetStartVelocity() * 1.5f));
		//newBullet->
		g_AudioSystem->PlaySound("RATS_EvilBullet", newBullet);// , *newBullet->GetTransform().GetPosition());

		AttachGreenParticles(newBullet, m_pObjManager);

		break;


	case AICompBase::eAITYPE::AI_MINELAYER:
		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::EvilBullet_Mine, *_in->GetOwner()->GetTransform().GetPosition());
		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
		g_AudioSystem->PlaySound("MENU_Back", newBullet);
		break;

	case AICompBase::eAITYPE::AI_FENCELAYER:
		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::EvilBullet_FencePost, *_in->GetOwner()->GetTransform().GetPosition());
		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
		g_AudioSystem->PlaySound("MENU_Back", newBullet);
		break;

	case AICompBase::eAITYPE::AI_PROMINENCE:
		newBullet = m_pObjManager->CreateObject(eOBJECT_TYPE::Env_Prominence_Bullet, *_in->GetOwner()->GetTransform().GetPosition(), "ProminenceBullet");
		newBullet->GetTransform() = _in->GetOwner()->GetTransform();
		m_pObjManager->m_Molecules->BuildEmitter(newBullet, 100, "ProminenceBulletMolecule");
		g_AudioSystem->PlaySound("RATS_EvilBullet", newBullet);
		break;

	default:
		std::cout << "Invalid AI Type Shoot()\n";
		break;
	}
}



void AIManager::SetupBehaviors(AICompBase* _in)
{
	int velocity;
	switch (_in->GetOwner()->GetType())
	{
		// ENEMIES
	case eOBJECT_TYPE::Enemy_Merc:

		_in->m_AIType = AICompBase::AI_MERC;
		_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = (rand() % 8) + 10;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(0.5f);
		_in->m_fShotTimerMax = 3;
		//_in->m_vBehaviors.push_front()
		// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vTrackUnused, m_vTrackUnused.begin());
		// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vFlockUnused, m_vFlockUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		reinterpret_cast<BehavFlocking*>((*_in->m_vBehaviors.begin()))->SetStats(m_MasterFlock);

		break;

	case eOBJECT_TYPE::Enemy_Koi:

		_in->m_AIType = AICompBase::AI_KOI_A;
		_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = 20;
		_in->m_fStartVelocity = 20;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(0.7f);
		_in->m_fShotTimerMax = 2;
		//_in->m_vBehaviors.push_front()
		// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vTrackUnused, m_vTrackUnused.begin());
		// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vFlockUnused, m_vFlockUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		reinterpret_cast<BehavFlocking*>((*_in->m_vBehaviors.begin()))->SetStats(m_KoiAttack);

		break;

	case eOBJECT_TYPE::Enemy_Disruptor:

		_in->m_AIType = AICompBase::AI_KOI_D;
		//_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = 50;
		_in->m_fStartVelocity = 50;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(0.8f);
		velocity = rand() % 10 + 10;
		_in->m_fShotTimerMax = (float)velocity;
		_in->m_fShotTimerMax *= 0.1f;
		//_in->m_vBehaviors.push_front()
		// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vTrackUnused, m_vTrackUnused.begin());
		// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vFlockUnused, m_vFlockUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		reinterpret_cast<BehavFlocking*>((*_in->m_vBehaviors.begin()))->SetStats(m_KoiDisrupt);

		break;

	case eOBJECT_TYPE::Enemy_Bomber:

		_in->m_AIType = AICompBase::AI_BOMBER;
		_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = 80;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(0.2f);
		//_in->m_fShotTimerMax = 3;
		//_in->m_vBehaviors.push_front()
// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vTrackUnused, m_vTrackUnused.begin());
// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vFlockUnused, m_vFlockUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		reinterpret_cast<BehavFlocking*>((*_in->m_vBehaviors.begin()))->SetStats(m_MasterFlock);

		break;

	case eOBJECT_TYPE::Enemy_MineLayer:

		_in->m_AIType = AICompBase::AI_MINELAYER;
		_in->m_pTarget = 0;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = (rand() % 15) + 27;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(2.0f);
		_in->m_fShotTimerMax = 1.7f;
		//_in->m_vBehaviors.push_front()
		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vRandMoveUnused, m_vRandMoveUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		break;

	case eOBJECT_TYPE::Enemy_Dodger:

		_in->m_AIType = AICompBase::AI_DODGE;
		_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = (rand() % 10) + 55;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(1.0f);
		_in->m_fShotTimerMax = 1.5f;
		//_in->m_vBehaviors.push_front()
		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vDodgeUnused, m_vDodgeUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		break;

	case eOBJECT_TYPE::Enemy_Goliath:

		_in->m_AIType = AICompBase::AI_GOLIATH;
		_in->m_pTarget = nullptr;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = (rand() % 10) + 10;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(1.0f);
		_in->m_fShotTimerMax = 1;
		//_in->m_vBehaviors.push_front()
		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vGrowUnused, m_vGrowUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		break;


	case eOBJECT_TYPE::Enemy_FenceLayer:

		_in->m_AIType = AICompBase::AI_FENCELAYER;
		_in->m_pTarget = nullptr;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = (rand() % 10) + 30;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(1.0f);
		_in->m_fShotTimerMax = 1.5f;
		//_in->m_vBehaviors.push_front()
		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vRandMoveUnused, m_vRandMoveUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
// 
		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		break;


	case eOBJECT_TYPE::Enemy_Boss:

		_in->m_AIType = AICompBase::AI_BOSS;
		_in->m_pTarget = nullptr;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = 8;//(rand() % 10) + 10;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		_in->SetTurnSpeed(1.0f);
		_in->m_fShotTimerMax = 1;
		//_in->m_vBehaviors.push_front()
		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vBossUnused, m_vBossUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vShootUnused, m_vShootUnused.begin());
		// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		break;

		///////////////////////////////////////////////////////
		// BULLETS

	case eOBJECT_TYPE::EvilBullet_HeatSeek:
		_in->m_AIType = AICompBase::AI_MISSILE;
		_in->m_pTarget = m_pObjManager->GetPlayer();
		//_in->GetOwner()->SetVelocityInWorldSpace(false);
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 60));
		_in->m_fStartVelocity = 60;
		_in->SetTurnSpeed(0.5f);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vTrackUnused, m_vTrackUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;


	case eOBJECT_TYPE::EvilBullet_Mine:
		_in->m_AIType = AICompBase::AI_MINE;
		_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 0));
		_in->m_fStartVelocity = 0;
		_in->SetTurnSpeed(1);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vProxMineUnused, m_vProxMineUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;

	case eOBJECT_TYPE::EvilBullet_FencePost:
		_in->m_AIType = AICompBase::AI_FENCEPOST;
		_in->m_pTarget = nullptr;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 0));
		_in->m_fStartVelocity = 0;
		_in->SetTurnSpeed(1);

// 		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vProxMineUnused, m_vProxMineUnused.begin());
// 		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;

	case eOBJECT_TYPE::Bullet_Whip:
		_in->m_AIType = AICompBase::AI_WHIP;
		//_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 60));
		_in->m_fStartVelocity = 60;
		_in->SetTurnSpeed(1);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vWhipUnused, m_vWhipUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;

	case eOBJECT_TYPE::Bullet_Missile:
		_in->m_AIType = AICompBase::AI_PLAYER_MISSILE;
		_in->m_pTarget = nullptr;
		//_in->GetOwner()->SetVelocityInWorldSpace(false);
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 70));
		_in->m_fStartVelocity = 70;
		_in->SetTurnSpeed(0.8f);

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vTrackUnused, m_vTrackUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		break;


		///////////////////////////////////////////////////////
		// ENVIRONMENTAL HAZARDS

	case eOBJECT_TYPE::Env_Tornado:
		_in->m_AIType = AICompBase::AI_TORNADO;
		_in->m_pTarget = 0;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = (rand() % 15) + 27;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));
		
		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vEnvMoveUnused, m_vEnvMoveUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;

	case eOBJECT_TYPE::Env_Prominence:
		_in->m_AIType = AICompBase::AI_PROMINENCE;
		_in->m_pTarget = 0;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = 0;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vProminenceUnused, m_vProminenceUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;

	case eOBJECT_TYPE::Env_Prominence_Bullet:
		_in->m_AIType = AICompBase::AI_PROMINENCE_BULLET;
		_in->m_pTarget = 0;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = 25;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vProminenceBulletsUnused, m_vProminenceBulletsUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;

	case eOBJECT_TYPE::Env_Singularity:
		_in->m_AIType = AICompBase::AI_SINGULARITY;
		_in->m_pTarget = 0;// m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(false);
		velocity = 5;
		_in->m_fStartVelocity = (FLOAT)velocity;
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (float)velocity));

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vEnvMoveUnused, m_vEnvMoveUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);
		break;

		//////////////////////////////////////////////////////////////////////////
		// extras

	case eOBJECT_TYPE::Landing_Indicator:
		_in->m_AIType = AICompBase::AI_LANDINGICON;

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vLandPointUnused, m_vLandPointUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);

		break;

	case eOBJECT_TYPE::Orb_Blue:
	case eOBJECT_TYPE::Pickup_Health:
	case eOBJECT_TYPE::Pickup_Shield:

		_in->m_AIType = AICompBase::AI_PICKUP;

		_in->m_pTarget = m_pObjManager->GetPlayer();
		_in->GetOwner()->SetVelocityInWorldSpace(true);
		_in->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 0));
		_in->m_fStartVelocity = 0;

		_in->m_vBehaviors.splice(_in->m_vBehaviors.begin(), m_vTrackUnused, m_vTrackUnused.begin());
		(*_in->m_vBehaviors.begin())->SetOwner(_in);


	default:
		break;

	}
}

void AIManager::CreateKoiAttack(int _numKoi)
{
	if (/*m_KoiAttack->m_vFlock.size() > 0 ||*/ _numKoi <= 0)
		return;

	m_KoiAttack->m_fAlignmentWait = 1;
	m_KoiAttack->m_fCohesionWait = 1;
	m_KoiAttack->m_fSeparationWait = 10;
	m_KoiAttack->m_fFlockRadius = 10;
	m_KoiAttack->m_bDisband = false;
	m_KoiAttack->m_bRotate = false;
	GameObject* newKoi;
	DirectX::XMFLOAT3 startPos(50, 50, 50), helperPos;


	for (int i = 0; i < _numKoi; i++)
	{
		helperPos = startPos;

		helperPos.x += rand() % 20 - 10;
		helperPos.y += rand() % 20 - 10;
		helperPos.z += rand() % 20 - 10;


		newKoi = m_pObjManager->CreateObject(eOBJECT_TYPE::Enemy_Koi, helperPos, "Koi A");

	}

	m_fNumTotalKoiA = (float)_numKoi;
	m_fNumSpawnKoiA = _numKoi;
}

void AIManager::CreateKoiDisrupt(int _numKoi)
{
	if (/*m_KoiDisrupt->m_vFlock.size() > 0 ||*/ _numKoi <= 0)
		return;

	m_KoiDisrupt->m_fAlignmentWait = 2;
	m_KoiDisrupt->m_fCohesionWait = 2;
	m_KoiDisrupt->m_fSeparationWait = 7;
	m_KoiDisrupt->m_fFlockRadius = 7;
	m_KoiDisrupt->m_bDisband = false;
	m_KoiDisrupt->m_bRotate = false;
	GameObject* newKoi;
	DirectX::XMFLOAT3 startPos(70, 70, 70), helperPos;

// 	std::list<GameObject*>& pylonBuckets = m_pObjManager->PassBuckets()[eOBJECT_TYPE::Pylon_Test];
// 	std::list<GameObject*>::iterator bucketIter = pylonBuckets.begin();
// 	int randHelp = rand() % pylonBuckets.size();
// 
// 	while (randHelp > 0)
// 	{
// 		bucketIter++;
// 		randHelp--;
// 	}

	float shotHelper = 0;
	for (int i = 0; i < _numKoi; i++)
	{
		helperPos = startPos;

		helperPos.x += rand() % 20 - 10;
		helperPos.y += rand() % 20 - 10;
		helperPos.z += rand() % 20 - 10;

		shotHelper += 0.2f;

		newKoi = m_pObjManager->CreateObject(eOBJECT_TYPE::Enemy_Disruptor, helperPos);
		reinterpret_cast<AICompBase*>(newKoi->GetComponent("AICompBase"))->SetTarget(*m_pObjManager->PassBuckets()[eOBJECT_TYPE::Player_Obj].begin());
		
		
		reinterpret_cast<AICompBase*>(newKoi->GetComponent("AICompBase"))->SetShotTimer(shotHelper);
		// 		if (i == 0)
		// 			m_KoiDisrupt->m_pLeader = reinterpret_cast<AICompBase*>(newKoi->GetComponent("AICompBase"));
	}

	m_fNumSpawnKoiD = _numKoi; 

}


void AIManager::DisbandSwarm(Flock* _group)
{

	unsigned int numBoids = _group->m_vFlock.size();
	int randHelp = 0;
	std::list<GameObject*>::iterator bucketIter;
	GameObject* tmpHealth = 0;
	if (numBoids == 0 || _group->m_bDisband)
		return;

	switch ((*_group->m_vFlock.begin())->GetAIType())
	{
	case AICompBase::eAITYPE::AI_KOI_A:

	{
		std::list<GameObject*>& pylonBuckets = m_pObjManager->PassBuckets()[eOBJECT_TYPE::Pylon_Test];
		for (m_Iter = _group->m_vFlock.begin(); m_Iter != _group->m_vFlock.end(); m_Iter++)
		{
			bucketIter = pylonBuckets.begin();
			randHelp = rand() % pylonBuckets.size();
			while (randHelp > 0)
			{
				bucketIter++;
				randHelp--;
			}

			(*m_Iter)->SetTarget((*bucketIter));
			(*m_Iter)->m_fShotTimerMax = (*m_Iter)->m_fShotTimerMax * 0.7f;
			
		}
	}

		_group->m_fAlignmentWait = 0;
		_group->m_fCohesionWait = 0;
		_group->m_fSeparationWait = 20;
		_group->m_bDisband = true;
		tmpHealth = m_pObjManager->CreateObject(eOBJECT_TYPE::Pickup_Health, *(*m_KoiAttack->m_vFlock.begin())->GetOwner()->GetTransform().GetPosition());
		tmpHealth->GetTransform().GetWorldMatrix();
		break;

	case AICompBase::eAITYPE::AI_KOI_D:

		for (m_Iter = _group->m_vFlock.begin(); m_Iter != _group->m_vFlock.end(); m_Iter++)
		{
			(*m_Iter)->SetTarget(m_pObjManager->GetPlayer());
			(*m_Iter)->m_fStartVelocity = (*m_Iter)->m_fStartVelocity * 1.6f;
			(*m_Iter)->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (*m_Iter)->m_fStartVelocity));
		}

		_group->m_fAlignmentWait = 1;
		_group->m_fCohesionWait = 1;
		_group->m_fSeparationWait = 6;
		_group->m_bDisband = true;
		_group->m_bRotate = false;
		break;

	default:

		break;
	}
}

void AIManager::CalculateSpawnPosition(AICompBase* _in)
{
	// place enemy in random offset from sphere 
	// TODO: replace hardcoded val with sphere offset variable

	// calculate random normal, multiply offset, becomes new position.  

	string& levelShape = gameData->m_levelShape;

	DirectX::XMFLOAT3 tmpPos;

	if (levelShape == "Torus")
		tmpPos =  CalcTorusPos(_in);
	else if (levelShape == "RoundedCube")
		tmpPos =  CalcCubePos(_in);
	else
		tmpPos = CalcSpherePos(_in);
		
// 	MoleculeEmitter* tmpEmitter = m_pObjManager->m_Molecules->BuildPosEmitter(tmpPos, 100, "Spawn Locale");
// 
// 	m_pObjManager->CreateObject(Landing_Indicator, tmpPos);
// 
// 	if (tmpEmitter != nullptr)
// 	{
// 		tmpEmitter->m_endColor = GetEnemyColor(_in);
// 		//tmpEmitter->m_data.timer += _stagger;
// 	}
// 
// 	return tmpEmitter;
}

MoleculeEmitter* AIManager::AttachTrail(AICompBase* _in)
{

// 	_in->m_pTrail = m_pObjManager->CreateObject(VFX_Ribbon, *_in->GetOwner()->GetTransform().GetPosition(), "Ribbon");
// 	reinterpret_cast<RibbonEffect*>(_in->m_pTrail)->SetParent(_in->GetOwner());
// 
// 	reinterpret_cast<RibbonEffect*>(_in->m_pTrail)->SetLength(TrailLength::VeryLong);
// 	

	MoleculeEmitter* tmpEmitter = m_pObjManager->m_Molecules->BuildEmitter(_in->m_pOwner, 100, "SpawnTrail");

	if (tmpEmitter != nullptr)
	{
		tmpEmitter->m_endColor = GetEnemyColor(_in);
		//tmpEmitter->m_data.timer += _stagger;
	}
	
	return tmpEmitter;
}

void AIManager::HandlePylonDeath(GameObject* _pylon)
{
	int randHelp = 0;
	std::list<GameObject*>::iterator bucketIter;
	GameObject* pylonCatch1 = nullptr, *pylonCatch2 = nullptr;

	for (m_Iter = m_KoiAttack->m_vFlock.begin(); m_Iter != m_KoiAttack->m_vFlock.end(); m_Iter++)
	{
		if ((*m_Iter)->GetTarget()->GetType() == Player_Obj)
			continue;

		if (m_pObjManager->PassBuckets()[Pylon_Test].size() == 0)
			(*m_Iter)->SetTarget(nullptr);

		if (m_pObjManager->PassBuckets()[Pylon_Test].size() == 1)
			(*m_Iter)->SetTarget(*m_pObjManager->PassBuckets()[Pylon_Test].begin());

		else
		{


			randHelp = rand() % 2;
			bucketIter = m_pObjManager->PassBuckets()[eOBJECT_TYPE::Pylon_Test].begin();
			while (randHelp > 0)
			{
				bucketIter++;
				randHelp--;
			}

			(*m_Iter)->SetTarget(*bucketIter);

		}

	}

	randHelp = rand() % 2;
	m_KoiDisrupt->m_bRotate = false;

	for (m_Iter = m_KoiDisrupt->m_vFlock.begin(); m_Iter != m_KoiDisrupt->m_vFlock.end(); m_Iter++)
	{
		if ((*m_Iter)->GetTarget()->GetType() == Player_Obj)
			continue;

		if ((*m_Iter)->GetTarget() != _pylon)
			continue;

		if (m_pObjManager->PassBuckets()[Pylon_Test].size() <= 1)
			(*m_Iter)->SetTarget(nullptr);
	
		if (m_pObjManager->PassBuckets()[Pylon_Test].size() == 2)
		{


			
			bucketIter = m_pObjManager->PassBuckets()[eOBJECT_TYPE::Pylon_Test].begin();
			if ((*bucketIter) == _pylon)
				bucketIter++;

			(*m_Iter)->SetTarget(*bucketIter);

		}

		if (m_pObjManager->PassBuckets()[Pylon_Test].size() == 3)
		{

			
			
			bucketIter = m_pObjManager->PassBuckets()[eOBJECT_TYPE::Pylon_Test].begin();

			if (*bucketIter == _pylon)
			{
				bucketIter++;
				pylonCatch1 = *bucketIter;
				bucketIter++;
				pylonCatch2 = *bucketIter;
			}
			else
			{
				pylonCatch1 = *bucketIter;
				bucketIter++;
				if (*bucketIter == _pylon)
					bucketIter++;

				pylonCatch2 = *bucketIter;
			}


			
			if (randHelp > 0)
			{
				(*m_Iter)->SetTarget(pylonCatch1);
			}
			else
			{
				(*m_Iter)->SetTarget(pylonCatch2);
			}
			

		}

	}

}

void AIManager::BuffSwarm(Flock* _group)
{
	if (_group->m_vFlock.size() == 0 || m_fNumTotalKoiA == 0)
		return;

	for (m_Iter = _group->m_vFlock.begin(); m_Iter != _group->m_vFlock.end(); m_Iter++)
	{
		(*m_Iter)->m_fShotTimerMax = max(KOI_BUFF_SHOOT, (*m_Iter)->m_fShotTimerMax *0.9f);
		(*m_Iter)->m_fStartVelocity += KOI_BUFF_SPEED / m_fNumTotalKoiA;

		(*m_Iter)->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, (*m_Iter)->m_fStartVelocity));
	}
}

DirectX::XMFLOAT3 AIManager::CalcSpherePos(AICompBase* _in)
{
	// radius + offs is 71 (x/y/z)

	XMFLOAT3 newPos;
// 	do
// 	{
// 		newPos = XMFLOAT3(m_UniformDist(m_RandEngine), m_UniformDist(m_RandEngine), m_UniformDist(m_RandEngine));
// 	} while (XMVector3LengthSq(XMLoadFloat3(&newPos)).m128_f32[0] <= 0);
// 
// 	XMStoreFloat3(&newPos, XMVector3Normalize(XMLoadFloat3(&newPos)) * 142.0f);
// 	_in->GetOwner()->GetTransform().SetPosition(newPos);
// 
// 	XMStoreFloat3(&newPos, XMLoadFloat3(&newPos) * 0.5f);

	float xOne;
	float xTwo;

	do
	{
		xOne = m_UniformDist(m_RandEngine);
		xTwo = m_UniformDist(m_RandEngine);

	} while ((xOne * xOne) + (xTwo * xTwo) >= 1);


	//DirectX::XMFLOAT3 newPos;
	newPos.x = 2 * xOne * sqrtf(1.0f - (xOne*xOne) - (xTwo*xTwo));
	newPos.y = 2 * xTwo * sqrtf(1.0f - (xOne*xOne) - (xTwo*xTwo));
	newPos.z = 1 - 2 * ((xOne*xOne) + (xTwo*xTwo));

	newPos.x *= 140;
	newPos.y *= 140;
	newPos.z *= 140;

	//	

	int tmp = 0;

// 	Transform lhs_transform = _in->GetOwner()->GetTransform();
// 
// 
// 
// 
// 	DirectX::XMMATRIX mat = XMLoadFloat4x4(&lhs_transform.GetLocalMatrix());
// 
// 	// Get WORLD-SPACE positions
// 	DirectX::XMFLOAT3 lhs_pos = *lhs_transform.GetPosition();
// 
// 	DirectX::XMVECTOR to_target = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0)), DirectX::XMLoadFloat3(&newPos));
// 
// 	float up_down = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(lhs_transform.GetYAxis()), to_target).m128_f32[0];
// 	float left_right = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(lhs_transform.GetXAxis()), to_target).m128_f32[0];


	_in->GetOwner()->GetTransform().SetPosition(newPos);
	DirectX::XMFLOAT3 landPos;
	CollisionManager::IntersectLine2Triangle(newPos, POS_ORIGIN, tmp, &landPos);

	reinterpret_cast<BehavSpawning*>(*_in->m_vBehaviors.begin())->SetLandingPoint(landPos);

	return landPos;
}

DirectX::XMFLOAT3 AIManager::CalcCubePos(AICompBase* _in)
{
	// size of internal cube is ~55.9289 (x/y/z)
	// radius is 14.0711 + offs = 15.0711 (x/y/z)

	int axis (rand() % 3);
	float negation = (float)(rand() % 2 ? 1.0f : -1.0f);
	XMVECTOR cubePoint =
		XMLoadFloat3(&XMFLOAT3((axis == 0 ? 46.6667f * negation : m_UniformDist(m_RandEngine) * 26.6667f),
							   (axis == 1 ? 46.6667f * negation : m_UniformDist(m_RandEngine) * 26.6667f),
							   (axis == 2 ? 46.6667f * negation : m_UniformDist(m_RandEngine) * 26.6667f)));

	
	// make ship face directly towards surface
	// set landing point directly under 

	// figure out which axis

	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, cubePoint + (XMVector3Normalize(cubePoint) * 90));
	_in->GetOwner()->GetTransform().SetPosition(newPos);

	XMStoreFloat3(&newPos, cubePoint + (XMVector3Normalize(cubePoint) * 23.4518f));
	reinterpret_cast<BehavSpawning*>(*_in->m_vBehaviors.begin())->SetLandingPoint(newPos);

	reinterpret_cast<BehavSpawning*>(*_in->m_vBehaviors.begin())->cubeOrientation =  true;

// 	switch (axis)
// 	{
// 	case 0:
		
// 		break;
// 	case 1:
// 		reinterpret_cast<BehavSpawning*>(*_in->m_vBehaviors.begin())->cubeOrientation.y = -1 * negation;
// 		break;
// 	case 2:
// 		reinterpret_cast<BehavSpawning*>(*_in->m_vBehaviors.begin())->cubeOrientation.z = -1 * negation;
// 		break;

// 	}


	return newPos;
}

DirectX::XMFLOAT3 AIManager::CalcTorusPos(AICompBase* _in)
{
	// major radius is 112.5 (x/z)
	// section radius + offs is 46 (y)

	_in->GetOwner()->GetTransform().ResetTransform();

	XMFLOAT3 randRad, randSecRad, newPos;
	XMVECTOR secRadPerp;
	do
	{
		randRad = XMFLOAT3(m_UniformDist(m_RandEngine), 0, m_UniformDist(m_RandEngine));
	} while (XMVector3Length(XMLoadFloat3(&randRad)).m128_f32[0] <= 0);

	XMStoreFloat3(&randRad, XMVector3Normalize(XMLoadFloat3(&randRad)) * 112.5f);
	XMStoreFloat3(&randSecRad, XMVector3Normalize(XMLoadFloat3(&randRad)));

	secRadPerp = XMVector3Rotate(XMLoadFloat3(&randSecRad), XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), XM_PIDIV2));
	XMStoreFloat3(&randSecRad, XMVector3Rotate(XMLoadFloat3(&randSecRad), XMQuaternionRotationAxis(secRadPerp, m_UniformDist(m_RandEngine) * XM_2PI)));

	XMStoreFloat3(&newPos, XMLoadFloat3(&randRad) + (XMLoadFloat3(&randSecRad) * 92));
	_in->GetOwner()->GetTransform().SetPosition(newPos);

	XMStoreFloat3(&newPos, XMLoadFloat3(&randRad) + (XMLoadFloat3(&randSecRad) * 46));
	reinterpret_cast<BehavSpawning*>(*_in->m_vBehaviors.begin())->SetLandingPoint(newPos);

	return newPos;
}

DirectX::XMFLOAT4 AIManager::GetEnemyColor(AICompBase* _in)
{
	switch (_in->GetOwner()->GetType())
	{
	case eOBJECT_TYPE::Enemy_Merc:
		 return DirectX::XMFLOAT4(0.8f, 0.0f, 0.8f, 1);
		break;

	case eOBJECT_TYPE::Enemy_Bomber:
		return DirectX::XMFLOAT4(1, 0, 0, 1);
		break;

	case eOBJECT_TYPE::Enemy_Koi:
		return DirectX::XMFLOAT4(0.2f, 0.3f, 0.9f, 1);
		break;

	case eOBJECT_TYPE::Enemy_Disruptor:
		return DirectX::XMFLOAT4(0.9f, 0.6f, 0.2f, 1);
		break;

	case eOBJECT_TYPE::Enemy_MineLayer:
		return DirectX::XMFLOAT4(0.9f, 0.6f, 0.9f, 1);
		break;

	case eOBJECT_TYPE::Enemy_Dodger:
		return DirectX::XMFLOAT4(0.4f, 0.9f, 0.1f, 1);
		break;

	case eOBJECT_TYPE::Enemy_Goliath:
		return DirectX::XMFLOAT4(0.9f, 0.9f, 0.1f, 1);
		break;

	case eOBJECT_TYPE::Enemy_FenceLayer:
		return DirectX::XMFLOAT4(0.2f, 0.8f, 0.9f, 1);
		break;

		default:
	//case eOBJECT_TYPE::Enemy_Boss:
		return DirectX::XMFLOAT4(1, 1, 1, 1);

	
		//std::cout << "wrong ai type for ribon spawn\n";
		break;
	}
}

