#pragma once

#include "GameObject.h"


class CameraObject : public GameObject
{
private:
	GameObject* target;

	DirectX::XMFLOAT3 offset;

	// This is multiplied into delta during the movement lerp. Set to 1000+ to snap within one frame.
	float moveSnapSpeed;

	// This is multiplied into delta during the rotation lerp. set to 1000+ to snap within one frame.
	float rotSnapSpeed;

	// Is the offset in world space? CAUTION: IF THE TARGET IS A PLAYER, THIS DOES NOT GET USED.
	bool offsetInWorldSpace;

	void Move(float ratio);
	void TurnToTarget(float ratio);

protected:

public:
	CameraObject(GameObject* targ = nullptr,
				 DirectX::XMFLOAT3 offs = DirectX::XMFLOAT3(0, 0, 0),
				 float movesnapspd = 10000,
				 float rotsnapspd = 10000,
				 bool worldspaceoffset = true)
		: target(targ),
		  offset(offs),
		  moveSnapSpeed(movesnapspd),
		  rotSnapSpeed(rotsnapspd),
		  offsetInWorldSpace(worldspaceoffset) {}
	~CameraObject();

	const GameObject* GetTarget() const { return target; }
	void SetTarget(GameObject* targ) { target = targ; }
	
	const bool IsOffsetInWorldSpace() const { return offsetInWorldSpace; }
	void SetOffSetInWorldSpace(const bool yes) { offsetInWorldSpace = yes; }

	DirectX::XMFLOAT3& GrabOffset() { return offset; }
	float& GrabMoveSnapSpeed() { return moveSnapSpeed; }
	float& GrabRotSnapSpeed() { return rotSnapSpeed; }

	void Update(float dt);

};
