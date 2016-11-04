#pragma once

#include "../MovementCompBase.h"

class GroundFollowComponent : public MovementCompBase
{
private:

	// How far off the ground to clamp
	float offset = 1.0f;

	float fineness = 0.05f;

	// How "soft" the clamp is.
	float inverseSoftness = 0;

protected:

public:
	GroundFollowComponent();
	~GroundFollowComponent();

	const float GetGroundClampOffset() const { return offset; }
	void SetGroundClampOffset(const float f) { offset = f; }

	float& GrabFineness(){ return fineness; }

	const float GetSoftness() const { return (inverseSoftness ? 1 / inverseSoftness : 0); }
	// How soft is the ground following? from 0 to 1.
	void SetSoft(const float howSoft) { inverseSoftness = (howSoft ? 1 / howSoft : 0); }

	void Update(float dt);
};
