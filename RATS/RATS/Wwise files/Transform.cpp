#include "stdafx.h"
#include "Transform.h"

#include <cstring>
//--------------------------------------------------------------------------------
using namespace DirectX;
//--------------------------------------------------------------------------------
Transform::Transform(void) : parent(0), child(0), sibling(0), dirtied(true) {

	memset(&m_fRotVals[0], 0, sizeof(float) * 3);

	XMStoreFloat4x4(&localMatrix, XMMatrixIdentity() );
}
//--------------------------------------------------------------------------------
Transform::Transform(const Transform& rhs) : child(nullptr), sibling(nullptr), parent(nullptr), dirtied(false), modified(false) {
	localMatrix = rhs.localMatrix;
	worldMatrix = rhs.worldMatrix;
	memcpy(&m_fRotVals[0], &rhs.m_fRotVals[0], sizeof(float) * 3);
}
//--------------------------------------------------------------------------------
void Transform::OnDestroy(void) {
	for (Transform* current = child; current != 0; current = current->sibling) {
		//remove this object
	}
}
//--------------------------------------------------------------------------------
const DirectX::XMFLOAT4X4& Transform::GetWorldMatrix(void) {
	using namespace DirectX;
	if (dirtied) {
		if (nullptr != parent) {
			XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMLoadFloat4x4(&parent->GetWorldMatrix() ) ) );
		}
		else
			worldMatrix = localMatrix;

		dirtied = false;
	}

	return worldMatrix;
}
//--------------------------------------------------------------------------------
const DirectX::XMFLOAT4X4& Transform::GetLocalMatrix(void) const {
	return localMatrix;
}
//--------------------------------------------------------------------------------
void Transform::SetLocalMatrix(const DirectX::XMFLOAT4X4& matrix) {

	memcpy(&localMatrix, &matrix, sizeof(XMFLOAT4X4));

	//localMatrix = matrix;

	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::SetLocals(const DirectX::XMFLOAT4X4& matrix, const float* rotArr)
{
	SetLocalMatrix(matrix);
	memcpy(&m_fRotVals[0], &rotArr[0], sizeof(float) * 3);
}
//--------------------------------------------------------------------------------
void Transform::DirtyTransform(void) {
	modified = true;

	if (!dirtied) {
		dirtied = true;

		Transform* current = child;

		while (nullptr != current) {
			current->DirtyTransform();
			current = current->sibling;
		}
	}
}
//--------------------------------------------------------------------------------
Transform* Transform::GetParent(void) {
	return parent;
}
//--------------------------------------------------------------------------------
const Transform* Transform::GetParent(void)const {
	return parent;
}
//--------------------------------------------------------------------------------
void Transform::SetParent(Transform* newParent) {
	if (newParent == this)
		return;

	if (parent == newParent)
		return;

	if (nullptr != parent)
		parent->RemoveChild(this);

	parent = newParent;

	if (nullptr != newParent) {
		using namespace DirectX;
		
		XMFLOAT4X4 newLocal;
		//world = local * worldParent is how we normally get our world
		//Adjust the local to keep the transform in the same world space position
		XMStoreFloat4x4(&newLocal, XMMatrixMultiply(XMLoadFloat4x4(&GetWorldMatrix()), XMMatrixInverse(nullptr, XMLoadFloat4x4(&parent->GetWorldMatrix() ) ) ) );
		SetLocalMatrix(newLocal);
		parent->SetChild(this);
	}
}
//--------------------------------------------------------------------------------
void Transform::SetChild(Transform* _child) {
	if (nullptr == child) {
		child = _child;
		return;
	}

	child->SetSibling(_child);
}
//--------------------------------------------------------------------------------
void Transform::SetSibling(Transform* _sibling) {
	if (nullptr == sibling) {
		sibling = _sibling;
		return;
	}
	sibling->SetSibling(_sibling);
}
//--------------------------------------------------------------------------------
void Transform::RemoveChild(Transform* _child) {
	if (nullptr == child || child->parent != this)
		return;

	if (child == _child) {
		child->SetLocalMatrix(child->GetWorldMatrix() );
		child = child->sibling;
		child->parent = nullptr;
		child->sibling = nullptr;
		return;
	}
	child->RemoveSibling(_child);
}
//--------------------------------------------------------------------------------
void Transform::RemoveSibling(Transform* _sibling) {
	if (nullptr == sibling)
		return;

	if (sibling == _sibling) {
		sibling->SetLocalMatrix(sibling->GetWorldMatrix() );
		sibling = sibling->sibling;
		sibling->parent = nullptr;
		sibling->sibling = nullptr;
		return;
	}
	sibling->RemoveSibling(_sibling);
}
//--------------------------------------------------------------------------------
void Transform::SetPosition(const XMFLOAT3 pos) {
	localMatrix._41 = pos.x;
	localMatrix._42 = pos.y;
	localMatrix._43 = pos.z;
	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::SetXAxis(const XMFLOAT3 xAxis) {
	localMatrix._11 = xAxis.x;
	localMatrix._12 = xAxis.y;
	localMatrix._13 = xAxis.z;
	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::SetYAxis(const XMFLOAT3 yAxis) {
	localMatrix._21 = yAxis.x;
	localMatrix._22 = yAxis.y;
	localMatrix._23 = yAxis.z;
	DirtyTransform();
}
//--------------------------------------------------------------------------------
void Transform::SetZAxis(const XMFLOAT3 zAxis) {
	localMatrix._31 = zAxis.x;
	localMatrix._32 = zAxis.y;
	localMatrix._33 = zAxis.z;
	DirtyTransform();
}

void Transform::SetScale(int axis, float newSize)
{
	XMFLOAT3 axisv;
	if (!axis)
	{
		XMStoreFloat3(&axisv, XMVector3Normalize(XMLoadFloat3(GetXAxis())) * newSize);
		SetXAxis(axisv);
	}
	else if (axis == 1)
	{
		XMStoreFloat3(&axisv, XMVector3Normalize(XMLoadFloat3(GetYAxis())) * newSize);
		SetYAxis(axisv);
	}
	else
	{
		XMStoreFloat3(&axisv, XMVector3Normalize(XMLoadFloat3(GetZAxis())) * newSize);
		SetZAxis(axisv);
	}

}

void Transform::SetScaleAll(float newSize)
{
	SetScale(0, newSize);
	SetScale(1, newSize);
	SetScale(2, newSize);
}

void Transform::SetScaleAll(float newX, float newY, float newZ)
{
	SetScale(0, newX);
	SetScale(1, newY);
	SetScale(2, newZ);
}

void Transform::RotateBy(int _whichAxis, float _amt, float dt, bool _isDegrees /*= false*/)
{

	float turnAmt;
	if (_isDegrees)
		turnAmt = DirectX::XMConvertToRadians(_amt);
	else
		turnAmt = _amt;
	turnAmt *= dt;
	m_fRotVals[_whichAxis] += turnAmt;

	DirectX::XMMATRIX newRotMat;
	DirectX::XMFLOAT4X4 tmp;
	switch (_whichAxis)
	{
	case 0:
		newRotMat = DirectX::XMMatrixRotationX(turnAmt);
		break;
	case 1:
		newRotMat = DirectX::XMMatrixRotationY(turnAmt);
		break;
	case 2:
		newRotMat = DirectX::XMMatrixRotationZ(turnAmt);
		break;
	default:
		std::cout << "ERROR: wrong axis passed into RotateBy();\n";
		break;
	}

	DirectX::XMStoreFloat4x4(&tmp, DirectX::XMMatrixMultiply(newRotMat, XMLoadFloat4x4(&GetLocalMatrix())));
	SetLocalMatrix(tmp);
	DirtyTransform();
}

void Transform::ResetTransform()
{
	parent = nullptr;
	child = nullptr;
	sibling = nullptr;

	memset(&worldMatrix, 0, sizeof(XMFLOAT4X4));
	memset(&localMatrix, 0, sizeof(XMFLOAT4X4));


	worldMatrix._11 = 1;
	worldMatrix._22 = 1;
	worldMatrix._33 = 1;
	worldMatrix._44 = 1;

	localMatrix._11 = 1;
	localMatrix._22 = 1;
	localMatrix._33 = 1;
	localMatrix._44 = 1;

	//worldMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	//localMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	dirtied = 1;
	modified = 1;

}