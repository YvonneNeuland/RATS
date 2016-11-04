#pragma once

#include "../VisualEffect/VisualEffect.h"

class ElectricityEffect : public VisualEffect
{
private:
	int m_numSegments;

	Position* m_positions;

	Position m_targetPos;
	Position m_OrigLastPos;

	std::default_random_engine randeng;
	std::uniform_real_distribution<float>* uniformrand;

	Color flowColor;

	float timeToWait = 0;
	float maxToWait = 0.05f;

protected:
public:
	ElectricityEffect(Position targ = XMFLOAT3(0,0,0), int segs = 6, XMFLOAT4 col = XMFLOAT4(1,1,0,1));
	~ElectricityEffect();

	Position* GetPositions() { return m_positions; }

	//position array size is always numsegs+1 for segment mode
	int GetNumSegments() { return m_numSegments; }

	Position& GrabTarget() { return m_targetPos; }

	Color& GrabColor() { return flowColor; }
	XMFLOAT4 GetColorAsXMFloat4() { XMFLOAT4 col; memcpy(&col.x, &flowColor.r, sizeof(float) * 4); return col; }

	Position GetOrigLastPos() const { return m_OrigLastPos; }
	void SetOrigLastPos(Position val) { m_OrigLastPos = val; }
	
	void Update(float dt);

	void ResetTarget();
	void ResetTimer() { timeToWait = 0; }
	void ResetElectricity();

	void SetMaxToWait(const float newwait) { maxToWait = newwait; }
};