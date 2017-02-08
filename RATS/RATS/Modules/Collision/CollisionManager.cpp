#include "stdafx.h"
#include "CollisionManager.h"
#include <DirectXMath.h>
#include "../Object Manager/GameObject.h"
#include "../Renderer/D3DGraphics.h"
#include "ColliderComp.h"
#include "../Components/HealthComponent.h"
#include "../Audio/FFmod.h"
#include "../Object Manager/ObjectManager.h"
#include "../AI/AICompBase.h"
#include "../Upgrade System/GameData.h"
#include "../Molecules/MoleculeManager.h"
#include "../Input/ThreeSixty.h"
#include "../Achievements/AchManager.h"


extern D3DGraphics* globalGraphicsPointer;
extern 	FFmod*		g_AudioSystem;
extern GameData *gameData;
extern ThreeSixty* gamePad;
extern CAchManager* g_SteamAchievements;



CollisionManager::CollisionManager()
{
}


CollisionManager::~CollisionManager()
{
}

bool CollisionManager::Sphere2Sphere(ColliderComp& _lhs, ColliderComp& _rhs)
{
	// TODO: Remove
	//return false;

	DirectX::XMVECTOR lhPos = DirectX::XMLoadFloat3(&_lhs.GetStart()), rhPos = DirectX::XMLoadFloat3(&_rhs.GetStart()), vecBetwix;
	vecBetwix = DirectX::XMVectorSubtract(rhPos, lhPos);
	float sumOfRadii = _lhs.GetRadius() + _rhs.GetRadius();
	DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(vecBetwix, vecBetwix);

	if ( abs(dotResult.m128_f32[0]) < sumOfRadii * sumOfRadii)
		return true;

	return false;
}

bool CollisionManager::Sphere2Capsule(ColliderComp& _lhs, ColliderComp& _rhs)
{
	//TODO: remove
	return false;
}

bool CollisionManager::Sphere2AABB(ColliderComp& _lhs, ColliderComp& _rhs)
{
	// TODO: remove
	return false;
}

bool CollisionManager::Capsule2AABB(ColliderComp& _lhs, ColliderComp& _rhs)
{
	//TODO: remove
	return false;
}

bool CollisionManager::IntersectLine2Triangle(const DirectX::XMFLOAT3& _start, const DirectX::XMFLOAT3& _end, int& _triIndex, DirectX::XMFLOAT3* _out)
{
	using namespace DirectX;

	// TODO: Remove once real mesh is in
	//return false;
	/////////////////////////////////

	bool bWaldo = false;
	XMVECTOR v3NewEnd = XMLoadFloat3(&_end), L, v3CollPoint, tmpEdge, tmpNormal;
	float fD1, fD2, fD3, D3, D2, DF = 0;

	const Mesh* gridShape = globalGraphicsPointer->GetMesh(gameData->m_levelShape);
	const POSUVNRM_VERTEX* Verts = gridShape->m_vertexArray;
	const unsigned int* Indicies = gridShape->m_indexArray;

	XMVECTOR vStart = XMLoadFloat3(&_start);
	
	XMVECTOR vTriNormHelper1, vTriHelperNorm2, p0, p1, p2, vTriNorm;

	for (UINT i = 0; i < gridShape->m_numIndices / 3; i++)
	{
		//DirectX::XMVectorSubtract()
		// FIND TRI NORMAL FIRST						// B - A
		
		p0 = XMLoadFloat4(&(Verts[Indicies[(i * 3)]].position));
		p1 = XMLoadFloat4(&(Verts[Indicies[(i * 3) + 1]].position));
		p2 = XMLoadFloat4(&(Verts[Indicies[(i * 3) + 2]].position));

		vTriNormHelper1 = XMVectorSubtract(p1, p0);
		vTriHelperNorm2 = XMVectorSubtract(p2, p0);

		vTriNorm = XMVector3Normalize(XMVector3Cross(vTriNormHelper1, vTriHelperNorm2));
		

 		fD1 = XMVector3Dot(vStart, vTriNorm).m128_f32[0];
 				
		fD2 = XMVector3Dot(p0, vTriNorm).m128_f32[0];
		fD3 = XMVector3Dot(v3NewEnd, vTriNorm).m128_f32[0];
 		
 				// EARLY OUTS
 		
 				if ((fD1 - fD2) < 0)
 					continue;
 		
 				if ((fD3 - fD2) > 0)
 					continue;
 		
 				D2 = fD1 - fD2;
 		
 				L = v3NewEnd - vStart;
 		
				D3 = XMVector3Dot(vTriNorm, L).m128_f32[0];
 		
 				if (D3)
 					DF = -(D2 / D3);
 		
 				v3CollPoint = vStart + (L * DF);
 		
 				//HALF SPACE TESTS
 		
 		
 				// Edge0
 				tmpEdge = p1 - p0;
				tmpNormal = XMVector3Cross( tmpEdge, vTriNorm);
 		
 				XMVECTOR tmpV = v3CollPoint - p0;
				float tmpD = XMVector3Dot(tmpNormal, tmpV).m128_f32[0];
 				if (tmpD >= 0)
 					continue;
 		
 				// EDGE 1
 		
 				tmpEdge = p2 - p1;
 				tmpNormal = XMVector3Cross(tmpEdge, vTriNorm);
 		
 				tmpV = v3CollPoint - p1;
 				tmpD = XMVector3Dot(tmpNormal, tmpV).m128_f32[0];
 				if (tmpD >= 0)
 					continue;
 		
 				// EDGE 2
 		
 				tmpEdge = p0 - p2;
 				tmpNormal = XMVector3Cross(tmpEdge, vTriNorm);
 		
 		
 				tmpV = v3CollPoint - p2;
 				tmpD = XMVector3Dot(tmpNormal, tmpV).m128_f32[0];
 				if (tmpD < 0)
 				{
 					bWaldo = true;
					v3NewEnd = v3CollPoint;
 					XMStoreFloat3(_out, v3NewEnd);
 					_triIndex = i;
 				}

	}




	return bWaldo;



}

void CollisionManager::CheckCollisions(std::vector<std::list<GameObject*>>* _masterList, float _dt)
{
	using namespace DirectX;

	// Player vs enemy bullet
	// Player vs enemy
	// Player vs Orb
	// Player vs Powerup
	// Player vs Pylon
	// Player vs Environmental
	// Enemy vs player bullet
	// Enemy vs pylon
	// Prominence Bullet vs Everything


	// Helper Vars
	fLastDT = _dt;
	int nIndex = 0;
	if ((*_masterList)[eOBJECT_TYPE::Player_Obj].empty())
		return;

	GameObject* pPlayer = (*(*_masterList)[eOBJECT_TYPE::Player_Obj].begin());
	ColliderComp lhs;
	ColliderComp rhs;
	ColliderComp phs;
	// ---------------

#pragma region PlayerVsX

	phs.m_fRadius = reinterpret_cast<ColliderComp*>(pPlayer->GetComponent("ColliderComp"))->m_fRadius;
	phs.m_Start = *pPlayer->GetTransform().GetPosition();
	phs.m_pOwner = pPlayer;

	// Player vs enemy bullet
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_Simple].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_Simple].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}

	for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_Disrupt].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_Disrupt].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}

	for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}


	for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_Mine].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_Mine].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fAggroRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReactionAggre(phs, rhs);
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReaction(phs, rhs);
			}

		}
	}

	////////////////

	// I decided to move the collision detection call between player and enemy into the enemy section,
	// because if I place it here then there will be two different iterations into the enemy lists, 
	// and thus redundant.  

