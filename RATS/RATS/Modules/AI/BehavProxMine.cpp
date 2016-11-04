#include "stdafx.h"
#include "BehavProxMine.h"
#include "../../Wwise files/Transform.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../Components/HealthComponent.h"
#include "../Renderer/RenderComp.h"
#include "../Audio/FFmod.h"
#include "../Renderer/D3DGraphics.h"
#include "../Object Manager/ObjectManager.h"
#include "../Object Manager/ObjectFactory.h"

extern	FFmod*		g_AudioSystem;
extern D3DGraphics* globalGraphicsPointer;

BehavProxMine::BehavProxMine()
{
	m_logicType = bProxMine;
	m_fPauseTimer = -1;
}


BehavProxMine::~BehavProxMine()
{
}

void BehavProxMine::DoLogic(float _dt)
{
	if (m_fPauseTimer == -1)
	{
		if (m_Owner->GetProxTrigger())
		{
			m_fPauseTimer = 1;
			//reinterpret_cast<RenderComp*>(m_Owner->GetOwner()->GetComponent("RenderComp"))->SetGlowID("Mine Glow2");
// 			auto _iter = GetOwner()->GetOwner()->GetComponents().begin();
// 			while (_iter != GetOwner()->GetOwner()->GetComponents().end())
// 			{
// 				if ((*_iter)->GetType() == eCOMPONENT_TYPE::Glow)
// 					break;
// 				_iter++;
// 			}
// 			if (_iter != GetOwner()->GetOwner()->GetComponents().end())
// 			{
			//	GetOwner()->GetOwner()->m_ObjManager->m_pFactory->RemoveGlowEffect((*_iter), globalGraphicsPointer->HashString("Mine Glow"));
				GetOwner()->GetOwner()->m_ObjManager->m_pFactory->AddGlowEffect(GetOwner()->GetOwner(), "Mine Glow2");
			//}

			//if (AddGlowEffect(gameObject, render_ID + " Glow"))
			//{

			//}
			//else return false;
		}
	}

	else if (m_fPauseTimer > 0)
	{
		m_fPauseTimer -= _dt;
		if (m_fPauseTimer <= 0)
		{
			m_fPauseTimer = 0;
			m_Owner->SetStartVelocity(65);
			m_Owner->GetOwner()->SetVelocity(DirectX::XMFLOAT3(0, 0, 70));
		}

		else
		{
			m_Owner->GetOwner()->GetTransform().SetScaleAll(0.5f + (1.0f - m_fPauseTimer * 0.5f));
		}
	}

	else
	{
		TrackTarget(_dt);
	}

	
}

void BehavProxMine::TrackTarget(float _dt)
{
	using namespace DirectX;
	GameObject* pOwner = GetOwner()->GetOwner();

	// Turn towards

	Transform lhs_transform = pOwner->GetTransform();
	Transform rhs_transform;


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

	m_Owner->GetOwner()->GetTransform().SetScaleAll(1.5f);
}
