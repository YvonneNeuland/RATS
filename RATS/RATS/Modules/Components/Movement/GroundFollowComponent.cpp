#include "stdafx.h"
#include "GroundFollowComponent.h"
#include "../../Object Manager/GameObject.h"

#include "../../Upgrade System/GameData.h"

extern GameData* gameData;

#pragma region Terrain Follow Functions

// SAVE FOR LATER
// http://www.merl.com/publications/docs/TR96-17.pdf
// THAT'S IT
// THEY USE A QUAD/OCTREE TO SORT THE TRIS
// AND MAKE ACCESS SHIT TONS FASTER
// (creation apparently takes roughly 2 seconds for 10,000 tris)

namespace TerrainFollow
{
	// ACTUAL TERRAIN FOLLOWING ALGORITHM
	//void TerrainFollow::TrueFollow(GroundFollowComponent* my, float dt)
	//{
	//	// P1    P2
	//	//    P3
	//	XMFLOAT3 P1, P2, P3,
	//		E1, E2, E3,
	//		O1, O2, O3,
	//		finalX, finalY, finalZ;
	//
	//	Transform& myTrans = my->GetOwner()->GetTransform();
	//
	//	XMVECTOR pos = XMLoadFloat3(myTrans.GetPosition()),
	//		xAxis = XMLoadFloat3(myTrans.GetXAxis()),
	//		yAxis = XMLoadFloat3(myTrans.GetYAxis()),
	//		zAxis = XMLoadFloat3(myTrans.GetZAxis()),
	//		fakeZ,
	//		Out1, Out2, Out3;
	//
	//	int dummyTriInd;
	//
	//	XMStoreFloat3(&P1, pos + (zAxis * 0.5f) - (xAxis * 0.5f));
	//	XMStoreFloat3(&E1, XMLoadFloat3(&P1) - (yAxis * 100));
	//
	//	XMStoreFloat3(&P2, pos + (zAxis * 0.5f) + (xAxis * 0.5f));
	//	XMStoreFloat3(&E2, XMLoadFloat3(&P2) - (yAxis * 100));
	//
	//	XMStoreFloat3(&P3, pos - (zAxis*0.5f));
	//	XMStoreFloat3(&E3, XMLoadFloat3(&P3) - (yAxis * 100));
	//
	//	if (CollisionManager::IntersectLine2Triangle(P1, E1, dummyTriInd, &O1) &&
	//		CollisionManager::IntersectLine2Triangle(P2, E2, dummyTriInd, &O2) &&
	//		CollisionManager::IntersectLine2Triangle(P3, E3, dummyTriInd, &O3))
	//	{
	//
	//		Out1 = (XMLoadFloat3(&O1) - XMLoadFloat3(&P1)) * dt + XMLoadFloat3(&P1);
	//		Out2 = (XMLoadFloat3(&O2) - XMLoadFloat3(&P2)) * dt + XMLoadFloat3(&P2);
	//		Out3 = (XMLoadFloat3(&O3) - XMLoadFloat3(&P3)) * dt + XMLoadFloat3(&P3);
	//
	//
	//		xAxis = XMVector3Normalize(Out2 - Out1);
	//		fakeZ = XMVector3Normalize(Out2 - Out3);
	//
	//		XMStoreFloat3(&finalX, xAxis);
	//		XMStoreFloat3(&finalY, XMVector3Cross(fakeZ, xAxis));
	//		XMStoreFloat3(&finalZ, XMVector3Cross(xAxis, XMLoadFloat3(&finalY)));
	//
	//		myTrans.SetXAxis(finalX);
	//		myTrans.SetYAxis(finalY);
	//		myTrans.SetZAxis(finalZ);
	//	}
	//}
	//
	//void TerrainFollow::CenterPoint(GroundFollowComponent* my, float dummydt)
	//{
	//	//PROBLEM: This works for spheres. Cubes will act weird, though.
	//	//			If I had access to the triangle normals, I could do a quick line2tri and then align it with the hit tri's normal?
	//	Transform& myTrans = my->GetOwner()->GetTransform();
	//	const XMFLOAT3* myPos = myTrans.GetPosition();
	//	XMFLOAT3 adjpos,
	//		newX, newY, newZ;
	//
	//	//XMStoreFloat3(&adjpos, XMLoadFloat3(myPos) + (XMLoadFloat3(myTrans.GetYAxis()) * 10));
	//	//myTrans.SetPosition(adjpos);
	//
	//	XMStoreFloat3(&newY, XMVector3Normalize(XMLoadFloat3(myPos)));
	//
	//	XMStoreFloat3(&newX, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newY), XMLoadFloat3(myTrans.GetZAxis()))));
	//
	//	XMStoreFloat3(&newZ, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newX), XMLoadFloat3(&newY))));
	//
	//	myTrans.SetXAxis(newX);
	//	myTrans.SetYAxis(newY);
	//	myTrans.SetZAxis(newZ);
	//}
	//
	//// Faux terrain follow, specifically to make Traversing of shapes where one dimension is clearly larger than the others.
	//void TerrainFollow::CenterLine(GroundFollowComponent* my, float dummydt)
	//{
	//	//PROBLEM: This works for cylinders and capsules. Peanuts will act weird, though.
	//	//			If I had access to the triangle normals, I could do a quick line2tri and then align it with the hit tri's normal?
	//}
	//
	//// Faux terrain follow, specifically to traverse torii.
	//void TerrainFollow::TorusLine(GroundFollowComponent* my, float dummydt)
	//{
	//	// THIS ASSUMES THE TORUS IS ALIGNED ALONG THE UNIVERSAL X AND Y AXES AND IS AT ORIGIN
	//
	//	float outerwidth = 0, innerwidth = 0, lengthToMiddle; // TODO: INIT OUTER AND INNER WIDTHS
	//	Transform& myTrans = my->GetOwner()->GetTransform();
	//	XMFLOAT3 FollowPos,
	//		newX, newY, newZ;
	//	const XMFLOAT3* myPos = my->GetOwner()->GetTransform().GetPosition();
	//
	//
	//	lengthToMiddle = (outerwidth - innerwidth) * 0.5f + innerwidth;
	//
	//	// Inputs the X,Y pos of the player. Z = 0.
	//	// Then readjusts it to find the proper clamping point.
	//	XMStoreFloat3(&FollowPos, XMVector3Normalize(XMLoadFloat3(&XMFLOAT3(myPos->x, myPos->y, 0))) * lengthToMiddle);
	//
	//
	//	// ... AND THEN WE JUST CHEAP OUT AND DO THE POINT ALGO ROFL
	//	XMStoreFloat3(&newY, XMVector3Normalize(XMLoadFloat3(myPos) - XMLoadFloat3(myTrans.GetYAxis())));
	//
	//	XMStoreFloat3(&newX, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newY), XMLoadFloat3(myTrans.GetZAxis()))));
	//
	//	XMStoreFloat3(&newZ, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newX), XMLoadFloat3(&newY))));
	//
	//	myTrans.SetXAxis(newX);
	//	myTrans.SetYAxis(newY);
	//	myTrans.SetZAxis(newZ);
	//}