// 	Player vs enemy
// 		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Merc].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Merc].end(); m_Iter++)
// 		{
// 			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
// 			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
// 			rhs.m_pOwner = (*m_Iter);
// 	
// 			if (Sphere2Sphere(phs, rhs))
// 			{
// 				HandleReaction(phs, rhs);
// 			}
// 		}

	//////////////////

	// Player vs orb
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Orb_Blue].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Orb_Blue].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fAggroRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReactionAggre(phs, rhs);
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReaction(phs, rhs);
			}

		}
	}

	
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Orb_Red].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Orb_Red].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fAggroRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReactionAggre(phs, rhs);
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReaction(phs, rhs);
			}

		}
	}

	
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Orb_Yellow].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Orb_Yellow].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fAggroRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReactionAggre(phs, rhs);
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReaction(phs, rhs);
			}

		}
	}
	//////////////////////////////


	// Player vs Pickups
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pickup_Health].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pickup_Health].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fAggroRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReactionAggre(phs, rhs);
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReaction(phs, rhs);
			}

		}
	}

	// Player vs Pickups
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pickup_Shield].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pickup_Shield].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fAggroRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReactionAggre(phs, rhs);
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReaction(phs, rhs);
			}

		}
	}


	//////////////////////////////////////////////////////////////////////////

	// Player Vs Pylon - 
 	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Test].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Test].end(); m_Iter++)
 	{
 		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
 		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
 		rhs.m_pOwner = (*m_Iter);
 
 		if (Sphere2Sphere(phs, rhs))
 		{
 			HandleReaction(phs, rhs);
 		}
 	}

	// Player Vs Pylon - 
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}


	// Player Vs Pylon - 
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}


	// Player Vs Pylon - 
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}


	// Player Vs Hazard - 
	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}

	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
	{
		rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		rhs.m_pOwner = (*m_Iter);

		if (Sphere2Sphere(phs, rhs))
		{
			HandleReaction(phs, rhs);
		}
	}

#pragma endregion

#pragma region EnemyVsX

	std::list<GameObject*>::iterator enemyIter;


	// Boss

	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_Boss].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_Boss].end(); enemyIter++)
	{
		if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
			continue;

		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		if (reinterpret_cast<HealthComponent*>(lhs.m_pOwner->GetComponent("HealthComponent"))->GetInvincibility())
			lhs.m_fRadius = lhs.m_fRadius * 1.5f;

		// Start by checking against the player, then check other objects
		if (Sphere2Sphere(lhs, phs))
		{
			HandleReaction(lhs, phs);
		}
		// small, but hopefully useful optimization


		// boss vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// boss vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// boss vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// boss vs evil missiles
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////

		// boss vs Pylon


		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

// 		Merc vs Hazard
// 				for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
// 				{
// 					rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
// 					rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
// 					rhs.m_pOwner = (*m_Iter);
// 		
// 					if (Sphere2Sphere(lhs, rhs))
// 					{
// 						HandleReaction(lhs, rhs);
// 					}
// 				}
// 		
// 				for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
// 				{
// 					rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
// 					rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
// 					rhs.m_pOwner = (*m_Iter);
// 		
// 					if (Sphere2Sphere(lhs, rhs))
// 					{
// 						HandleReaction(lhs, rhs);
// 					}
// 				}

		//////////////////////////////////////////////////////////////////////////


	}






	// Merc

	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_Merc].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_Merc].end(); enemyIter++)
	{
		if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
			continue;

		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		// Start by checking against the player, then check other objects
		if (Sphere2Sphere(lhs, phs))
		{
			HandleReaction(lhs, phs);
		}
		// small, but hopefully useful optimization


		// Merc vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// Merc vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// merc vs evil missiles
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////

		// Merc vs Pylon
		

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// Merc vs Hazard
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////////////////////////////////////////////////////


	}


	// KOI A
	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_Koi].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_Koi].end(); enemyIter++)
	{

		if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
			continue;
		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		// Start by checking against the player, then check other objects
		if (Sphere2Sphere(lhs, phs))
		{
			HandleReaction(lhs, phs);
		}
		// small, but hopefully useful optimization


		// Koi vs other koi
		// didn't think this was needed, but the attack koi shoot weird do to separation code,
		// so I need a basic pushback for th ekoi
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Koi].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Koi].end(); m_Iter++)
		{
			if ((*m_Iter) == (*enemyIter))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}


		// Koi vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// koi vs evil missiles
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////

		// Koi vs Pylon


		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// Koi A - Hazard
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
		//////////////////////////////////////////////////////////////////////////


	}

	// KOI Disrupt
	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].end(); enemyIter++)
	{
		if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
			continue;

		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		// Start by checking against the player, then check other objects
		if (Sphere2Sphere(lhs, phs))
		{
			HandleReaction(lhs, phs);
		}
		// small, but hopefully useful optimization


		// Dsiruptor vs other Disrupt
		// didn't think this was needed, but the attack koi shoot weird do to separation code,
		// so I need a basic pushback for th ekoi
		// at this rate, i'm gonna have to do this for everything
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].end(); m_Iter++)
		{
			if ((*m_Iter) == (*enemyIter))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// disruptor vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}


		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// koi vs evil missiles
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////

		// diruptor vs Pylon


		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Test].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Test].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// koi disrupt - hazard
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////////////////////////////////////////////////////


	}

	// Mine Layer
	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_MineLayer].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_MineLayer].end(); enemyIter++)
	{
		if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
			continue;

		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		// Start by checking against the player, then check other objects
		if (Sphere2Sphere(lhs, phs))
		{
			HandleReaction(lhs, phs);
		}
		// small, but hopefully useful optimization


		// disruptor vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// mine lyaer vs evil missiles
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////

		// minelayer vs Pylon


		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// Minelayer vs hazard
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}


	// Goliath
	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_Goliath].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_Goliath].end(); enemyIter++)
	{
		if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
			continue;

		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		// Start by checking against the player, then check other objects
		if (Sphere2Sphere(lhs, phs))
		{
			HandleReaction(lhs, phs);
		}
		// small, but hopefully useful optimization


		// disruptor vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// mine lyaer vs evil missiles
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Orb_Blue].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Orb_Blue].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// Goliath vs Pylon
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Test].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Test].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// Minelayer vs hazard
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}

		// DODGER
		for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_Dodger].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_Dodger].end(); enemyIter++)
		{
			if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
				continue;

			lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
			lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
			lhs.m_pOwner = (*enemyIter);

			// Start by checking against the player, then check other objects
			if (Sphere2Sphere(lhs, phs))
			{
				HandleReaction(lhs, phs);
			}
			// small, but hopefully useful optimization


			// Dodger vs Player Bullet
			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
			{
				
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fAggroRadius;
				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReactionAggre(lhs, rhs);
					lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
					if (Sphere2Sphere(lhs, rhs))
						HandleReaction(lhs, rhs);
				}
			}

			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
			{

				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fAggroRadius;
				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReactionAggre(lhs, rhs);
					lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
					if (Sphere2Sphere(lhs, rhs))
						HandleReaction(lhs, rhs);
				}
			}

			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
			{
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fAggroRadius;
				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReactionAggre(lhs, rhs);
					lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
					if (Sphere2Sphere(lhs, rhs))
						HandleReaction(lhs, rhs);
				}
			}

			// mine lyaer vs evil missiles
			for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
			{
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}
			//////////////////////////////////////////////////////////////////////////

			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Dodger].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Dodger].end(); m_Iter++)
			{
				if ((*enemyIter) == (*m_Iter))
					continue;

				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}

			//////////////////////////

			// dodger vs Pylon


			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
			{
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}

			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
			{
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}

			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
			{
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}

			// dodger vs hazard
			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
			{
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}
			for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
			{
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
				rhs.m_pOwner = (*m_Iter);

				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}

			//////////////////////////////////////////////////////////////////////////
	}

	// Bomber
	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Enemy_Bomber].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Enemy_Bomber].end(); enemyIter++)
	{
		if ((*enemyIter)->GetFlag(eSTATUS_FLAGS::Spwaning))
			continue;

		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		// Start by checking against the player, then check other objects
		if (Sphere2Sphere(lhs, phs))
		{
			HandleReaction(lhs, phs);
		}
		// small, but hopefully useful optimization


		// Bomber vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// bomber vs evil missiles
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////

		// Bomber vs Pylon


		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Test].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Test].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Lambda].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Sigma].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Omega].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Omega].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		// Bomber vs hazard
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		//////////////////////////////////////////////////////////////////////////


	}
	

