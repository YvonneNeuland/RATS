#include "stdafx.h"
#include "CameraObject.h"

#include <Windows.h>

using namespace DirectX;

CameraObject::~CameraObject()
{

}

void CameraObject::Update(float dt)
{
	if (target)
	{
		Move(fminf(1, dt * moveSnapSpeed));
		TurnToTarget(fminf(1, dt * rotSnapSpeed));
	}
}

void CameraObject::Move(float ratio)
{
	if (target->GetType() == eOBJECT_TYPE::Player_Obj)
	{
		GetTransform().SetLocals(target->GetTransform().GetLocalMatrix(), target->GetTransform().GetRotations());

		GetTransform().RotateBy(1, -GetTransform().GetRotation(1));

		XMVECTOR newOne = XMLoadFloat3(GetTransform().GetPosition());
		newOne += XMLoadFloat3(GetTransform().GetXAxis()) * offset.x * sinf(GetTickCount64() * 0.001f);
		newOne += XMLoadFloat3(GetTransform().GetYAxis()) * offset.y;
		newOne += XMLoadFloat3(GetTransform().GetZAxis()) * offset.z;

		XMFLOAT3 newPos;
		XMStoreFloat3(&newPos, newOne);

		GetTransform().SetPosition(newPos);
	}
	else
	{
		XMVECTOR toPos = XMLoadFloat3(target->GetTransform().GetPosition());
		if (offsetInWorldSpace)
		{
			toPos += XMLoadFloat3(&offset);
		}
		else
		{
			toPos += XMLoadFloat3(target->GetTransform().GetXAxis()) * offset.x;
			toPos += XMLoadFloat3(target->GetTransform().GetYAxis()) * offset.y;
			toPos += XMLoadFloat3(target->GetTransform().GetZAxis()) * offset.z;
		}


		XMFLOAT3 newPos;
		XMStoreFloat3(&newPos, XMVectorLerp(XMLoadFloat3(GetTransform().GetPosition()), toPos, ratio));

		GetTransform().SetPosition(newPos);
	}

}

void CameraObject::TurnToTarget(float ratio)
{
	//XMVECTOR toTarg = XMLoadFloat3(target->GetTransform().GetPosition()) - XMLoadFloat3(GetTransform().GetPosition());

	//XMVECTOR newZ = XMVector3Normalize(XMVectorLerp(XMLoadFloat3(GetTransform().GetZAxis()), toTarg, ratio));

	//XMFLOAT3 x, y, z;

	//XMStoreFloat3(&x,XMVector3Cross(XMLoadFloat3(target->GetTransform().GetYAxis()), newZ));
	//XMStoreFloat3(&y,XMVector3Cross(newZ, XMLoadFloat3(&x)));
	//XMStoreFloat3(&z, newZ);

	//GetTransform().SetXAxis(x);
	//GetTransform().SetYAxis(y);
	//GetTransform().SetZAxis(z);


	XMVECTOR toTarg = XMLoadFloat3(target->GetTransform().GetPosition()) - XMLoadFloat3(GetTransform().GetPosition());

	XMVECTOR newZ = XMVector3Normalize(XMVectorLerp(XMLoadFloat3(GetTransform().GetZAxis()), toTarg, ratio));

	XMFLOAT3 x, y, z;

	XMStoreFloat3(&x,
		XMVector3Cross(XMLoadFloat3((target->GetType() == eOBJECT_TYPE::Player_Obj ? GetTransform().GetZAxis():GetTransform().GetYAxis())), newZ));
	XMStoreFloat3(&y,XMVector3Cross(newZ, XMLoadFloat3(&x)));
	XMStoreFloat3(&z, newZ);

	GetTransform().SetXAxis(x);
	GetTransform().SetYAxis(y);
	GetTransform().SetZAxis(z);
}
