#pragma once
#include "BaseComponent.h"
class HealthComponent :
	public BaseComponent
{
	float m_fHealth;

	float m_fMaxHealth;

	bool timer;

	float m_fMaxEnergy = 100.0f;
	float m_fEnergy = 0.0f;

	bool m_bInvincible = false;

public:
	HealthComponent();
	~HealthComponent();

	void Update(float dt);
	float GetHealth() const { return m_fHealth; }
	void SetHealth(float val);
	float GetMaxHealth() const { return m_fMaxHealth; }
	void SetMaxHealth(float val) { m_fHealth = m_fMaxHealth = val; }

	void SetTimer(bool on) { timer = on; }
	void Reset();

	float GetMaxEnergy(void) const { return m_fMaxEnergy; }
	float GetEnergy(void) const { return m_fEnergy; }
	void SetMaxEnergy(float maxEnergy) { m_fMaxEnergy = maxEnergy; }
	void SetEnergy(float energy);
	void SetInvincibility(bool invincible){ m_bInvincible = invincible; }
	bool GetInvincibility() { return m_bInvincible; }
};