#pragma endregion

#pragma region PylonVsX

	for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pylon_Test].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pylon_Test].end(); m_Iter++)
	{
		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
		lhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*m_Iter);

		for (enemyIter = (*_masterList)[eOBJECT_TYPE::EvilBullet_Simple].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::EvilBullet_Simple].end(); enemyIter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*enemyIter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (enemyIter = (*_masterList)[eOBJECT_TYPE::EvilBullet_Disrupt].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::EvilBullet_Disrupt].end(); enemyIter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*enemyIter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
	}

#pragma endregion

#pragma region Env_Singularity

	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Env_Singularity].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Env_Singularity].end(); enemyIter++)
	{
		phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
		phs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		phs.m_pOwner = (*enemyIter);

		//Singularity vs Player
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Player_Obj].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Player_Obj].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		

		//Singularity vs Merc
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Merc].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Merc].end(); m_Iter++)
		{

			if ((*m_Iter)->GetFlag(Spwaning))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();


			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//Singularity vs Merc Missile
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::EvilBullet_HeatSeek].end(); m_Iter++)
		{

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();


			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
		\

		//Singularity vs Bomber
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Bomber].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Bomber].end(); m_Iter++)
		{
			if ((*m_Iter)->GetFlag(Spwaning))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();

		//Singularity vs Koi
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Koi].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Koi].end(); m_Iter++)
		{
			if ((*m_Iter)->GetFlag(Spwaning))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();

		//Singularity vs Disruptor
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].end(); m_Iter++)
		{
			if ((*m_Iter)->GetFlag(Spwaning))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();

		//Singularity vs Minelayer
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_MineLayer].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_MineLayer].end(); m_Iter++)
		{
			if ((*m_Iter)->GetFlag(Spwaning))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//Singularity vs Dodger
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Dodger].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Dodger].end(); m_Iter++)
		{
			if ((*m_Iter)->GetFlag(Spwaning))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//Singularity vs Goliath
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Goliath].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Goliath].end(); m_Iter++)
		{
			if ((*m_Iter)->GetFlag(Spwaning))
				continue;

			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();

		//Singularity vs Orb
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Orb_Blue].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Orb_Blue].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();

		//Singularity vs Health Pickup
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pickup_Health].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pickup_Health].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
	}
#pragma endregion

#pragma region Env_Prominence_Bullet

	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Env_Prominence_Bullet].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Env_Prominence_Bullet].end(); enemyIter++)
	{
		phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
		phs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		phs.m_pOwner = (*enemyIter);

		//Prominence vs Player
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Player_Obj].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Player_Obj].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//Prominence vs Merc
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Merc].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Merc].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
		//Prominence vs Bomber
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Bomber].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Bomber].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
		//Prominence vs Koi
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Koi].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Koi].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
		//Prominence vs Disruptor
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_Disruptor].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
		//Prominence vs Minelayer
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Enemy_MineLayer].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Enemy_MineLayer].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}

		//Prominece vs Orb
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Orb_Blue].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Orb_Blue].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
		//Prominence vs Health Pickup
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pickup_Health].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pickup_Health].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
	}

#pragma endregion

#pragma region Misc

	//Tornados
	for (enemyIter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); enemyIter++)
	{
		phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
		phs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		phs.m_pOwner = (*enemyIter);

		//Tornado vs Orb
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Orb_Blue].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Orb_Blue].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
		//Tornado vs Health Pickup
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Pickup_Health].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Pickup_Health].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->GetRadius();
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(phs, rhs))
			{
				HandleReactionAggre(phs, rhs);
				rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
				phs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetRadius();
				if (Sphere2Sphere(phs, rhs))
				{
					HandleReaction(phs, rhs);
				}
			}
		}
	}

	// Mines 
	for (enemyIter = (*_masterList)[eOBJECT_TYPE::EvilBullet_Mine].begin(); enemyIter != (*_masterList)[eOBJECT_TYPE::EvilBullet_Mine].end(); enemyIter++)
	{
		lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
		lhs.m_Start = *(*enemyIter)->GetTransform().GetPosition();
		lhs.m_pOwner = (*enemyIter);

		// Mine vs Player Bullet
		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Simple].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Simple].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(lhs, rhs))
			{
				//HandleReaction(lhs, rhs);
				HandleReactionAggre(lhs, rhs);
				lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Whip].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Whip].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(lhs, rhs))
			{
				//HandleReaction(lhs, rhs);
				HandleReactionAggre(lhs, rhs);
				lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Spread].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Spread].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(lhs, rhs))
			{
				//HandleReaction(lhs, rhs);
				HandleReactionAggre(lhs, rhs);
				lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Bullet_Missile].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Bullet_Missile].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->GetAggroRadius();
			if (Sphere2Sphere(lhs, rhs))
			{
				//HandleReaction(lhs, rhs);
				HandleReactionAggre(lhs, rhs);
				lhs.m_fRadius = reinterpret_cast<ColliderComp*>((*enemyIter)->GetComponent("ColliderComp"))->m_fRadius;
				if (Sphere2Sphere(lhs, rhs))
				{
					HandleReaction(lhs, rhs);
				}
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Tornado].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Tornado].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}

		for (m_Iter = (*_masterList)[eOBJECT_TYPE::Env_Prominence].begin(); m_Iter != (*_masterList)[eOBJECT_TYPE::Env_Prominence].end(); m_Iter++)
		{
			rhs.m_fRadius = reinterpret_cast<ColliderComp*>((*m_Iter)->GetComponent("ColliderComp"))->m_fRadius;
			rhs.m_Start = *(*m_Iter)->GetTransform().GetPosition();
			rhs.m_pOwner = (*m_Iter);

			if (Sphere2Sphere(lhs, rhs))
			{
				HandleReaction(lhs, rhs);
			}
		}
	}


