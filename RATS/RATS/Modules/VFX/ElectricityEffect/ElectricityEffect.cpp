#include "stdafx.h"
#include "ElectricityEffect.h"

ElectricityEffect::ElectricityEffect(Position targ, int segs, XMFLOAT4 col) : m_targetPos(targ), m_numSegments(segs)
{
	m_positions = new Position[segs + 1];
	m_positions[0] = *GetTransform().GetPosition();
	for (int i = 1; i < segs + 1; ++i)
		m_positions[i] = m_positions[0];

	m_positions[0] = XMFLOAT3(0, -50, 0);

	uniformrand = new std::uniform_real_distribution<float>(-1, 1);

	memcpy(&flowColor.r, &col.x, sizeof(float) * 4);
}

ElectricityEffect::~ElectricityEffect()
{
	delete uniformrand;
	delete[] m_positions;
}

void ElectricityEffect::Update(float dt)
{
	if (timeToWait <= 0)
	{

		XMVECTOR start = XMLoadFloat3(&m_positions[0]), end = XMLoadFloat3(&m_positions[m_numSegments]);
		if (!DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&m_targetPos), DirectX::XMLoadFloat3(&Position(0, 0, 0))))
		{
			m_positions[m_numSegments] = m_targetPos;
			end = XMLoadFloat3(&m_targetPos);
		}


		XMVECTOR axis = end - start;

		int randquataxis = rand() % 3;

		XMVECTOR quat = XMQuaternionRotationRollPitchYaw((randquataxis == 0 ? 90.0f : 0), (randquataxis == 1 ? 90.0f : 0), (randquataxis == 2 ? 90.0f : 0));
		XMVECTOR axisperp = XMVector3Normalize(XMVector3Rotate(axis, quat)) * 3;

		XMVECTOR currPoint;

		for (int i = 1; i < m_numSegments; ++i)
		{
			currPoint = XMVectorLerp(start, end, float(i) / m_numSegments);
			currPoint += axisperp * (*uniformrand)(randeng);
			XMStoreFloat3(&m_positions[i], currPoint);
		}

		timeToWait = maxToWait;
	}
	else
	{
		timeToWait -= dt;

		for (int i = 1; i < m_numSegments; ++i)
		{
			XMStoreFloat3(&m_positions[i], XMLoadFloat3(&m_positions[i]) + (XMLoadFloat3(&m_positions[i])*dt*(*uniformrand)(randeng)*(rand() % 2 ? 1.0f : -1.0f)));
		}

	}
}

void ElectricityEffect::ResetTarget()
{
	m_targetPos = Position(0, 0, 0);
	m_positions[m_numSegments] = m_OrigLastPos;
}

void ElectricityEffect::ResetElectricity()
{
	ResetTarget();
	ResetTimer();

	ZeroMemory(m_positions, sizeof(Position) * m_numSegments);

	m_targetPos = Position(0,0,0);
	m_OrigLastPos = m_targetPos;
}