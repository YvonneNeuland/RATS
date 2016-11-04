#include "stdafx.h"
#include "BehavTracking.h"
#include "../../Wwise files/Transform.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../Components/HealthComponent.h"
#include "../Audio/FFmod.h"
#include "../Object Manager/ObjectManager.h"

extern	FFmod*		g_AudioSystem;

BehavTracking::BehavTracking()
{
	m_logicType = bFollow;
	m_fMissilePauseTimer = 0;
	m_fRotSpeed = 0;
	m_targetTimer = 0;
}


BehavTracking::~BehavTracking()
{
}

void BehavTracking::DoLogic(float _dt)
{
	using namespace DirectX;
	GameObject* pOwner = GetOwner()->GetOwner();

	
		

	if (GetOwner()->GetAIType() == AICompBase::AI_PLAYER_MISSILE)
	{
		m_targetTimer += _dt;

		//Find a new target
		if (m_targetTimer >= 0.2f)
		{
			m_targetTimer = 0;
			GameObject* missile = GetOwner()->GetOwner();

			GameObject* closestEnemy = nullptr;
			float closestDistance = FLT_MAX;

			XMVECTOR missilePos = XMLoadFloat3(missile->GetTransform().GetPosition());

			XMVECTOR enemyPos;
			XMVECTOR distanceVec;
			XMFLOAT3 distanceResult;


			vector<list<GameObject*>>& buckets = missile->m_ObjManager->PassBuckets();

			int currbucket = Enemy_Merc;
			list<GameObject*>::iterator currEnemy;

			while (currbucket != Player_Obj)
			{
				currEnemy = buckets[currbucket].begin();

				while (currEnemy != buckets[currbucket].end())
				{
					if ((*currEnemy)->GetFlag(Spwaning))
					{
						++currEnemy;
						continue;
					}

					enemyPos = XMLoadFloat3((*currEnemy)->GetTransform().GetPosition());

					distanceVec = XMVector3LengthSq(enemyPos - missilePos);

					XMStoreFloat3(&distanceResult, distanceVec);

					if (distanceResult.x < closestDistance && distanceResult.x < 1000)
					{
						closestDistance = distanceResult.x;						
						closestEnemy = *currEnemy;

					}

					++currEnemy;
				}

				++currbucket;
			}

			for (auto mine : buckets[eOBJECT_TYPE::EvilBullet_Mine])
			{
				enemyPos = XMLoadFloat3(mine->GetTransform().GetPosition());

				distanceVec = XMVector3LengthSq(enemyPos - missilePos);

				XMStoreFloat3(&distanceResult, distanceVec);

				if (distanceResult.x < closestDistance)
				{
					closestDistance = distanceResult.x;
					closestEnemy = mine;
				}
			}

			GetOwner()->SetTarget(closestEnemy);

		}
	}

	// Turn towards

	Transform lhs_transform = pOwner->GetTransform();
	Transform rhs_transform;

	if (GetOwner()->GetTarget() != nullptr)
	{
		rhs_transform = GetOwner()->GetTarget()->GetTransform();
		XMMATRIX mat = XMLoadFloat4x4(&lhs_transform.GetLocalMatrix());

		// Get WORLD-SPACE positions
		XMFLOAT3 lhs_pos = *lhs_transform.GetPosition();
		XMFLOAT3 rhs_pos = *rhs_transform.GetPosition();
		XMVECTOR to_target = XMVectorSubtract(XMLoadFloat3(&rhs_pos), XMLoadFloat3(&lhs_pos));

		//float up_down = DotProduct(mat.YAxis, to_target);
		float left_right = XMVector3Dot(XMLoadFloat3(lhs_transform.GetXAxis()), to_target).m128_f32[0];

		//if (up_down > DEAD_ZONE || up_down < -DEAD_ZONE)
		//	mat.rotateLocalX(up_down * turn_speed * Game::GetDeltaTime());
		if (left_right > FLT_EPSILON || left_right < -FLT_EPSILON)
			pOwner->GetTransform().RotateBy(1, left_right * GetOwner()->GetTurnSpeed() * _dt);//mat.rotateLocalY(left_right * -turn_speed * Game::GetDeltaTime());


		// Slow down once we get close

		float SqMag = DirectX::XMVector3Dot(to_target, to_target).m128_f32[0];

		if (GetOwner()->GetAIType() == AICompBase::AI_PICKUP)
		{
			XMFLOAT3 pickupVel;
			XMStoreFloat3(&pickupVel, to_target * GetOwner()->GetStartVelocity());

			pOwner->SetVelocity(pickupVel);

			return;
		}


		if (GetOwner()->GetAIType() != AICompBase::AI_BOMBER && GetOwner()->GetAIType() != AICompBase::AI_MINE
			&& GetOwner()->GetAIType() != AICompBase::AI_PLAYER_MISSILE)
		{
			if (this->GetOwner()->GetAIType() != AICompBase::AI_MISSILE)
			{
				if (SqMag <= 90)
				{
					DirectX::XMFLOAT3 newVel;
					XMStoreFloat3(&newVel, XMVectorMultiply(XMLoadFloat3(&pOwner->GetVelocity()), XMLoadFloat3(&XMFLOAT3(0, 0, 0.95f))));

					pOwner->SetVelocity(newVel);
				}

				else if (SqMag > 450)
				{
					pOwner->SetVelocity(XMFLOAT3(0, 0, GetOwner()->GetStartVelocity()));
				}
			}
			else
			{
				if (m_fMissilePauseTimer > 0)
				{
					m_fMissilePauseTimer -= _dt;
					m_fRotSpeed += _dt * 10;

					if (m_fMissilePauseTimer <= 0)
					{
						//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_RATS_MISSILERUSH, &pOwner->GetTransform());
						g_AudioSystem->PlaySound("RATS_MissileRush", pOwner);
						pOwner->SetVelocity(XMFLOAT3(0, 0, GetOwner()->GetStartVelocity() * 2.5f));
						GetOwner()->SetTarget(0);
						reinterpret_cast<HealthComponent*>(pOwner->GetComponent("HealthComponent"))->SetMaxHealth(1.0f);
					}
				}

				else if (SqMag <= 500)
				{
					m_fMissilePauseTimer = 0.7f;
					pOwner->SetVelocity(XMFLOAT3(0, 0, 0));
					//g_AudioSystem->PostEvent(AK::EVENTS::STOP_RATS_MISSILELOOP, &pOwner->GetTransform());
					pOwner->GetChannel()->stop();
					g_AudioSystem->PlaySound("RATS_MissileCharge", pOwner);

				}
			}
		}

		if (m_fRotSpeed > 0)
		{
			pOwner->GetTransform().RotateBy(2, m_fRotSpeed);
			pOwner->GetTransform().SetScaleAll(1 + (0.7f - m_fMissilePauseTimer));
		}

	}
}