#pragma endregion

// 	for (int nBucket = 0; nBucket < eOBJECT_TYPE::MAX_obj; nBucket++)
// 	{
// 
// 	}
}

void CollisionManager::HandleReaction(ColliderComp& _lhs, ColliderComp& _rhs)
{
	// helper vars cause switch statments are finicky

	HealthComponent* pStats;
	HealthComponent* pylonStats;
	bool ExtraUmph = false;
	// Player vs x

	if (_lhs.m_pOwner->GetType() == Player_Obj)
	{

		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::EvilBullet_Simple:
		case eOBJECT_TYPE::EvilBullet_Disrupt:
			_rhs.m_pOwner->SetDead(true);

			if (_lhs.m_pOwner->IsShielded())
				break;

			if (_lhs.m_pOwner->TakeDamage(15 * gameData->m_PlayerShip.resilienceScalar))
			{
				g_AudioSystem->PlaySound("RATS_PlayerHit", _lhs.m_pOwner);
				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 0.3f, 0.2f);
				}
				//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_RATS_PLAYERHIT, &_lhs.m_pOwner->GetTransform());// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		
			

		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			_rhs.m_pOwner->SetDead(true);

			if (_lhs.m_pOwner->IsShielded())
				break;

			if (_lhs.m_pOwner->TakeDamage(50 * gameData->m_PlayerShip.resilienceScalar))
			{
				g_AudioSystem->PlaySound("RATS_PlayerHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 0.3f, 0.5f);
				}
			}
			break;

		case eOBJECT_TYPE::EvilBullet_Mine:
			//_lhs.m_pOwner->TakeDamage(25);
			_rhs.m_pOwner->SetDead(true);

			if (_lhs.m_pOwner->IsShielded())
				break;

			if (_lhs.m_pOwner->TakeDamage(20 * gameData->m_PlayerShip.resilienceScalar))
			{
				g_AudioSystem->PlaySound("RATS_PlayerHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 0.3f, 0.2f);
				}
			}
			break;

// 		case eOBJECT_TYPE::Enemy_Merc:
// 		case eOBJECT_TYPE::Enemy_Koi:
// 		case eOBJECT_TYPE::Enemy_Bomber:
// 
// 			break;

		case eOBJECT_TYPE::Orb_Blue:
		case eOBJECT_TYPE::Orb_Red:
		case eOBJECT_TYPE::Orb_Yellow:
			pStats = reinterpret_cast<HealthComponent*>(_lhs.m_pOwner->GetComponent("HealthComponent"));

			if (pStats->GetEnergy() == pStats->GetMaxEnergy())
				break;

			pStats->SetEnergy(min(pStats->GetMaxEnergy(), pStats->GetEnergy() + 15));
			_rhs.GetOwner()->SetDead(true);
			g_AudioSystem->PlaySound("RATS_Pickup", _lhs.m_pOwner);
			break;

		case eOBJECT_TYPE::Pickup_Health:
			pStats = reinterpret_cast<HealthComponent*>(_lhs.m_pOwner->GetComponent("HealthComponent"));

			if (pStats->GetHealth() == pStats->GetMaxHealth())
				break;

			pStats->SetHealth(min(pStats->GetMaxHealth(), pStats->GetHealth() + 20));
			_rhs.GetOwner()->SetDead(true);
			g_AudioSystem->PlaySound("RATS_Pickup", _lhs.m_pOwner);
			break;

		case eOBJECT_TYPE::Pickup_Shield:
			_rhs.GetOwner()->SetDead(true);
			_lhs.m_pOwner->SetShielded(true);
			globalGraphicsPointer->SetPlayerShieldShowing(true);
			g_AudioSystem->PlaySound("RATS_Pickup", _lhs.m_pOwner);
			break;

		case eOBJECT_TYPE::Pylon_Test:
			//std::cout << "leech\n";
			//TODO: transfer energy from player to pylon

			pStats= reinterpret_cast<HealthComponent*>(_lhs.m_pOwner->GetComponent("HealthComponent"));
			 pylonStats= reinterpret_cast<HealthComponent*>(_rhs.m_pOwner->GetComponent("HealthComponent"));

			
			 if (pStats->GetEnergy() > 0 && pylonStats->GetHealth() < pylonStats->GetMaxHealth() && pylonStats->GetEnergy() == 0)
			{
				 float fEnergyHelper = min(pStats->GetEnergy(), pylonStats->GetMaxHealth() - pylonStats->GetHealth());		
				 pylonStats->SetEnergy(fEnergyHelper);

				 if (pylonStats->GetHealth() <= pylonStats->GetMaxHealth() * 0.1f)
				 {
					 pylonStats->SetHealth(pylonStats->GetMaxHealth() * 0.12f);
				 }

				g_AudioSystem->PlaySound("RATS_PylonCharging", _rhs.m_pOwner);

				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 1, 0.4f);
				}

			}
			
		

			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:
			
			PushBackStatic(_rhs, _lhs);

			break;
		
		case eOBJECT_TYPE::Env_Tornado:

			if (_lhs.m_pOwner->IsShielded())
				break;

			if (_lhs.m_pOwner->TakeDamage(5 * gameData->m_PlayerShip.resilienceScalar))
			{
				g_AudioSystem->PlaySound("RATS_PlayerHit", _lhs.m_pOwner);
				_lhs.m_pOwner->SpinOut();
				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 0.3f, 0.2f);
				}
				//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_RATS_PLAYERHIT, &_lhs.m_pOwner->GetTransform());// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
		
			break;


		case eOBJECT_TYPE::Env_Prominence:
			if (_lhs.m_pOwner->IsShielded())
				break;

			if (_lhs.m_pOwner->TakeDamage(5 * gameData->m_PlayerShip.resilienceScalar))
			{
				g_AudioSystem->PlaySound("RATS_PlayerHit", _lhs.m_pOwner);
				_lhs.m_pOwner->Ignite();
				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 2, 0.2f);
				}
				//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_RATS_PLAYERHIT, &_lhs.m_pOwner->GetTransform());// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;


		default:
			break;
		}

		return;
	}

	if (_lhs.m_pOwner->GetType() == Enemy_Merc)
	{
		switch (_rhs.m_pOwner->GetType())
		{	
		case eOBJECT_TYPE::Player_Obj:

			 ExtraUmph = _rhs.GetOwner()->GetFlag(Dashing);

			PushBack(_lhs, _rhs, ExtraUmph);

			
				if (_lhs.m_pOwner->TakeDamage(ExtraUmph ? 10.0f : 2.0f))
				{
					g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

				}
			
				if (!_rhs.m_pOwner->IsShielded())
				{
					if (_rhs.m_pOwner->TakeDamage(2))
					{
						g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
						if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
						{
							gamePad->Vibrate(0, 0.2f, 0.2f);
						}
					}
				}
			break;
			
		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			if (reinterpret_cast<AICompBase*>(_rhs.GetOwner()->GetComponent("AICompBase"))->GetShooter() == reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase")))
				break;
			
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(35))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);

			break;
		
		case Env_Prominence:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->Ignite();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case Env_Tornado:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->SpinOut();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;

		default:
			break;
		}
		
		return;
	}

	if (_lhs.m_pOwner->GetType() == Enemy_Boss)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:

			PushBackStatic(_lhs, _rhs);


			if (_lhs.m_pOwner->TakeDamage(2))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}

			if (!_rhs.m_pOwner->IsShielded())
			{
				if (_rhs.m_pOwner->TakeDamage(2))
				{
					g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

				}
			}
			break;

		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

