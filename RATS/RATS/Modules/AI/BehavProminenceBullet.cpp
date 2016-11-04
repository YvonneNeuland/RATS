#include "stdafx.h"
#include "BehavProminenceBullet.h"
#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/Transform.h"
#include "../Collision/CollisionManager.h"
#include "../Object Manager/ObjectManager.h"
#include "../Molecules/MoleculeManager.h"
#include "../Components/Movement/GroundFollowComponent.h"

BehavProminenceBullet::BehavProminenceBullet()
{
	m_logicType = bProminenceBullet;
}


BehavProminenceBullet::~BehavProminenceBullet()
{
}

void BehavProminenceBullet::DoLogic(float _dt)
{
	m_fFrameTimer += _dt;

	if (m_fFrameTimer > 0.01667)
	{
		m_fFrameTimer = 0.0f;

		if (!targetSet)
		{
			SetTargetPosition();
		}

		DirectX::XMVECTOR to_world = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPosition), DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetPosition()));
		float sqrWorldMag = DirectX::XMVector3Dot(to_world, to_world).m128_f32[0];

		DirectX::XMVECTOR to_start = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&startingPosition), DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetPosition()));
		float sqrStartMag = DirectX::XMVector3Dot(to_world, to_world).m128_f32[0];

		if (sqrWorldMag < 21.0f)
		{
			targetSet = false;
			velocityIncreased = false;
			m_Owner->GetOwner()->SetDead();
		}
		else if (sqrStartMag > 800)
		{
			TranslateTargetUp(1.0f);
			LookAt();
		}
		else
		{
			if (!velocityIncreased)
			{
				m_Owner->GetOwner()->SetVelocity(XMFLOAT3(m_Owner->GetOwner()->GetVelocity().x, m_Owner->GetOwner()->GetVelocity().y + 1.5f, m_Owner->GetOwner()->GetVelocity().z));
				velocityIncreased = true;

				/*GroundFollowComponent* followComp = (GroundFollowComponent*)m_Owner->GetOwner()->GetComponent("GroundFollowComponent");
				followComp->SetSoft(1.0f);*/
			}

			LookAt();
		}
	}
}

void BehavProminenceBullet::SetTargetPosition()
{
	
	TranslateTargetUp(15.0f);
	startingPosition = *m_Owner->GetOwner()->GetTransform().GetPosition();

	int addX = rand() % 15 + 15;

	//int addY = rand() % 15 + 5;
	int addZ = rand() % 15 + 15;

	int negativeX = rand() % 2 + 1;
	int negativeZ = rand() % 2 + 1;

	if (negativeX % 2 == 0)
	{
		addX *= -1;
	}

	if (negativeZ % 2 == 0)
	{
		addZ *= -1;
	}	

	DirectX::XMVECTOR tmpOffset = DirectX::XMLoadFloat3(&startingPosition);
	tmpOffset += (DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetXAxis()) * DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((float)addX, (float)addX, (float)addX)));

	tmpOffset += (DirectX::XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetZAxis()) * DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((float)addZ, (float)addZ, (float)addZ)));

	DirectX::XMStoreFloat3(&startingPosition, tmpOffset);

	XMVECTOR targetVec = XMLoadFloat3(&startingPosition);
	float mag = XMVector3Length(targetVec).m128_f32[0];

	int tmp = 0;
	CollisionManager::IntersectLine2Triangle(startingPosition, DirectX::XMFLOAT3(0, 0, 0), tmp, &targetPosition);

	targetVec = XMLoadFloat3(&startingPosition);
	mag = XMVector3Length(targetVec).m128_f32[0];
	startingPosition = *m_Owner->GetOwner()->GetTransform().GetPosition();

	targetSet = true;
	//MoleculeEmitter* mol = this->m_Owner->GetOwner()->m_ObjManager->m_Molecules->BuildPosEmitter(targetPosition, 1000, "DebugMolecule");

	//mol->m_constDirection = XMFLOAT3(0.0f, 50.0f, 50.0f);
	//mol->m_spawnTime = 0.1f;
	//mol->m_spawnCount = 10;
}

void BehavProminenceBullet::TranslateTargetUp(float offset)
{

	XMFLOAT4X4 localMatrix = m_Owner->GetOwner()->GetTransform().GetLocalMatrix();
	XMFLOAT4X4 translationMatrix = XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, offset, 0.0f, 1.0f);

	XMMATRIX translation = XMLoadFloat4x4(&translationMatrix);
	XMMATRIX rotation = XMLoadFloat4x4(&localMatrix);

	XMMATRIX newLocal = XMMatrixMultiply(translation, rotation);

	XMStoreFloat4x4(&localMatrix, newLocal);
	m_Owner->GetOwner()->GetTransform().SetLocalMatrix(localMatrix);
}

void BehavProminenceBullet::LookAt()
{
	currentPosition = *m_Owner->GetOwner()->GetTransform().GetPosition();

	XMVECTOR targetPos = XMLoadFloat3(&targetPosition);
	XMVECTOR entityPos = XMLoadFloat3(&currentPosition);
	XMVECTOR zAxis = targetPos - entityPos;
	zAxis = XMVector3Normalize(zAxis);

	XMVECTOR worldUpVector = XMLoadFloat3(m_Owner->GetOwner()->GetTransform().GetYAxis());
	XMVECTOR xAxis;
	xAxis = XMVector3Cross(worldUpVector, zAxis);
	xAxis = XMVector3Normalize(xAxis);

	XMVECTOR yAxis;
	yAxis = XMVector3Cross(zAxis, xAxis);
	yAxis = XMVector3Normalize(yAxis);

	XMFLOAT3 xA, yA, zA;
	XMStoreFloat3(&xA, xAxis);
	XMStoreFloat3(&yA, yAxis);
	XMStoreFloat3(&zA, zAxis);

	m_Owner->GetOwner()->GetTransform().SetXAxis(xA);
	m_Owner->GetOwner()->GetTransform().SetYAxis(yA);
	m_Owner->GetOwner()->GetTransform().SetZAxis(zA);
}