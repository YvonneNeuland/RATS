#include "stdafx.h"
#include "BehavFlocking.h"
#include "../../Wwise files/Transform.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../Components/HealthComponent.h"
#include "../Object Manager/ObjectManager.h"
#include "../Audio/FFmod.h"
#include "../Input/ThreeSixty.h"
#include "../Upgrade System/GameData.h"

extern FFmod* g_AudioSystem;
extern ThreeSixty* gamePad;
extern GameData* gameData;

BehavFlocking::BehavFlocking()
{
	m_pStats = 0;
	m_logicType = bFlock;
	m_fSafeRadius = 4;
	m_bLatch = false;
	m_pLatchVFX[0] = nullptr;
	m_pLatchVFX[1] = nullptr;
	m_fDrainTImer = 0;
}


BehavFlocking::~BehavFlocking()
{
}

void BehavFlocking::DoLogic(float _dt)
{
	if (m_Owner->GetOwner()->IsSpinningOut())
	{
		if (m_bLatch)
		{
			Detach();
		}

		return;
	}


	HealthComponent* playerStats = 0;
	if (m_Owner->GetTarget())
	{
		playerStats = reinterpret_cast<HealthComponent*>(m_Owner->GetTarget()->GetComponent("HealthComponent"));
	}


	bSlowDownBuddy = false;
	float currentAngle = m_Owner->GetOwner()->GetTransform().GetRotation(1);

	DirectX::XMVECTOR accel = DirectX::XMVectorAdd(CalcAlignment(), CalcCohesion());// , dotRes;
	accel = DirectX::XMVectorAdd(accel, CalcSeparation());
	accel = DirectX::XMVectorAdd(accel, CalcTracking());
	//accel = DirectX::XMVector3Normalize(accel);

	DirectX::XMVECTOR locX = DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetXAxis());
	DirectX::XMVECTOR locZ = DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetZAxis());


	DirectX::XMVECTOR onPlane;
	onPlane = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(locX, DirectX::XMVector3Dot(locX, accel)), DirectX::XMVectorMultiply(locZ, DirectX::XMVector3Dot(locZ, accel)));




	float signage = DirectX::XMVector3Dot(onPlane, locX).m128_f32[0];

	//if (signage != 0)
	//	signage /= abs(signage);
	//
	float ang = DirectX::XMVector3AngleBetweenVectors(locX, onPlane).m128_f32[0];
	//
	m_Owner->GetOwner()->GetTransform().RotateBy(1, (signage /*- currentAngle*/)* GetOwner()->GetTurnSpeed() * _dt);


	if (bSlowDownBuddy)
	{
		m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, m_Owner->GetOwner()->GetVelocity().z * 0.9f));
	}
	else if (m_Owner->GetOwner()->GetVelocity().z < m_Owner->GetStartVelocity())
	{
		m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, m_Owner->GetOwner()->GetVelocity().z * 1.2f));
	}

	// DISRUPTOR DRAINING
	else if (m_bLatch)
	{
		DirectX::XMVECTOR vecBetwix = DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetPosition()) - DirectX::XMLoadFloat3(m_Owner->GetTarget()->GetTransform().GetPosition());
		vecBetwix = DirectX::XMVector3Normalize(vecBetwix);
		vecBetwix *= 20;
		vecBetwix += DirectX::XMLoadFloat3(m_Owner->GetTarget()->GetTransform().GetPosition());
		DirectX::XMFLOAT3 newPOs;
		DirectX::XMStoreFloat3(&newPOs, vecBetwix );
		m_Owner->GetOwner()->GetTransform().SetPosition(newPOs);

		if (m_pLatchVFX[0] != nullptr)
		{
			Position* positions = m_pLatchVFX[0]->GetPositions();
			positions[0] = *m_Owner->GetOwner()->GetTransform().GetPosition();
			/*positions[m_pLatchVFX[0]->GetNumSegments()]*/
			reinterpret_cast<ElectricityEffect*>(m_pLatchVFX[0])->GrabTarget() = *m_Owner->GetTarget()->GetTransform().GetPosition();

			positions = m_pLatchVFX[1]->GetPositions();
			positions[0] = *m_Owner->GetTarget()->GetTransform().GetPosition();
			/*positions[m_pLatchVFX[1]->GetNumSegments()]*/
			reinterpret_cast<ElectricityEffect*>(m_pLatchVFX[1])->GrabTarget() = *m_Owner->GetOwner()->GetTransform().GetPosition();

		
		}

		if (GetOwner()->GetTarget()->IsShielded())
		{
			Detach();
		}

		m_fDrainTImer -= _dt;
		if (m_fDrainTImer <= 0)
		{
			m_fDrainTImer = DRAINDELAY;
			if (playerStats->GetEnergy() > 0)
			{
				playerStats->SetEnergy(playerStats->GetEnergy() - 1);
				g_AudioSystem->PlaySound("RATS_DisruptorLeech", m_Owner->GetOwner());
			}
			else
			{
				m_Owner->GetTarget()->TakeDamage(1);
				g_AudioSystem->PlaySound("RATS_DisruptorLeech", m_Owner->GetOwner());
			}
				
			if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
			{
				gamePad->Vibrate(0, 0.1f, 0.4f);
			}
		}

		if (m_Owner->GetTarget()->GetFlag(Dashing))
		{
			Detach();
		}
		
	}

}