// 		case eOBJECT_TYPE::EvilBullet_HeatSeek:
// 			//_lhs.m_pOwner->TakeDamage(50);
// // 			if (reinterpret_cast<AICompBase*>(_rhs.GetOwner()->GetComponent("AICompBase"))->GetShooter() == reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase")))
// // 				break;
// 
// 			_rhs.m_pOwner->SetDead(true);
// 			if (_lhs.m_pOwner->TakeDamage(35))
// 			{
// 				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
// 			}
// 			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);

			break;

// 		case Env_Prominence:
// 		case Env_Tornado:
// 			if (_lhs.m_pOwner->TakeDamage(5))
// 			{
// 				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
// 			}
// 			break;

		default:
			break;
		}

		return;
	}

	if (_lhs.m_pOwner->GetType() == Enemy_Goliath)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:

			 ExtraUmph = _rhs.GetOwner()->GetFlag(Dashing);

			PushBack(_lhs, _rhs, ExtraUmph);


			if (_lhs.m_pOwner->TakeDamage(ExtraUmph ? 10.0f : 2.0f))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}

			if (!_rhs.m_pOwner->IsShielded())
			{
				if (_rhs.m_pOwner->TakeDamage(2))
				{
					g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
					if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
					{
						gamePad->Vibrate(0, 0.2f, 0.2f);
					}
				}
			}
			break;

		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			

			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(35))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);

			break;

		case Env_Prominence:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->Ignite();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case Env_Tornado:
		
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->SpinOut();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;

		case eOBJECT_TYPE::Orb_Blue:
		case eOBJECT_TYPE::Orb_Yellow:
		case eOBJECT_TYPE::Orb_Red:
			_rhs.GetOwner()->SetDead(true);
			reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase"))->SetAssimilate(true);

			break;

		case eOBJECT_TYPE::Pylon_Test:
			if (_rhs.GetOwner() == reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase"))->GetTarget())
			{
				reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase"))->SetAssimilate(true);
				reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase"))->SetTarget(nullptr);
				
				pylonStats = reinterpret_cast<HealthComponent*>(_rhs.GetOwner()->GetComponent("HealthComponent"));
				pylonStats->SetHealth(pylonStats->GetHealth() - (pylonStats->GetMaxHealth() * 0.2f));
			}
			

			break;

		default:
			break;
		}

		return;
	}

	if (_lhs.m_pOwner->GetType() == Enemy_Koi)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:
			
			 ExtraUmph = _rhs.GetOwner()->GetFlag(Dashing);

			PushBack(_lhs, _rhs, ExtraUmph);

			
			if (_lhs.m_pOwner->TakeDamage(ExtraUmph ? 10.0f : 2.0f))
				{
					g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				}
			
				if (!_rhs.m_pOwner->IsShielded())
				{

				
			if (_rhs.m_pOwner->TakeDamage(2))
			{
				g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 0.2f, 0.2f);
				}
			}
				}
			break;

		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;


		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(25))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);

			break;
		case Env_Prominence:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->Ignite();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case Env_Tornado:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->SpinOut();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		

		case Enemy_Koi:
			PushBack(_lhs, _rhs);
			break;

		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == Enemy_Disruptor)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:
			
			 ExtraUmph = _rhs.GetOwner()->GetFlag(Dashing);

			PushBack(_lhs, _rhs, ExtraUmph);

			

			if (_lhs.m_pOwner->TakeDamage(ExtraUmph ? 10.0f : 2.0f))
				{
					g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				}

		if (!_rhs.m_pOwner->IsShielded())
		{


			if (_rhs.m_pOwner->TakeDamage(2))
			{
				g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
				{
					gamePad->Vibrate(0, 0.2f, 0.2f);
				}
			}
		}
			break;

		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(25))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			}
			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);

			break;

		case Env_Prominence:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->Ignite();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case Env_Tornado:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->SpinOut();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;

		case Enemy_Disruptor:
			PushBack(_lhs, _rhs);
			break;

		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == Enemy_MineLayer)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:
			
			 ExtraUmph = _rhs.GetOwner()->GetFlag(Dashing);

			PushBack(_lhs, _rhs, ExtraUmph);

			

			if (_lhs.m_pOwner->TakeDamage(ExtraUmph ? 10.0f : 2.0f))
				{
					g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				}
	if (!_rhs.m_pOwner->IsShielded())
	{


		if (_rhs.m_pOwner->TakeDamage(2))
		{
			g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
			{
				gamePad->Vibrate(0, 0.f, 0.2f);
			}
		}
	}
			break;

		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;

		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(25))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);

			break;

		case Env_Prominence:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->Ignite();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;

		case Env_Tornado:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->SpinOut();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;

		default:
			break;
		}
	}


	if (_lhs.m_pOwner->GetType() == Enemy_Dodger)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:

			 ExtraUmph = _rhs.GetOwner()->GetFlag(Dashing);

			PushBack(_lhs, _rhs, ExtraUmph);

			

			if (_lhs.m_pOwner->TakeDamage(ExtraUmph ? 10.0f : 2.0f))
				{
					g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				}
	if (!_rhs.m_pOwner->IsShielded())
	{
		if (_rhs.m_pOwner->TakeDamage(2))
		{
			g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
			{
				gamePad->Vibrate(0, 0.f, 0.2f);
			}
		}
	}
			break;

		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;
		case eOBJECT_TYPE::Bullet_Missile:
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchMissile();
				}
			}
			break;

		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(25))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega:
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);

			break;

		case Enemy_Dodger:
			PushBack(_lhs, _rhs);
			break;

		case Env_Tornado:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->SpinOut();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case Env_Prominence:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				_lhs.m_pOwner->Ignite();

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case Env_Prominence_Bullet:
			if (_lhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;

		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == Enemy_Bomber)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:
			

			

			if (_lhs.m_pOwner->TakeDamage(1000))
				{
					g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
					_lhs.m_pOwner->BomberFlag();
				}
			if (!_rhs.m_pOwner->IsShielded())
			{
				if (_rhs.m_pOwner->TakeDamage(50))
				{
					g_AudioSystem->PlaySound("RATS_PlayerHit", _rhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
					if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
					{
						gamePad->Vibrate(0, 0.2f, 0.4f);

					}
				}
			}
			
			break;

		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:

		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(gameData->GetDamage());
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;
		case eOBJECT_TYPE::Bullet_Spread:
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchSpread();
				}
			}
			break;

		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			//_lhs.m_pOwner->TakeDamage(50);
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(25))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;

		case eOBJECT_TYPE::Pylon_Lambda:
		case eOBJECT_TYPE::Pylon_Omega: 
		case eOBJECT_TYPE::Pylon_Sigma:

			PushBackStatic(_rhs, _lhs);
			break;



		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == Pylon_Test)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		//case EvilBullet_Simple:
		case EvilBullet_Disrupt:

			if (_lhs.m_pOwner->TakeDamage(5))
			{

			}
			_rhs.m_pOwner->SetDead(true);
			break;

		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == EvilBullet_Mine)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Spread:
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;
		case eOBJECT_TYPE::Bullet_Whip:
			//_lhs.m_pOwner->TakeDamage(5);
			_rhs.m_pOwner->SetDead(true);
			if (_lhs.m_pOwner->TakeDamage(gameData->GetDamage()))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
				if (_lhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchWhip();
				}
			}
			break;

		case Env_Prominence:
		case Env_Tornado:
			if (_lhs.m_pOwner->TakeDamage(50))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			}
			break;

		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == eOBJECT_TYPE::Env_Prominence_Bullet)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:
			if (!_rhs.m_pOwner->IsShielded())
			{


				if (_rhs.m_pOwner->TakeDamage(20))
				{
					g_AudioSystem->PlaySound("RATS_PlayerHit", _lhs.m_pOwner);
				}
			}
			break;
		case eOBJECT_TYPE::Enemy_Merc:			
		case eOBJECT_TYPE::Enemy_Bomber:
		case eOBJECT_TYPE::Enemy_Koi:
		case eOBJECT_TYPE::Enemy_Disruptor:
		case eOBJECT_TYPE::Enemy_MineLayer:
			if (_rhs.m_pOwner->TakeDamage(5))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

				if (_rhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case eOBJECT_TYPE::Orb_Blue:
		case eOBJECT_TYPE::Pickup_Health:
			_rhs.m_pOwner->SetDead();
			g_AudioSystem->PlaySound("RATS_EnemyDeath", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			break;

		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == eOBJECT_TYPE::Env_Tornado)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Orb_Blue:
		case eOBJECT_TYPE::Pickup_Health:
			_rhs.m_pOwner->SetDead();
			g_AudioSystem->PlaySound("RATS_EnemyDeath", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			break;

		default:
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == eOBJECT_TYPE::Env_Singularity)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:
			if (!_rhs.m_pOwner->IsShielded())
			{
				if (_rhs.m_pOwner->TakeDamage(20))
				{
					g_AudioSystem->PlaySound("RATS_PlayerHit", _lhs.m_pOwner);
					if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
					{
						gamePad->Vibrate(0, 0.2f, 0.5f);
					}
				}
			}
			break;
		case eOBJECT_TYPE::Enemy_Koi:
		case eOBJECT_TYPE::Enemy_Bomber:
		{
			_rhs.m_pOwner->SetDead();
			g_AudioSystem->PlaySound("RATS_EnemyDeath", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

			HandleAchHazards();
			
		}
			break;
		case eOBJECT_TYPE::EvilBullet_HeatSeek:
		{
			_rhs.m_pOwner->SetDead();
			g_AudioSystem->PlaySound("RATS_EnemyDeath", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

		}
			break;
		case eOBJECT_TYPE::Enemy_Merc:
		case eOBJECT_TYPE::Enemy_Dodger:
		case eOBJECT_TYPE::Enemy_Disruptor:
		case eOBJECT_TYPE::Enemy_MineLayer:
		case eOBJECT_TYPE::Enemy_Goliath:
		case eOBJECT_TYPE::Enemy_FenceLayer:
			if (_rhs.m_pOwner->TakeDamage(10))
			{
				g_AudioSystem->PlaySound("RATS_EnemyHit", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());

				if (_rhs.m_pOwner->GetComponent<HealthComponent>()->GetHealth() <= 0)
				{
					HandleAchHazards();
				}
			}
			break;
		case eOBJECT_TYPE::Orb_Blue:
		case eOBJECT_TYPE::Pickup_Health:
			_rhs.m_pOwner->SetDead();
			g_AudioSystem->PlaySound("RATS_EnemyDeath", _lhs.m_pOwner);// , *_lhs.m_pOwner->GetTransform().GetPosition());
			break;

		default:
			break;
		}
	}
	
}

