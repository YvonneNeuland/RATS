#pragma once
#include <DirectXMath.h>
class Molecule
{
public:
	float m_fLifetime;

	DirectX::XMFLOAT3 m_Velocity;

	DirectX::XMFLOAT3 m_Position;

	DirectX::XMFLOAT4 m_Color;

	float m_Scale;
	
public:

	float GetLifetime() const { return m_fLifetime; }
	void SetLifetime(float val) { m_fLifetime = val; }
	DirectX::XMFLOAT3 GetVelocity() const { return m_Velocity; }
	void SetVelocity(DirectX::XMFLOAT3 val) { m_Velocity = val; }
	DirectX::XMFLOAT3 GetPosition() const { return m_Position; }
	void SetPosition(DirectX::XMFLOAT3 val) { m_Position = val; }

	void Update(float _dt);
	void Reset();

	Molecule& operator=(const Molecule& rhs);

	Molecule();
	~Molecule();
};