DirectX::XMVECTOR BehavFlocking::CalcAlignment()
{
	if (m_pStats->m_fAlignmentWait == 0)
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));

	// Rotation
	DirectX::XMVECTOR newFwd = DirectX::XMLoadFloat3(&m_pStats->m_vAvgForward);
	//newFwd = DirectX::XMVector3Normalize(newFwd);
	return DirectX::XMVectorMultiply(newFwd, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_pStats->m_fAlignmentWait, m_pStats->m_fAlignmentWait, m_pStats->m_fAlignmentWait)));


}

DirectX::XMVECTOR BehavFlocking::CalcCohesion()
{
	if (m_pStats->m_fCohesionWait == 0)
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));


	DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_pStats->m_vAvgPos), DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetPosition()));
	float sqrdDist = DirectX::XMVector3Dot(tmp, tmp).m128_f32[0];

	// 	float left_right = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetXAxis()), tmp).m128_f32[0];
	// 	left_right *= 1 / (2 * DirectX::XM_PI);
	// 
	float scalar = sqrdDist / (m_pStats->m_fFlockRadius * m_pStats->m_fFlockRadius);
	if (sqrdDist < m_pStats->m_fFlockRadius * m_pStats->m_fFlockRadius)
		tmp = DirectX::XMVectorMultiply(tmp, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(scalar, scalar, scalar)));
	// 		left_right *= sqrdDist / (m_pStats->m_fFlockRadius * m_pStats->m_fFlockRadius);
	tmp = DirectX::XMVector3Normalize(tmp);
	return DirectX::XMVectorMultiply(tmp, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_pStats->m_fCohesionWait, m_pStats->m_fCohesionWait, m_pStats->m_fCohesionWait)));


}

DirectX::XMVECTOR BehavFlocking::CalcSeparation()
{
	if (m_pStats->m_fSeparationWait == 0)
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));



	DirectX::XMVECTOR rotSum = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));

	m_Iter = m_pStats->m_vFlock.begin();

	while (m_Iter != m_pStats->m_vFlock.end())
	{
		if ((*m_Iter) == m_Owner)
		{
			m_Iter++;
			continue;
		}
		DirectX::XMVECTOR btwix = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetPosition()), DirectX::XMLoadFloat3((*m_Iter)->GetOwner()->GetTransform().GetPosition()));

		float sqrDist = DirectX::XMVector3Dot(btwix, btwix).m128_f32[0];
		float safeDist = reinterpret_cast<BehavFlocking*>(*(*m_Iter)->m_vBehaviors.begin())->m_fSafeRadius + m_fSafeRadius;

		if (sqrDist < safeDist * safeDist)
		{

			btwix = DirectX::XMVector3Normalize(btwix);
			float fScal = ((safeDist * safeDist) - sqrDist) / (safeDist * safeDist);
			btwix = DirectX::XMVectorMultiply(btwix, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(fScal, fScal, fScal)));
			rotSum = DirectX::XMVectorAdd(rotSum, btwix);

			//bSlowDownBuddy = true;
		}


		m_Iter++;
	}

	if (DirectX::XMVector3Length(rotSum).m128_f32[0] > 1)
		rotSum = DirectX::XMVector3Normalize(rotSum);

	return DirectX::XMVectorMultiply(rotSum, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_pStats->m_fSeparationWait, m_pStats->m_fSeparationWait, m_pStats->m_fSeparationWait)));

}