void CollisionManager::HandleReactionAggre(ColliderComp& _lhs, ColliderComp& _rhs)
{
	DirectX::XMVECTOR vecBetwix, newVel;
	DirectX::XMFLOAT3 tmpVel;
	HealthComponent* pStats;


	if (_lhs.m_pOwner->GetType() == Player_Obj)
	{

		switch (_rhs.m_pOwner->GetType())
		{

		case eOBJECT_TYPE::Orb_Blue:
		case eOBJECT_TYPE::Orb_Red:
		case eOBJECT_TYPE::Orb_Yellow:
			pStats = reinterpret_cast<HealthComponent*>(_lhs.m_pOwner->GetComponent("HealthComponent"));

			if (pStats->GetEnergy() == pStats->GetMaxEnergy())
			{
				_rhs.GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 0));
				break;
			}
				

			
			//_rhs.GetOwner()->SetVelocity(DirectX::XMFLOAT3(100, 100, 100));
			_rhs.GetOwner()->GetComponent<AICompBase>()->SetStartVelocity(20);
			_rhs.GetOwner()->SetVelocityInWorldSpace(true);
			break;
			
		case eOBJECT_TYPE::Pickup_Health:

			pStats = reinterpret_cast<HealthComponent*>(_lhs.m_pOwner->GetComponent("HealthComponent"));

			if (pStats->GetHealth() == pStats->GetMaxHealth())
			{
				_rhs.GetOwner()->SetVelocity(DirectX::XMFLOAT3(0,0,0));
				break;
			}
			
			
				//_rhs.GetOwner()->SetVelocity(DirectX::XMFLOAT3(100, 100, 100));
				_rhs.GetOwner()->GetComponent<AICompBase>()->SetStartVelocity(20);
			_rhs.GetOwner()->SetVelocityInWorldSpace(true);

			break;

		case eOBJECT_TYPE::Pickup_Shield:
		
			_rhs.GetOwner()->GetComponent<AICompBase>()->SetStartVelocity(20);
			_rhs.GetOwner()->SetVelocityInWorldSpace(true);

			break;

		case eOBJECT_TYPE::EvilBullet_Mine:
// 			DirectX::XMVECTOR vecBetwix = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(_lhs.GetOwner()->GetTransform().GetPosition()), DirectX::XMLoadFloat3(_rhs.GetOwner()->GetTransform().GetPosition())), newVel;
// 
// 			newVel = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(vecBetwix), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(40, 40, 40)));
// 
 			//DirectX::XMFLOAT3 tmpVel(0,0,30);	//DirectX::XMStoreFloat3(&tmpVel, newVel);
			if (reinterpret_cast<AICompBase*>(_rhs.GetOwner()->GetComponent("AICompBase"))->GetProxTrigger() == false)
			{
				//_rhs.GetOwner()->SetVelocity(DirectX::XMFLOAT3(0,0,70));
				reinterpret_cast<AICompBase*>(_rhs.GetOwner()->GetComponent("AICompBase"))->SetProxTrigger(true);
				g_AudioSystem->PlaySound("RATS_MineAlert", _rhs.m_pOwner);//, *_lhs.m_pOwner->GetTransform().GetPosition());
				m_pObjManager->m_Molecules->BuildEmitter(_rhs.m_pOwner, 100, "MineTrigger");
			}
			
			//_rhs.GetOwner()->SetVelocityInWorldSpace(true);
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == EvilBullet_Mine)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Bullet_Simple:
		case eOBJECT_TYPE::Bullet_Missile:
		case eOBJECT_TYPE::Bullet_Spread:
		case eOBJECT_TYPE::Bullet_Whip:
			// 			DirectX::XMVECTOR vecBetwix = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(_lhs.GetOwner()->GetTransform().GetPosition()), DirectX::XMLoadFloat3(_rhs.GetOwner()->GetTransform().GetPosition())), newVel;
			// 
			// 			newVel = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(vecBetwix), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(40, 40, 40)));
			// 
			//DirectX::XMFLOAT3 tmpVel(0, 0, 30);	//DirectX::XMStoreFloat3(&tmpVel, newVel);
			if (reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase"))->GetProxTrigger() == false)
			{
				//_lhs.GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 70));
				reinterpret_cast<AICompBase*>(_lhs.GetOwner()->GetComponent("AICompBase"))->SetProxTrigger(true);
				g_AudioSystem->PlaySound("RATS_MineAlert", _lhs.m_pOwner);//, *_lhs.m_pOwner->GetTransform().GetPosition());
				m_pObjManager->m_Molecules->BuildEmitter(_lhs.m_pOwner, 100, "MineTrigger");
			}

			//_rhs.GetOwner()->SetVelocityInWorldSpace(true);
			break;
			break;
		}
	}

	if (_lhs.m_pOwner->GetType() == Enemy_Dodger)
	{
		DirectX::XMVECTOR EnemyX = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));
		DirectX::XMVECTOR toVec = EnemyX, inpulseHelper = EnemyX;
		DirectX::XMFLOAT3 outVec(0,0,0);
		switch (_rhs.m_pOwner->GetType())
		{
		case Bullet_Whip:
		case Bullet_Missile:
		case Bullet_Spread:
		case Bullet_Simple:

		inpulseHelper += DirectX::XMLoadFloat3(_lhs.GetOwner()->GetTransform().GetZAxis()) * -50;

		EnemyX = DirectX::XMLoadFloat3(_lhs.GetOwner()->GetTransform().GetXAxis());
		toVec = DirectX::XMLoadFloat3(&_lhs.GetStart()) - DirectX::XMLoadFloat3(&_rhs.GetStart());
		
		//outVec.z = -10;
		if (DirectX::XMVector3Dot(EnemyX, toVec).m128_f32[0] >= 0)
		{
			inpulseHelper += EnemyX * 50;
		}
		else
		{
			inpulseHelper += EnemyX * -50;
		}

		DirectX::XMStoreFloat3(&outVec, inpulseHelper);
		_lhs.GetOwner()->SetImpulse(outVec);



// 			dynaPos = DirectX::XMLoadFloat3(_dynamic.GetOwner()->GetTransform().GetPosition());
// 			staticPos = DirectX::XMLoadFloat3(_static.GetOwner()->GetTransform().GetPosition());
// 
// 			impulseHelper = dynaPos - staticPos;
// 			impulseHelper = DirectX::XMVector3NormalizeEst(impulseHelper);
// 			impulseHelper *= 35;
// 			impulseHelper += DirectX::XMLoadFloat3(&_dynamic.GetOwner()->GetImpulse());
// 			DirectX::XMStoreFloat3(&outVec, impulseHelper);
// 			_dynamic.GetOwner()->SetImpulse(outVec);

			break;

		default:
			break;
		}

	}

	if (_lhs.m_pOwner->GetType() == eOBJECT_TYPE::Env_Singularity)
	{
		switch (_rhs.m_pOwner->GetType())
		{
		case eOBJECT_TYPE::Player_Obj:

			if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
			{
				gamePad->Vibrate(0, 0.1f, 0.2f);
			}

		case eOBJECT_TYPE::Enemy_Bomber:
		case eOBJECT_TYPE::Enemy_Disruptor:
		case eOBJECT_TYPE::Enemy_Dodger:
		case eOBJECT_TYPE::Enemy_Koi:
		case eOBJECT_TYPE::Enemy_Goliath:
		case eOBJECT_TYPE::Enemy_Merc:
		case eOBJECT_TYPE::Enemy_MineLayer:
		case eOBJECT_TYPE::Enemy_FenceLayer:
		case eOBJECT_TYPE::EvilBullet_HeatSeek:
			
			vecBetwix = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(_lhs.GetOwner()->GetTransform().GetPosition()), DirectX::XMLoadFloat3(_rhs.GetOwner()->GetTransform().GetPosition()));

			newVel = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(vecBetwix), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(40, 40, 40)));

			DirectX::XMStoreFloat3(&tmpVel, newVel);
			//_rhs.GetOwner()->SetVelocity(tmpVel);
			_rhs.GetOwner()->SetImpulse(tmpVel);
			//_rhs.GetOwner()->SetVelocityInWorldSpace(true);


			break;
			break;
		}
	}

}

