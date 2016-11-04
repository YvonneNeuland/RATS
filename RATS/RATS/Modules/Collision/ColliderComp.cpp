#include "stdafx.h"
#include "ColliderComp.h"


ColliderComp::ColliderComp()
{
}


ColliderComp::~ColliderComp()
{
}

void ColliderComp::Update(float _dt)
{

}

void ColliderComp::Reset()
{
	DirectX::XMFLOAT3 blank = DirectX::XMFLOAT3(1,1,1);
	m_fRadius = 0.0f;
	m_fAggroRadius = 0;
	m_Min = blank;
	m_Max = blank;
	m_Start = blank;
	m_End = blank;
	m_Type = eCOLLIDERTYPE::Sphere;
}