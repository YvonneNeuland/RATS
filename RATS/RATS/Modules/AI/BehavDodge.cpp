#include "stdafx.h"
#include "BehavDodge.h"
#include "../../Wwise files/Transform.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../Components/HealthComponent.h"
#include "../Audio/FFmod.h"

extern	FFmod*		g_AudioSystem;

BehavDodge::BehavDodge()
{
	m_logicType = bDodge;
}


BehavDodge::~BehavDodge()
{
}

void BehavDodge::DoLogic(float _dt)
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

	if (SqMag < 1500)
	{
		// back off
		if (m_Owner->GetOwner()->GetVelocity().z > 0)
		{
			m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, m_Owner->GetOwner()->GetVelocity().z - 3));
		}
		else if (m_Owner->GetOwner()->GetVelocity().z > -1 * m_Owner->GetStartVelocity())
		{
			m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0,  m_Owner->GetOwner()->GetVelocity().z - 2));

		}
	}
	else if (SqMag > 2000)
	{
		//pursue

		if (m_Owner->GetOwner()->GetVelocity().z <= 0)
		{
			m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, m_Owner->GetOwner()->GetVelocity().z  + 3));
		}
		else
		{
			m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, min(m_Owner->GetStartVelocity(), m_Owner->GetOwner()->GetVelocity().z * 1.2f + 1)));
		}
		

	}
	else if (m_Owner->GetOwner()->GetVelocity().z > FLT_EPSILON || m_Owner->GetOwner()->GetVelocity().z < -FLT_EPSILON)
	{
		// slow down
		m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, m_Owner->GetOwner()->GetVelocity().z * 0.8f));
	}
	else
	{
		m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0,0));
	}
	
}

/*

if close, back away
if far away, move closer.
if just right, stay there.  

if bullet incoming, move to the side



*/