void CollisionManager::PushBack(ColliderComp& _lhs, ColliderComp& _rhs, bool _extra /*= false*/)
{

	DirectX::XMVECTOR impulseHelper = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));
	DirectX::XMVECTOR lhsPos = impulseHelper, rhsPos = impulseHelper;
	DirectX::XMFLOAT3 outVec;


	lhsPos = DirectX::XMLoadFloat3(_lhs.GetOwner()->GetTransform().GetPosition());
	rhsPos = DirectX::XMLoadFloat3(_rhs.GetOwner()->GetTransform().GetPosition());

	impulseHelper = lhsPos - rhsPos;
	impulseHelper = DirectX::XMVector3NormalizeEst(impulseHelper);
	impulseHelper *= _extra ? 500.0f : 10.0f;
	impulseHelper += DirectX::XMLoadFloat3(&_lhs.GetOwner()->GetImpulse());
	DirectX::XMStoreFloat3(&outVec, impulseHelper);
	_lhs.GetOwner()->SetImpulse(outVec);

	impulseHelper = rhsPos - lhsPos;
	impulseHelper = DirectX::XMVector3NormalizeEst(impulseHelper);
	impulseHelper *= 10;
	impulseHelper += DirectX::XMLoadFloat3(&_rhs.GetOwner()->GetImpulse());
	DirectX::XMStoreFloat3(&outVec, impulseHelper);
	_rhs.GetOwner()->SetImpulse(outVec);
}

