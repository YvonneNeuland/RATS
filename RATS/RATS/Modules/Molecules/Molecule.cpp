#include "stdafx.h"
#include "Molecule.h"


Molecule::Molecule()
{
	Reset();
}


Molecule::~Molecule()
{
}

void Molecule::Update(float _dt)
{
	m_fLifetime -= _dt;

	if (m_fLifetime <= 0)
	{
		m_fLifetime = -1;
	}

}

void Molecule::Reset()
{
	m_fLifetime = -1;
	m_Position = m_Velocity = DirectX::XMFLOAT3(0, 0, 0);
	m_Color = DirectX::XMFLOAT4(1,1,1,1);
	m_Scale = 1.0f;

}

Molecule& Molecule::operator=(const Molecule& rhs)
{
	this->m_Color			= rhs.m_Color;
	this->m_fLifetime		= rhs.m_fLifetime;
	this->m_Position		= rhs.m_Position;
	this->m_Velocity		= rhs.m_Velocity;
	this->m_Scale			= rhs.m_Scale;
	return *this;
}
