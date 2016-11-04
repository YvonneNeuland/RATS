#pragma once

#include "../../Object Manager/GameObject.h"

#include <DirectXMath.h>
#include <dxgitype.h>
#include <random>

using namespace DirectX;

typedef XMFLOAT3 Position, Direction, Rotation, Velocity, Scale;
typedef DXGI_RGBA Color;

//struct VFXInfoDummy
//{
//
//	// Unordered Access View
//	//ID3D11UnorderedAccessView* unorderedAccessView;
//
//	void InitParticles(void* particleArray, unsigned int size)
//	{
//
//	}
//
//	void UpdateParticles(void* particleArray, unsigned int size)
//	{
//
//	}
//};

class VisualEffect : public GameObject
{
private:

	VisualEffect(const VisualEffect&) = delete;
	VisualEffect(const VisualEffect&&) = delete;
	VisualEffect& operator=(const VisualEffect&) = delete;
	VisualEffect& operator=(const VisualEffect&&) = delete;

protected:

public:
	VisualEffect() ;
	 ~VisualEffect() ;

	void Update(float dt) = 0;
};