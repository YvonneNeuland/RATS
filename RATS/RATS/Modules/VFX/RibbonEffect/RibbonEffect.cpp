#include "stdafx.h"
#include "RibbonEffect.h"

#include <cstring>

#define SafeDelArr(pointer) if(pointer) delete[] pointer; pointer = nullptr

RibbonEffect::RibbonEffect(GameObject* toFollow, unsigned int numberOfPoints, bool measureBydistance, float dropRate, bool followsTransform, unsigned int followAxis, unsigned int upAxis)
	: myParent(toFollow), m_numPoints(numberOfPoints), m_useDistance(measureBydistance), m_dropFrequency(dropRate), m_followsTransform(followsTransform), normAxis(followAxis), rotAxis(upAxis)
{
	XMFLOAT3 myPos = *GetTransform().GetPosition();

	m_mediumSize = numberOfPoints;

	XMFLOAT3 myNorm;
	XMFLOAT3 myRotAxis;
	if (followsTransform)
	{
		myNorm = (!followAxis?*GetTransform().GetXAxis():(followAxis==1?*GetTransform().GetYAxis():*GetTransform().GetZAxis()));
		myRotAxis = (!upAxis ? *GetTransform().GetXAxis() : (followAxis == 1 ? *GetTransform().GetYAxis() : *GetTransform().GetZAxis()));
	}
	else
	{
		myNorm = Direction(0, 0, 1);
		myRotAxis = Direction(0, 1, 0);
	}

	unsigned int wholetrailsize = m_mediumSize * 4;

	m_vertArr = new RibbonVert[wholetrailsize];

	RibbonVert* currRibVert;
	for (unsigned int currPoint = 0; currPoint < wholetrailsize; ++currPoint)
	{
		currRibVert = &m_vertArr[currPoint];

		currRibVert->point = myPos;
		currRibVert->normal = myNorm;
		currRibVert->rotationAxis = myRotAxis;
		currRibVert->width = halfWidth;
	}

	trailColor = { 1, 1, 1, 1 };

	m_currentAmount = 0;

	currentNumPairs = m_numPoints - 1;

	lowerHalfWidth = halfWidth;
	upperHalfWidth = lowerHalfWidth * 3.5f;
}

RibbonEffect::~RibbonEffect()
{

}

void RibbonEffect::ShiftPieces()
{
	unsigned int wholesize = currentNumPairs + 1;
	for (int i = wholesize - 2; i >= 0; --i)
		m_vertArr[i + 1] = m_vertArr[i];

}

void RibbonEffect::Update(float dt)
{
	if (myParent)
	{
		GetTransform().SetLocalMatrix(myParent->GetTransform().GetLocalMatrix());
	}

	if (m_useDistance)
	{
		Velocity currDist = *GetTransform().GetPosition();
		currDist.x -= m_prevPos.x;
		currDist.y -= m_prevPos.y;
		currDist.z -= m_prevPos.z;

		m_currentAmount += sqrtf((currDist.x*currDist.x) + (currDist.y*currDist.y) + (currDist.z*currDist.z));
	}
	else
	{
		m_currentAmount += dt;
	}

	if (m_currentAmount >= m_dropFrequency)
	{
		m_currentAmount = 0;

		ShiftPieces();

		m_vertArr[0].point = *GetTransform().GetPosition();
		
		if (m_followsTransform)
		{
			m_vertArr[0].normal =
				(!normAxis ? *GetTransform().GetXAxis() : (normAxis == 1 ? *GetTransform().GetYAxis() : *GetTransform().GetZAxis()));

			m_vertArr[0].rotationAxis = 
				(!rotAxis ? *GetTransform().GetXAxis() : (rotAxis == 1 ? *GetTransform().GetYAxis() : *GetTransform().GetZAxis()));
		}
		else
		{
			m_vertArr[0].normal = Direction(0, 0, 1);
			m_vertArr[0].rotationAxis = Direction(0, 1, 0);
		}

		m_vertArr[0].width = halfWidth;

		if (currentNumPairs > m_numPoints - 1)
			--currentNumPairs;

	}

	halfWidth += (toHalfWidth - halfWidth) * (dt * 16);
	halfWidth = fmaxf(lowerHalfWidth, fminf(upperHalfWidth, halfWidth));

	m_prevPos = *GetTransform().GetPosition();
}

void RibbonEffect::ShutDown()
{
	SafeDelArr(ribPairs);
	SafeDelArr(m_vertArr);
}