void CollisionManager::PushBackStatic(ColliderComp& _static, ColliderComp& _dynamic)
{
	DirectX::XMVECTOR impulseHelper = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));
	DirectX::XMVECTOR dynaPos = impulseHelper, staticPos = impulseHelper;
	DirectX::XMFLOAT3 outVec;


	dynaPos = DirectX::XMLoadFloat3(_dynamic.GetOwner()->GetTransform().GetPosition());
	staticPos = DirectX::XMLoadFloat3(_static.GetOwner()->GetTransform().GetPosition());

	impulseHelper = dynaPos - staticPos;
	impulseHelper = DirectX::XMVector3NormalizeEst(impulseHelper);
	impulseHelper *= 35;
	impulseHelper += DirectX::XMLoadFloat3(&_dynamic.GetOwner()->GetImpulse());
	DirectX::XMStoreFloat3(&outVec, impulseHelper);
	_dynamic.GetOwner()->SetImpulse(outVec);
}

// ACHIEVEMENT CHECK
void CollisionManager::HandleAchHazards()
{
	int prevNumKills;

	if (g_SteamAchievements)
	{
		prevNumKills = g_SteamAchievements->m_pStats[EStats::STAT_ENV_HAZ_KILLS].m_iValue;

		if (prevNumKills == ENV_HAZ_KILLS_TOTAL)
			return;

		g_SteamAchievements->m_pStats[EStats::STAT_ENV_HAZ_KILLS].m_iValue++;
		SteamUserStats()->SetStat("STAT_ENV_HAZ_KILLS", g_SteamAchievements->m_pStats[EStats::STAT_ENV_HAZ_KILLS].m_iValue);

		//INDICATION PROGRESS CHECKS

		if (prevNumKills == 29)
			SteamUserStats()->IndicateAchievementProgress("ACH_USE_DEM_HAZARDS", 30, ENV_HAZ_KILLS_TOTAL);
		
		if (prevNumKills == 59)
			SteamUserStats()->IndicateAchievementProgress("ACH_USE_DEM_HAZARDS", 60, ENV_HAZ_KILLS_TOTAL);

		if (prevNumKills == 89)
			SteamUserStats()->IndicateAchievementProgress("ACH_USE_DEM_HAZARDS", 90, ENV_HAZ_KILLS_TOTAL);

		if (prevNumKills == 119)
			SteamUserStats()->IndicateAchievementProgress("ACH_USE_DEM_HAZARDS", 120, ENV_HAZ_KILLS_TOTAL);

	}

}

void CollisionManager::HandleAchSpread()
{
	int prevNumKills;

	if (g_SteamAchievements)
	{
		prevNumKills = g_SteamAchievements->m_pStats[EStats::STAT_BOMBER_SPREAD_KILLS].m_iValue;

		if (prevNumKills == BOMBER_KILLS_TOTAL)
			return;

		g_SteamAchievements->m_pStats[EStats::STAT_BOMBER_SPREAD_KILLS].m_iValue++;
		SteamUserStats()->SetStat("STAT_BOMBER_SPREAD_KILLS", g_SteamAchievements->m_pStats[EStats::STAT_BOMBER_SPREAD_KILLS].m_iValue);

		//INDICATION PROGRESS CHECKS
		if (prevNumKills == 49)
			SteamUserStats()->IndicateAchievementProgress("ACH_SPREADSHOT_BOMBER", 50, BOMBER_KILLS_TOTAL);

		if (prevNumKills == 99)
			SteamUserStats()->IndicateAchievementProgress("ACH_SPREADSHOT_BOMBER", 100, BOMBER_KILLS_TOTAL);

		if (prevNumKills == 149)
			SteamUserStats()->IndicateAchievementProgress("ACH_SPREADSHOT_BOMBER", 150, BOMBER_KILLS_TOTAL);

		if (prevNumKills == 199)
			SteamUserStats()->IndicateAchievementProgress("ACH_SPREADSHOT_BOMBER", 200, BOMBER_KILLS_TOTAL);
	}
}

void CollisionManager::HandleAchWhip()
{
	int prevNumKills;

	if (g_SteamAchievements)
	{
		prevNumKills = g_SteamAchievements->m_pStats[EStats::STAT_MINE_WHIP_KILLS].m_iValue;

		if (prevNumKills == MINE_KILLS_TOTAL)
			return;

		g_SteamAchievements->m_pStats[EStats::STAT_MINE_WHIP_KILLS].m_iValue++;
		SteamUserStats()->SetStat("STAT_MINE_WHIP_KILLS", g_SteamAchievements->m_pStats[EStats::STAT_MINE_WHIP_KILLS].m_iValue);

		//INDICATION PROGRESS CHECKS
		if (prevNumKills == 49)
			SteamUserStats()->IndicateAchievementProgress("ACH_WHIP_75", 50, MINE_KILLS_TOTAL);

		if (prevNumKills == 99)
			SteamUserStats()->IndicateAchievementProgress("ACH_WHIP_75", 100, MINE_KILLS_TOTAL);

		if (prevNumKills == 149)
			SteamUserStats()->IndicateAchievementProgress("ACH_WHIP_75", 150, MINE_KILLS_TOTAL);

	}
}

void CollisionManager::HandleAchMissile()
{
	int prevNumKills;

	if (g_SteamAchievements)
	{
		prevNumKills = g_SteamAchievements->m_pStats[EStats::STAT_DODGER_MISSILE_KILLS].m_iValue;

		if (prevNumKills == DODGER_KILLS_TOTAL)
			return;

		g_SteamAchievements->m_pStats[EStats::STAT_DODGER_MISSILE_KILLS].m_iValue++;
		SteamUserStats()->SetStat("STAT_DODGER_MISSILE_KILLS", g_SteamAchievements->m_pStats[EStats::STAT_DODGER_MISSILE_KILLS].m_iValue);

		//INDICATION PROGRESS CHECKS
		if (prevNumKills == 29)
			SteamUserStats()->IndicateAchievementProgress("ACH_HOMINGMISSILE_40", 30, DODGER_KILLS_TOTAL);

		if (prevNumKills == 59)
			SteamUserStats()->IndicateAchievementProgress("ACH_HOMINGMISSILE_40", 60, DODGER_KILLS_TOTAL);

		if (prevNumKills == 89)
			SteamUserStats()->IndicateAchievementProgress("ACH_HOMINGMISSILE_40", 90, DODGER_KILLS_TOTAL);

		if (prevNumKills == 119)
			SteamUserStats()->IndicateAchievementProgress("ACH_HOMINGMISSILE_40", 120, DODGER_KILLS_TOTAL);

	}
}

