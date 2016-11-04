#pragma once
#include "../Components/BaseComponent.h"
#include <DirectXMath.h>

class CollisionManager;

class ColliderComp : public BaseComponent
{
	friend CollisionManager;
public:
	enum eCOLLIDERTYPE {Sphere = 0, Capsule = 1, AABB = 2};

private:
	
	float m_fRadius;

	float m_fAggroRadius;
	
	DirectX::XMFLOAT3 m_Min;

	DirectX::XMFLOAT3 m_Max;

	DirectX::XMFLOAT3 m_Start;

	DirectX::XMFLOAT3 m_End;

	eCOLLIDERTYPE m_Type;

public:
	ColliderComp();
	~ColliderComp();

	void Update(float _dt);

	float GetRadius() const { return m_fRadius; }
	void SetRadius(float val) { m_fRadius = val; }
	DirectX::XMFLOAT3 GetMin() const { return m_Min; }
	void SetMin(DirectX::XMFLOAT3 val) { m_Min = val; }
	DirectX::XMFLOAT3 GetMax() const { return m_Max; }
	void SetMax(DirectX::XMFLOAT3 val) { m_Max = val; }
	DirectX::XMFLOAT3 GetStart() const { return m_Start; }
	void SetStart(DirectX::XMFLOAT3 val) { m_Start = val; }
	DirectX::XMFLOAT3 GetEnd() const { return m_End; }
	void SetEnd(DirectX::XMFLOAT3 val) { m_End = val; }
	ColliderComp::eCOLLIDERTYPE GetType() const { return m_Type; }
	void SetType(ColliderComp::eCOLLIDERTYPE val) { m_Type = val; }
	float GetAggroRadius() const { return m_fAggroRadius; }
	void SetAggroRadius(float val) { m_fAggroRadius = val; }
	//Sphere - Radius
	//AABB - Min/Max
	//Capsule - Start/End/Radius

	void Reset();



};

