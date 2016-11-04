#pragma once

#include "../VisualEffect/VisualEffect.h"

// Very Short : One Quarter of the original number of points
// Short : Half the original number of points
// Medium : The original number of points
// Long : Twice the original number of points
enum class TrailLength{ VeryShort, Short, Medium, Long, VeryLong };

struct RibbonPair
{
	Position P1;
	Direction N1;
	Direction R1;
	float W1;

	Position P2;
	Direction N2;
	Direction R2;
	float W2;

	float indexRatio;
};

class RibbonEffect : public VisualEffect
{
private:
	// Allows parentage without using parenting because the transform inheritance is broken
	GameObject* myParent = nullptr;

	struct RibbonVert
	{
		Position point;
		Direction normal;
		Direction rotationAxis;
		float width;
	};

	RibbonVert* m_vertArr;

	// The array of points which is being used.
	//Position* m_pointArr;

	// The iterator which determines the next point to initialize
	//int m_nextParticle = 0;

	// The normals of the points. By default, they point up.
	//Direction* m_normArr;

	// The rotation axis for each vertex
	//Direction* m_rotAxisArr;

	// The array of RibbonPairs
	RibbonPair* ribPairs = nullptr;

	// The current trail length
	TrailLength m_currTrailLength = TrailLength::Medium;

	// The number of points for the medium trail length setting.
	unsigned int m_mediumSize;

	// The current number of points in use in the above array.
	unsigned int m_numPoints;

	// How frequently are new points dropped? In distance, this is distance between. In time, this is time units.
	float m_dropFrequency;

	// Use Distance between current and next points to drop the next point? If not, use time instead.
	bool m_useDistance = false;
	// Previous point of monitored position, for distance calculations
	Position m_prevPos;

	// Measurement used for determining when to drop another node.
	float m_currentAmount;

	// Does this follow its transform for orientation?
	// By default, the trail's normal always points up
	bool m_followsTransform;

	// the axis which acts as the normal to follow when following the transform
	int normAxis;

	// the axis which acts as the normal to rotate the normaxis by within the geometry shader.
	int rotAxis;

	// half the width of a segment
	float halfWidth = 0.5f;

	float lowerHalfWidth = 0.5f;
	float upperHalfWidth = 0.5f;

	float toHalfWidth = 0.5f;

	// The color of the trail
	XMFLOAT4 trailColor;

	// Lerping number of points.
	unsigned int currentNumPairs;

	void ShiftPieces();

protected:
public:
	RibbonEffect(GameObject* toFollow = nullptr, unsigned int numberOfPoints = 30, bool measureBydistance = false, float dropFrequency = 0.01f, bool followsTransform = true, unsigned int followAxis = 2, unsigned int upAxis = 1);
	~RibbonEffect();

	void Update(float dt);

	void ShutDown();

	void GetPairs(RibbonPair* &Arr);

	unsigned int GetNumPairsTotal() { return (m_mediumSize * 4) - 1; }
	unsigned int GetNumPairsInUse() { return currentNumPairs; }

	XMFLOAT3 GetRotAxis() { return (!rotAxis ? *GetTransform().GetXAxis() : (rotAxis == 1 ? *GetTransform().GetYAxis() : *GetTransform().GetZAxis())); }

	float GetLowerHalfWidth() { return lowerHalfWidth; }
	float GetCurrentHalfWidth() { return halfWidth; }
	float GetUpperHalfWidth() { return upperHalfWidth; }
	void SetNextHalfWidth(const float f) { toHalfWidth = f; }

	float& GrabDropFrequency() { return m_dropFrequency; }

	XMFLOAT4& GrabTrailColor() { return trailColor; }

	const bool UsesDistance() const { return m_useDistance; }
	void SetDistanceUse(const bool b) { m_useDistance = b; }

	void SetParent(GameObject* g) { myParent = g; }

	//void SetNumPoints(int newNumPoints);
	void SetLength(TrailLength newlength);

	void ResetRibbon();
};