DirectX::XMVECTOR BehavFlocking::CalcTracking()
{

	if (GetOwner()->GetTarget() == nullptr)
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0));


	GameObject* pOwner = GetOwner()->GetOwner();

	// Turn towards

	Transform lhs_transform = pOwner->GetTransform();
	Transform rhs_transform = GetOwner()->GetTarget()->GetTransform();
	//XMMATRIX mat = XMLoadFloat4x4(&lhs_transform.GetLocalMatrix());

	// Get WORLD-SPACE positions
	DirectX::XMFLOAT3 lhs_pos = *lhs_transform.GetPosition();
	DirectX::XMFLOAT3 rhs_pos = *rhs_transform.GetPosition();
	DirectX::XMVECTOR to_target = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&rhs_pos), DirectX::XMLoadFloat3(&lhs_pos));



	float SqMag = DirectX::XMVector3Dot(to_target, to_target).m128_f32[0];
	


	if (SqMag <= 500)
	{

		if (m_Owner->GetAIType() == AICompBase::AI_KOI_D )
		{

			if (m_Owner->GetTarget()->GetFlag(Dashing) || m_Owner->GetTarget()->IsShielded())
				return to_target;

			m_Owner->m_bRotate = true;
			m_bLatch = true;

			if (m_pLatchVFX[0] == nullptr)
			{

				Color col = { 1, 1, 0, 1 };

				m_pLatchVFX[0] = reinterpret_cast<ElectricityEffect*>(m_Owner->GetOwner()->m_ObjManager->CreateObject(eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3(0, 0, 0)));

				Position* positions = m_pLatchVFX[0]->GetPositions();
				positions[0] = *m_Owner->GetOwner()->GetTransform().GetPosition();
				/*positions[m_pLatchVFX[0]->GetNumSegments()]*/
				reinterpret_cast<ElectricityEffect*>(m_pLatchVFX[0])->GrabTarget() = *m_Owner->GetTarget()->GetTransform().GetPosition();
				m_pLatchVFX[0]->GrabColor() = col;


				m_pLatchVFX[1] = reinterpret_cast<ElectricityEffect*>(m_Owner->GetOwner()->m_ObjManager->CreateObject(eOBJECT_TYPE::VFX_Lightning, DirectX::XMFLOAT3(0, 0, 0)));

				positions = m_pLatchVFX[1]->GetPositions();
				positions[0] = *m_Owner->GetTarget()->GetTransform().GetPosition();
				/*positions[m_pLatchVFX[1]->GetNumSegments()]*/
				reinterpret_cast<ElectricityEffect*>(m_pLatchVFX[1])->GrabTarget() = *m_Owner->GetOwner()->GetTransform().GetPosition();
				m_pLatchVFX[1]->GrabColor() = col;

			}
			
			//m_pLatchVFX->SetOrigLastPos(positions[m_pLatchVFX->GetNumSegments()]);
		}
			

		if (m_Owner->GetAIType() != AICompBase::AI_KOI_D && m_Owner->GetAIType() != AICompBase::AI_BOMBER)
		{
			bSlowDownBuddy = true;
		}
		else
		{
			if (!m_Owner->m_bRotate)
				return to_target;

			DirectX::XMVECTOR crossRes = DirectX::XMVector3Cross(to_target, DirectX::XMLoadFloat3(rhs_transform.GetYAxis()));
			return crossRes;

		}

		
	}
	else if (m_Owner->m_bRotate)
		m_Owner->m_bRotate = false;
	

	return to_target;
}

void BehavFlocking::Detach()
{
	m_fDrainTImer = 0;
	m_pLatchVFX[0]->SetDead(true);
	reinterpret_cast<ElectricityEffect*>(m_pLatchVFX[0])->ResetTimer();
	reinterpret_cast<ElectricityEffect*>(m_pLatchVFX[1])->ResetTimer();
	m_pLatchVFX[1]->SetDead(true);
	m_pLatchVFX[0] = nullptr;
	m_pLatchVFX[1] = nullptr;


	m_bLatch = false;
}

void Flock::Update(float _dt)
{
	CalcAverages();


	m_Iter = m_vFlock.begin();
	while (m_Iter != m_vFlock.end())
	{
		(*m_Iter)->Update(_dt);
		m_Iter++;
	}
}

void Flock::CalcAverages()
{
	m_vAvgForward = m_vAvgPos = DirectX::XMFLOAT3(0, 0, 0);

	DirectX::XMVECTOR tmpVec = XMLoadFloat3(&m_vAvgPos), tmpFwd = tmpVec;
	m_Iter = m_vFlock.begin();

	while (m_Iter != m_vFlock.end())
	{


		tmpVec = DirectX::XMVectorAdd(tmpVec, DirectX::XMLoadFloat3((*m_Iter)->GetOwner()->GetTransform().GetPosition()));
		tmpFwd = DirectX::XMVectorAdd(tmpFwd, DirectX::XMLoadFloat3((*m_Iter)->GetOwner()->GetTransform().GetZAxis()));

		m_Iter++;
	}
	float scalar = (float)m_vFlock.size();
	scalar = 1 / scalar;
	tmpFwd = DirectX::XMVectorMultiply(tmpFwd, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(scalar, scalar, scalar)));
	tmpVec = DirectX::XMVectorMultiply(tmpVec, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(scalar, scalar, scalar)));

	DirectX::XMStoreFloat3(&m_vAvgPos, tmpVec);
	DirectX::XMStoreFloat3(&m_vAvgForward, tmpFwd);

}
