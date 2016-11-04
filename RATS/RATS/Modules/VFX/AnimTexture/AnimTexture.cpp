#include "stdafx.h"
#include "AnimTexture.h"

#include "../../Object Manager/ObjectManager.h"

AnimTexture::AnimTexture(int panelsX, int panelsY, int timesToLoop, float flipTime, bool billboardToCam)
	: m_numPanelsX(panelsX), m_numPanelsY(panelsY), m_timesToLoop(timesToLoop), m_flipTime(flipTime), m_snapToCamera(billboardToCam)
{
	m_panelWidth = 1.0f / m_numPanelsX;
	m_panelHeight = 1.0f / m_numPanelsY;

	float currX = m_panelWidth * m_currPanelX,
		currY = m_panelHeight * m_currPanelY;

	m_vertUVs[0].x = currX;
	m_vertUVs[0].y = currY;
	m_vertUVs[1].x = currX + m_panelWidth;
	m_vertUVs[1].y = currY;
	m_vertUVs[2].x = currX;
	m_vertUVs[2].y = currY + m_panelHeight;
	m_vertUVs[3].x = currX + m_panelWidth;
	m_vertUVs[3].y = currY + m_panelHeight;

	m_parent = nullptr;
}

AnimTexture::~AnimTexture()
{

}

void AnimTexture::Update(float dt)
{
	
	if (m_snapToCamera)
	{
		XMFLOAT3 toCamera;
		XMStoreFloat3(&toCamera,
			-XMVector3Normalize(XMLoadFloat3(m_cam->GetTransform().GetPosition()) - XMLoadFloat3(GetTransform().GetPosition())));
		SetBillboardDirection(toCamera);
	}

	if (!m_isDone && m_currTime >= m_flipTime)
	{
		++m_currPanelX;

		if (!(m_currPanelX%m_numPanelsX))
		{
			++m_currPanelY;
			if (!(m_currPanelY%m_numPanelsY))
			{
				if (m_timesToLoop > 0 && (--m_timesToLoop) == 0)
					m_isDone = true;

				m_currPanelY = 0;
			}
			m_currPanelX = 0;
		}

		float currX = m_panelWidth * m_currPanelX,
			  currY = m_panelHeight * m_currPanelY;

		m_vertUVs[0].x = currX;
		m_vertUVs[0].y = currY;
		m_vertUVs[1].x = currX + m_panelWidth;
		m_vertUVs[1].y = currY;
		m_vertUVs[2].x = currX;
		m_vertUVs[2].y = currY + m_panelHeight;
		m_vertUVs[3].x = currX + m_panelWidth;
		m_vertUVs[3].y = currY + m_panelHeight;

		m_currTime = 0;
	}
	else
		m_currTime += dt;

	if( m_isDone)
	{
		SetFlags(MostlyDead);
	}
}

void AnimTexture::SetBillboardDirection(Direction dir)
{
	XMFLOAT3 newX, newY;
	Transform& myTrans = GetTransform();

	XMStoreFloat3(&newX, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(myTrans.GetYAxis()), XMLoadFloat3(&dir))));
	XMStoreFloat3(&newY, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&dir), XMLoadFloat3(&newX))));

	myTrans.SetXAxis(newX);
	myTrans.SetYAxis(newY);
	myTrans.SetZAxis(dir);

}

void AnimTexture::Reset()
{
	m_isDone = false;
	m_timesToLoop = 1;
	m_currTime = 0;
	m_currPanelX = 0;
	m_currPanelY = 0;
	m_parent = nullptr;

	m_scale = 5.0f;

	SetPanels(4, 4);



	//SetPanels(4,4);
	//this->GetTransform().SetPosition(DirectX::XMFLOAT3(0,0,0));
}