#include "stdafx.h"
#include "BehavGoliath.h"
#include "../../Wwise files/Transform.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../Object Manager/ObjectManager.h"
#include "../Components/HealthComponent.h"
#include "../Audio/FFmod.h"
#include "../Molecules/MoleculeManager.h"

extern	FFmod*		g_AudioSystem;

BehavGoliath::BehavGoliath()
{
	m_logicType = bGrow;
	Reset();
}


BehavGoliath::~BehavGoliath()
{
}

void BehavGoliath::DoLogic(float _dt)
{

	if (GetOwner()->GetAssimilate() && !bDoneGrowing)
	{
		GetOwner()->SetAssimilate(false);
		LevelUp();
	}


	if (GetOwner()->GetTarget() == nullptr)
		ChooseTarget();

	if (GetOwner()->GetTarget()->GetFlag(MostlyDead))
		ChooseTarget();

	
	TrackTarget(_dt);

}

void BehavGoliath::TrackTarget(float _dt)
{
	using namespace DirectX;
	GameObject* pOwner = GetOwner()->GetOwner();

	// Turn towards

	Transform lhs_transform = pOwner->GetTransform();
	Transform rhs_transform;

	if (GetOwner()->GetTarget() == nullptr)
		return;

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
}

void BehavGoliath::ChooseTarget()
{
	GameObject* newTarget = nullptr;
	XMVECTOR to_target;
	float Mag = FLT_MAX;
	float tmpMag = 0;
	HealthComponent* pylonStats = nullptr;
	
	if (m_fStrength < 10)
	{
		// LOOK FOR ORBS
		if (GetOwner()->GetOwner()->m_ObjManager->PassBuckets()[Orb_Blue].size() > 0)
		{
			for (m_Iter = GetOwner()->GetOwner()->m_ObjManager->PassBuckets()[Orb_Blue].begin();
				m_Iter != GetOwner()->GetOwner()->m_ObjManager->PassBuckets()[Orb_Blue].end(); m_Iter++)
			{
				if (GetOwner()->GetTarget() == (*m_Iter))
					continue;

				 to_target = XMVectorSubtract(XMLoadFloat3(GetOwner()->GetOwner()->GetTransform().GetPosition()), XMLoadFloat3((*m_Iter)->GetTransform().GetPosition()));
				 tmpMag = DirectX::XMVector3Dot(to_target, to_target).m128_f32[0];
				 if (tmpMag < Mag)
				 {
					 Mag = tmpMag;
					 newTarget = (*m_Iter);
				 }
			}

			GetOwner()->m_pTarget = newTarget;
		}
		// LOOK FOR PYLON
		else
		{
			for (m_Iter = GetOwner()->GetOwner()->m_ObjManager->PassBuckets()[Pylon_Test].begin();
				m_Iter != GetOwner()->GetOwner()->m_ObjManager->PassBuckets()[Pylon_Test].end(); m_Iter++)
			{
				pylonStats = reinterpret_cast<HealthComponent*>((*m_Iter)->GetComponent("HealthComponent"));

				if (pylonStats->GetHealth() <= pylonStats->GetMaxHealth() * 0.1f)
					continue;

				if ((*m_Iter)->GetSecondaryType() == Pylon_Omega && bHitOmega)
					continue;

				if ((*m_Iter)->GetSecondaryType() == Pylon_Sigma && bHitSigma)
					continue;

				if ((*m_Iter)->GetSecondaryType() == Pylon_Lambda && bHitLambda)
					continue;

				to_target = XMVectorSubtract(XMLoadFloat3(GetOwner()->GetOwner()->GetTransform().GetPosition()), XMLoadFloat3((*m_Iter)->GetTransform().GetPosition()));
				tmpMag = DirectX::XMVector3Dot(to_target, to_target).m128_f32[0];
				if (tmpMag < Mag)
				{
					Mag = tmpMag;
					newTarget = (*m_Iter);
				}
			}

			if (newTarget == nullptr)
				goto worstcasescenario;

			GetOwner()->m_pTarget = newTarget;
			switch (newTarget->GetSecondaryType())
			{
			case Pylon_Lambda:
				bHitLambda = true;
				break;

			case Pylon_Sigma:
				bHitSigma = true;
				break;

			case Pylon_Omega:
				bHitOmega = true;
				break;
			}

		}

	}
	// READY TO CHARGE
	else
	{
		worstcasescenario:

		GetOwner()->m_pTarget = GetOwner()->GetOwner()->m_ObjManager->GetPlayer();
		GetOwner()->m_bNeedShootComp = true;
		bDoneGrowing = true;
	}
}

void BehavGoliath::LevelUp()
{
	if (bDoneGrowing)
		return;

	if (GetOwner()->GetTarget() == nullptr)
	{
		m_fStrength += 5;
		m_fScale += 0.3f;
		GetOwner()->SetStartVelocity(GetOwner()->GetStartVelocity() * 1.3f);
		reinterpret_cast<HealthComponent*>(GetOwner()->GetOwner()->GetComponent("HealthComponent"))->SetMaxHealth(reinterpret_cast<HealthComponent*>(GetOwner()->GetOwner()->GetComponent("HealthComponent"))->GetMaxHealth() * 1.5f);

	}
		
	else
	{
		m_fStrength += 1;
		GetOwner()->SetStartVelocity(GetOwner()->GetStartVelocity() * 1.1f);
		m_fScale += 0.1f;
		reinterpret_cast<HealthComponent*>(GetOwner()->GetOwner()->GetComponent("HealthComponent"))->SetMaxHealth(reinterpret_cast<HealthComponent*>(GetOwner()->GetOwner()->GetComponent("HealthComponent"))->GetMaxHealth() * 1.1f);

	}

	GetOwner()->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, GetOwner()->GetStartVelocity()));

	ChooseTarget();
	if (m_fStrength < 10)
		GetOwner()->GetOwner()->m_ObjManager->m_Molecules->BuildEmitter(GetOwner()->GetOwner(), 100, "GoliathLevelUp");
	else
		GetOwner()->GetOwner()->m_ObjManager->m_Molecules->BuildEmitter(GetOwner()->GetOwner(), 100, "GoliathLevelDone");

	//g_AudioSystem->PlaySound("", GetOwner()->GetOwner());

	GetOwner()->GetOwner()->GetTransform().SetScaleAll(m_fScale);

}

void BehavGoliath::Reset()
{
	m_fStrength = m_fScale = 1;
	bHitLambda = bHitSigma = bHitOmega = false;
	//m_bNeedShootComponent = false;
	bDoneGrowing = false;
}
