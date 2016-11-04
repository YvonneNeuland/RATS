#include "stdafx.h"
#include "HealthComponent.h"
#include "../Object Manager/GameObject.h"


HealthComponent::HealthComponent()
{
	m_fHealth = 0;
	m_fMaxHealth = 0;
	timer = false;
}


HealthComponent::~HealthComponent()
{
}

void HealthComponent::Update(float dt)
{
	if(timer == true)
	{
		m_fHealth -= dt;
	}

	if (GetOwner()->GetType() == eOBJECT_TYPE::Pylon_Test && m_fHealth/m_fMaxHealth < 0.1f)
	{
		m_fHealth = 0.1f * m_fMaxHealth;
	}

	if(m_fHealth <= 0.0f)
	{
		m_fHealth = 0.0f;
		m_pOwner->SetDead();
	}

	if (m_fEnergy < 0.0f)
	{
		m_fEnergy = 0.0f;
	}

	if (m_bInvincible && GetOwner()->GetType() != Enemy_Boss)
	{
		m_fHealth = m_fMaxHealth;
	}
}

void HealthComponent::SetHealth(float val)
{
	  m_fHealth = val; 

	if (m_fHealth > m_fMaxHealth)
	{
		m_fHealth = m_fMaxHealth;
	}
}

void HealthComponent::SetEnergy(float val)
{
	m_fEnergy = val;

	if (m_fEnergy > m_fMaxEnergy)
	{
		m_fEnergy = m_fMaxEnergy;
	}
}

void HealthComponent::Reset()
{
	m_fHealth = 0;
	m_fMaxHealth = 0;
	timer = false;
	m_fMaxEnergy = 300;
	m_fEnergy = 0;
	m_bInvincible = false;
}