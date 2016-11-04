#include "stdafx.h"
#include "BehavSpawning.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/Transform.h"
#include "../Collision/CollisionManager.h"
#include "AICompBase.h"
#include "../Upgrade System/GameData.h"
#include "../VFX/RibbonEffect/RibbonEffect.h"
#include "../Object Manager/ObjectManager.h"
#include "../Object Manager/ObjectFactory.h"
#include "../Components/Movement/GroundFollowComponent.h"

extern GameData* gameData;

BehavSpawning::BehavSpawning()
{
	Reset();
}

BehavSpawning::~BehavSpawning()
{

}

void BehavSpawning::SetLandingPoint(const DirectX::XMFLOAT3 v)
{
	m_vLandPoint = v;

	Transform& myTrans = GetOwner()->GetOwner()->GetTransform();

	XMFLOAT3 newX, newY, newZ;

	XMStoreFloat3(&newZ, XMVector3Normalize(XMLoadFloat3(&v) - XMLoadFloat3(myTrans.GetPosition())));
	XMStoreFloat3(&newX, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(myTrans.GetYAxis()), XMLoadFloat3(&newZ))));
	XMStoreFloat3(&newY, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newZ), XMLoadFloat3(&newX))));

	myTrans.SetXAxis(newX);
	myTrans.SetYAxis(newY);
	myTrans.SetZAxis(newZ);

}

void BehavSpawning::DoLogic(float _dt)
{

	m_fTotalTimer += _dt;
	if (m_fTotalTimer >= 17.0f)
	{
		//m_pPointFX->loop = false;
		m_pLandIcon->SetDead();
		m_pTrailFX->loop = false;
		GetOwner()->GetOwner()->SetDead(true);
	}

	if (m_bFirst)
	{
		//LookAtGround();
		m_bFirst = false;
		m_Owner->GetOwner()->SetVelocityInWorldSpace(true);
		CreateLandIcon();

	}

	DirectX::XMVECTOR to_world = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_vLandPoint), DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetPosition()));
	float sqrMag = DirectX::XMVector3Dot(to_world, to_world).m128_f32[0];

	

	if (m_bClose)
	{
		float up_down = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetZAxis()), to_world).m128_f32[0];

		if (up_down > FLT_EPSILON || up_down < -FLT_EPSILON)
		{
			m_Owner->GetOwner()->GetTransform().RotateBy(0, -1 * up_down * _dt);
		}

		if (sqrMag < 25)
		{
			m_bLanded = true;
		
		}

		RibbonEffect* trail = m_Owner->GetTrail();
		if (trail)
			trail->SetLength(TrailLength::Medium);
	}
	else
	{
		if (sqrMag < 150)
		{
			m_bClose = true;
			m_Owner->GetOwner()->SetVelocityInWorldSpace(true);

			//m_pPointFX->loop = false;
			m_pLandIcon->SetDead();
			m_pTrailFX->loop = false;

			DirectX::XMFLOAT3 newVel;
			DirectX::XMVECTOR normVel = DirectX::XMLoadFloat3(GetOwner()->GetOwner()->GetTransform().GetZAxis());
			DirectX::XMStoreFloat3(&newVel, DirectX::XMVectorMultiply(normVel, DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(7, 7, 7))));

			m_Owner->GetOwner()->SetVelocity(newVel);
		}
		else
		{
			DirectX::XMFLOAT3 newVel;
			DirectX::XMStoreFloat3(&newVel, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m_fSpeeed, m_fSpeeed, m_fSpeeed)), DirectX::XMLoadFloat3(GetOwner()->GetOwner()->GetTransform().GetZAxis())));

			m_Owner->GetOwner()->SetVelocity(newVel);

			//reinterpret_cast<BehavLandPoint*>((*m_pLandIcon->GetComponent<AICompBase>()->m_vBehaviors.begin()))->m_currDist = sqrMag;
		}
	}

	

	



}

void BehavSpawning::LookAtGround()
{
	int tmp = 0;
	CollisionManager::IntersectLine2Triangle(*m_Owner->GetOwner()->GetTransform().GetPosition(), DirectX::XMFLOAT3(0, 0, 0), tmp, &m_vLandPoint);

}

void BehavSpawning::Reset()
{
	m_bLanded = false;
	m_logicType = bSpawn;
	m_bClose = false;
	m_bFirst = true;
	m_fTotalTimer = 0;
	m_pTrailFX = nullptr;
	m_pLandIcon = nullptr;
	cubeOrientation = false;
}

void BehavSpawning::CreateLandIcon()
{

	GameObject* actualObject = GetOwner()->GetOwner();
	ObjectManager* pObjMan = actualObject->m_ObjManager;
	DirectX::XMFLOAT3 newZ = *actualObject->GetTransform().GetZAxis();
	DirectX::XMFLOAT3 newPos;
	
	bool zAxis = false;

	float x = actualObject->GetTransform().GetZAxis()->x;
	float y = actualObject->GetTransform().GetZAxis()->y;
	float z = actualObject->GetTransform().GetZAxis()->z;

	if (abs(z) > abs(y) && abs(z) > abs(x))
	{
		zAxis = true;
	}

	if (zAxis && cubeOrientation)
	{
		DirectX::XMStoreFloat3(&newPos, DirectX::XMLoadFloat3(&m_vLandPoint) + (DirectX::XMLoadFloat3(actualObject->GetTransform().GetZAxis()) * -5));

	}
	else
	{
		DirectX::XMStoreFloat3(&newPos, DirectX::XMLoadFloat3(&m_vLandPoint) + (DirectX::XMLoadFloat3(actualObject->GetTransform().GetZAxis()) * -2));

	}


	newZ.x *= -1;
	newZ.y *= -1;
	newZ.z *= -1;





	m_pLandIcon = pObjMan->CreateObject(Landing_Indicator, newPos);

	if (!cubeOrientation)
	{
		m_pLandIcon->GetTransform().SetZAxis(newZ);
		m_pLandIcon->GetTransform().SetYAxis(*actualObject->GetTransform().GetYAxis());
		m_pLandIcon->GetTransform().SetXAxis(*actualObject->GetTransform().GetXAxis());
	}
	else
	{
		if (!zAxis)
		{
			m_pLandIcon->GetComponent<GroundFollowComponent>()->Update(1);
			m_pLandIcon->GetTransform().RotateBy(0, /*cubeOrientation.x **/ 90, 1, true);

		}
		else
		{

			XMFLOAT4X4 idenMat = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, newPos.x, newPos.y, newPos.z, 1);

			m_pLandIcon->GetTransform().SetLocalMatrix(idenMat);

			if (DirectX::XMVector3Dot(DirectX::XMLoadFloat3(actualObject->GetTransform().GetZAxis()), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0,0,1))).m128_f32[0] < 0)
			{
				m_pLandIcon->GetTransform().RotateBy(0, /*cubeOrientation.x **/ 180, 1, true);
			}
			


		}


		
	}
	
	float dist = DirectX::XMVector3Dot(XMLoadFloat3(actualObject->GetTransform().GetPosition()) - XMLoadFloat3(&newPos), XMLoadFloat3(actualObject->GetTransform().GetPosition()) - XMLoadFloat3(&newPos)).m128_f32[0];

	//reinterpret_cast<BehavLandPoint*>((*m_pLandIcon->GetComponent<AICompBase>()->m_vBehaviors.begin()))->m_StartDist = dist;
	//reinterpret_cast<BehavLandPoint*>((*m_pLandIcon->GetComponent<AICompBase>()->m_vBehaviors.begin()))->m_currDist = dist;

}