	void FollowSphere(Transform& toModify, float softness, float fineness, XMFLOAT3 centerPoint, float radius, float dt)
	{
		XMVECTOR currPoint = XMLoadFloat3(toModify.GetPosition());
		XMVECTOR centerP = XMLoadFloat3(&centerPoint);

		XMVECTOR toCurr = XMVector3Normalize(currPoint - centerP);

		float axisLengths[3] =
		{
			XMVector3Length(XMLoadFloat3(toModify.GetXAxis())).m128_f32[0],
			XMVector3Length(XMLoadFloat3(toModify.GetYAxis())).m128_f32[0],
			XMVector3Length(XMLoadFloat3(toModify.GetZAxis())).m128_f32[0]
		};

		float ang = XMVector3AngleBetweenNormals(XMLoadFloat3(toModify.GetYAxis()), toCurr).m128_f32[0];
		if (ang > fineness)
		{
			XMFLOAT3 xAxis, yAxis, zAxis;

			XMStoreFloat3(&yAxis, toCurr * axisLengths[1]);
			XMStoreFloat3(&xAxis, XMVector3Normalize(XMVector3Cross(toCurr, XMLoadFloat3(toModify.GetZAxis()))) * axisLengths[0]);
			XMStoreFloat3(&zAxis, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xAxis), toCurr)) * axisLengths[2]);

			toModify.SetZAxis(zAxis);
			toModify.SetYAxis(yAxis);
			toModify.SetXAxis(xAxis);
		}

		XMFLOAT3 newPos;
		if (softness)
			XMStoreFloat3(&newPos, XMVectorLerp(XMLoadFloat3(toModify.GetPosition()), centerP + (toCurr * radius), softness * dt));
		else
			XMStoreFloat3(&newPos, centerP + (toCurr * radius));
		toModify.SetPosition(newPos);
	}

	void FollowRoundedCuboid(Transform& toModify, float softness, float fineness, XMFLOAT3 centerPoint, XMFLOAT3 cubeRadius, float cornerRadius, float dt)
	{
		XMFLOAT3 secondaryCenter = *toModify.GetPosition();
		secondaryCenter.x = fmaxf(-cubeRadius.x, fminf(cubeRadius.x, secondaryCenter.x - centerPoint.x));
		secondaryCenter.y = fmaxf(-cubeRadius.y, fminf(cubeRadius.y, secondaryCenter.y - centerPoint.y));
		secondaryCenter.z = fmaxf(-cubeRadius.z, fminf(cubeRadius.z, secondaryCenter.z - centerPoint.z));

		FollowSphere(toModify, softness, fineness, secondaryCenter, cornerRadius, dt);
	}

	void FollowTorus(Transform& toModify, float softness, float fineness, XMFLOAT3 centerPoint, XMFLOAT3 AxisMask, float radius, float tubeRadius, float dt)
	{
		XMFLOAT3 secondaryCenter;

		XMVECTOR pointWithinTorus = XMLoadFloat3(toModify.GetPosition()) - XMLoadFloat3(&centerPoint);
		pointWithinTorus *= XMLoadFloat3(&AxisMask);
		pointWithinTorus = XMVector3Normalize(pointWithinTorus) * radius;

		XMStoreFloat3(&secondaryCenter, pointWithinTorus);

		FollowSphere(toModify, softness, fineness, secondaryCenter, tubeRadius, dt);
	}

#pragma endregion

}


GroundFollowComponent::GroundFollowComponent()
{

}

GroundFollowComponent::~GroundFollowComponent()
{

}

void GroundFollowComponent::Update(float dt)
{

	if (m_pOwner->GetType() != eOBJECT_TYPE::World_Obj)
	{
		if (gameData != nullptr)
		{
			std::string currGeom = gameData->m_levelShape;

			if (currGeom == "Sphere")
				TerrainFollow::FollowSphere(GetOwner()->GetTransform(), GetSoftness(), GrabFineness(), XMFLOAT3(0, 0, 0), 70 + GetGroundClampOffset(), dt);
			else if (currGeom == "Torus")
				TerrainFollow::FollowTorus(GetOwner()->GetTransform(), GetSoftness(), GrabFineness(), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 1), 112.5f, 45 + GetGroundClampOffset(), dt);
			else if (currGeom == "RoundedCube")
				TerrainFollow::FollowRoundedCuboid(GetOwner()->GetTransform(), GetSoftness(), GrabFineness(), XMFLOAT3(0, 0, 0), XMFLOAT3(46.6667f, 46.6667f, 46.6667f), 23.4518f + GetGroundClampOffset(), dt);
		}
	}
}
