#include "stdafx.h"
#include "BehavWhipBullet.h"

#include <DirectXMath.h>

#include "../AICompBase.h"

#include "../../Object Manager/GameObject.h"
#include "../../Renderer/D3DGraphics.h"



using namespace DirectX;
void Whip::Update(float dt)
{
	//if (!on) return;
	m_timeSinceLastUpdate += dt;

	if (m_timeSinceLastUpdate < 0.0052631578947368f)
	{
		return;
	}
	else
	{
		m_timeSinceLastUpdate = 0;
	}

	m_currBullet = m_prevBullet = m_cBullets.begin();
	++m_currBullet;

	float newCurrRot, dotSign;
	XMFLOAT3 newCurrPos;
	XMVECTOR currBulletPos, prevBulletPos, prevBulletZ, currBulletZ, currBulletX;
	while (m_currBullet != m_cBullets.end())
	{




		int numUpdates = (int)(dt * 190);

		if (numUpdates < 1)
		{
			numUpdates = 1;
		}

		for (int i = 0; i < numUpdates; i++)
		{
			prevBulletZ = XMLoadFloat3((*m_prevBullet)->GetOwner()->GetTransform().GetZAxis());
			currBulletX = XMLoadFloat3((*m_currBullet)->GetOwner()->GetTransform().GetXAxis());
			currBulletZ = XMLoadFloat3((*m_currBullet)->GetOwner()->GetTransform().GetZAxis());

			newCurrRot = XMVector3AngleBetweenVectors(prevBulletZ, currBulletZ).m128_f32[0];

			dotSign = XMVector3Dot(prevBulletZ, currBulletX).m128_f32[0];

			if (dotSign != 0)
			{
				newCurrRot *= dotSign / abs(dotSign);


				if (newCurrRot && on)
					(*m_currBullet)->GetOwner()->GetTransform().RotateBy(1, newCurrRot * m_fStiffness);

				currBulletPos = XMLoadFloat3((*m_currBullet)->GetOwner()->GetTransform().GetPosition());
				prevBulletPos = XMLoadFloat3((*m_prevBullet)->GetOwner()->GetTransform().GetPosition());
				currBulletZ = XMLoadFloat3((*m_currBullet)->GetOwner()->GetTransform().GetZAxis());


				XMStoreFloat3(&newCurrPos, XMVectorLerp(prevBulletPos, currBulletPos + (currBulletZ), m_fCohesion));

				(*m_currBullet)->GetOwner()->GetTransform().SetPosition(newCurrPos);
			}
		}
		
		++m_prevBullet;
		++m_currBullet;
	}
}

BehavWhipBullet::BehavWhipBullet()
{
	m_logicType = bWhipBullet;
}