void RibbonEffect::GetPairs(RibbonPair* &Arr)
{
	unsigned int wholesize = m_mediumSize * 4 - 1;

	if (ribPairs == nullptr)
	{
		ribPairs = new RibbonPair[wholesize];

		//RibbonPair derp[10000];

		RibbonPair* currentRibPair;
		RibbonVert* currentRibVert, *nextRibVert;
		for (unsigned int currPair = 0; currPair < wholesize; ++currPair)
		{
			currentRibPair = &ribPairs[currPair];
			currentRibVert = &m_vertArr[currPair];
			nextRibVert = &m_vertArr[currPair + 1];

			currentRibPair->P1 = currentRibVert->point;
			currentRibPair->N1 = currentRibVert->normal;
			currentRibPair->R1 = currentRibVert->rotationAxis;
			currentRibPair->W1 = currentRibVert->width;

			currentRibPair->P2 = nextRibVert->point;
			currentRibPair->N2 = nextRibVert->normal;
			currentRibPair->R2 = nextRibVert->rotationAxis;
			currentRibPair->W2 = nextRibVert->width;

			currentRibPair->indexRatio = (currPair < currentNumPairs ? 1 - (float(currPair + 1) / currentNumPairs) : 0);

			//derp[currPair] = *currentRibPair;
		}
	}
	else
	{
		for (int i = currentNumPairs - 2; i >= 0; --i)
			ribPairs[i + 1] = ribPairs[i];

		ribPairs[0].P1 = m_vertArr[0].point;
		ribPairs[0].N1 = m_vertArr[0].normal;
		ribPairs[0].R1 = m_vertArr[0].rotationAxis;
		ribPairs[0].W1 = m_vertArr[0].width;

		ribPairs[0].P2 = m_vertArr[1].point;
		ribPairs[0].N2 = m_vertArr[1].normal;
		ribPairs[0].R2 = m_vertArr[1].rotationAxis;
		ribPairs[0].W2 = m_vertArr[1].width;

		for (unsigned int i = 0; i < wholesize; ++i)
			ribPairs[i].indexRatio = (i < currentNumPairs ? 1 - (float(i + 1) / currentNumPairs) : 0);


	}

	Arr = ribPairs;
}

void RibbonEffect::ResetRibbon()
{
	myParent = nullptr;

	// Redux of Constructor
	XMFLOAT3 myPos = *GetTransform().GetPosition();
	bool followsTransform = true;
	XMFLOAT3 myNorm;
	XMFLOAT3 myRotAxis;

	if (followsTransform)
	{
		myNorm = (!normAxis ? *GetTransform().GetXAxis() : (normAxis == 1 ? *GetTransform().GetYAxis() : *GetTransform().GetZAxis()));
		myRotAxis = (!rotAxis ? *GetTransform().GetXAxis() : (normAxis == 1 ? *GetTransform().GetYAxis() : *GetTransform().GetZAxis()));
	}
	else
	{
		myNorm = Direction(0, 0, 1);
		myRotAxis = Direction(0, 1, 0);
	}

	unsigned int wholetrailsize = m_mediumSize * 4;

	RibbonVert* currRibVert;
	for (unsigned int currPoint = 0; currPoint < wholetrailsize; ++currPoint)
	{
		currRibVert = &m_vertArr[currPoint];

		currRibVert->point = myPos;
		currRibVert->normal = myNorm;
		currRibVert->rotationAxis = myRotAxis;
		currRibVert->width = lowerHalfWidth;
	}

	if (ribPairs)
	{
		RibbonPair* currRibPair;
		for (unsigned int currPair = 0; currPair < wholetrailsize - 1; ++currPair)
		{
			currRibPair = &ribPairs[currPair];

			currRibPair->P1 = currRibPair->P2 = myPos;
			currRibPair->N1 = currRibPair->N2 = myNorm;
			currRibPair->R1 = currRibPair->R2 = myRotAxis;
			currRibPair->W1 = currRibPair->W2 = lowerHalfWidth;
		}
	}

	trailColor = { 1, 1, 1, 1 };

	m_currentAmount = 0;
}

void RibbonEffect::SetLength(TrailLength newlength)
{
	if (m_currTrailLength != newlength)
	{
		if (newlength == TrailLength::VeryShort)
			m_numPoints = m_mediumSize / 4;
		else if (newlength == TrailLength::Short)
			m_numPoints = m_mediumSize / 2;
		else if (newlength == TrailLength::Medium)
			m_numPoints = m_mediumSize;
		else if (newlength == TrailLength::Long)
			m_numPoints = m_mediumSize * 2;
		else // Very Long
			m_numPoints = m_mediumSize * 4;

		m_currTrailLength = newlength;

		if (currentNumPairs < m_numPoints)
			currentNumPairs = m_numPoints - 1;
	}
}

//RibbonEffect::RibbonEffect(GameObject* toFollow, unsigned int numberOfPoints, bool measureBydistance, float dropRate, bool followsTransform, unsigned int followAxis, unsigned int upAxis)
//: myParent(toFollow), m_numPoints(numberOfPoints), m_useDistance(measureBydistance), m_dropFrequency(dropRate), m_followsTransform(followsTransform), normAxis(followAxis), rotAxis(upAxis)