#pragma once
#include "BaseBehavior.h"
#include <DirectXMath.h>
#include "../Molecules/MoleculeEmitter.h"
class AIManager;

class BehavSpawning :
	public BaseBehavior
{

	friend AIManager;
	
	bool m_bLanded;
	bool m_bClose;
	bool m_bFirst;

	void LookAtGround();

	const DirectX::XMFLOAT3 GetLandingPoint() const { return m_vLandPoint; }
	void SetLandingPoint(const DirectX::XMFLOAT3 v);

	float m_fSpeeed;
	float m_fTotalTimer;

	MoleculeEmitter* m_pTrailFX;
	GameObject* m_pLandIcon;

	

	void CreateLandIcon();

public:
	BehavSpawning();
	~BehavSpawning();
	void DoLogic(float _dt);
	void Reset();
	DirectX::XMFLOAT3 m_vLandPoint;
	bool cubeOrientation;
};

