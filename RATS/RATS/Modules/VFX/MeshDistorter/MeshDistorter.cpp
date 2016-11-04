#include "stdafx.h"
#include "MeshDistorter.h"

#include "../../Renderer/D3DGraphics.h"
#include "../../Upgrade System/GameData.h"

#include "../../Object Manager/ObjectManager.h"
extern D3DGraphics* globalGraphicsPointer;
extern GameData* gameData;

#define SAFEDELARR(ptr) if(ptr != nullptr) delete[] ptr; ptr = nullptr

MeshDistorter::MeshDistorter()
{

}

MeshDistorter::~MeshDistorter()
{
	Shutdown();
}

void MeshDistorter::Shutdown()
{
	SAFEDELARR(meshPreserveOriginal);
}

void MeshDistorter::Reset(Mesh* newToDeform)
{
	if (newToDeform != nullptr)
	{
		wave.Deactivate(this);

		XMFLOAT4X4 identity;
		XMStoreFloat4x4(&identity, XMMatrixIdentity());
		GetTransform().SetLocalMatrix(identity);

		SAFEDELARR(meshPreserveOriginal);

		numMeshPreserveVerts = newToDeform->m_numVertices;
		meshPreserveOriginal = new POSUVNRM_VERTEX[newToDeform->m_numVertices];

		for (unsigned int currVert = 0; currVert < newToDeform->m_numVertices; ++currVert)
			meshPreserveOriginal[currVert] = newToDeform->m_vertexArray[currVert];

		wave.SetActive(newToDeform);
	}
}

void MeshDistorter::Wave::Deactivate(MeshDistorter* my)
{
	if (myMesh != nullptr)
	{
		WaveTemplate& tmpWave = my->waveTemplate;
		for (unsigned int currRing = 0; currRing < tmpWave.GetNumRings(); ++currRing)
		{
			tmpWave.GetRing(currRing).Reset(myMesh, my->meshPreserveOriginal);
		}

		SaveToMesh();

		globalGraphicsPointer->ReturnDistortionMesh(myMesh, gameData->m_levelShape);
		myMesh = nullptr;
	}

	currFront = 0;

	currLifetime = -1;

	active = false;
}

void MeshDistorter::SetEpicenter(Position pos)
{
	Transform& myTrans = GetTransform();

	XMFLOAT3 newX, newY, newZ;

	string& levelShape = gameData->m_levelShape;
	if (levelShape == "Torus")
	{
		// changes must be clamped to X-Z plane
		myTrans.SetScaleAll(1);
	}
	else if (levelShape == "RoundedCube")
	{
		myTrans.SetScaleAll(0.99f);
	}
	else // Sphere
	{
		XMStoreFloat3(&newY, XMVector3Normalize(XMLoadFloat3(&pos)));

		XMStoreFloat3(&newX, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newY), XMLoadFloat3(myTrans.GetZAxis()))));

		if (XMVector3Length(XMLoadFloat3(&newX)).m128_f32[0] < 1)
			XMStoreFloat3(&newX, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newY), XMLoadFloat3(myTrans.GetXAxis()))));

		XMStoreFloat3(&newZ, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newX), XMLoadFloat3(&newY))));

		myTrans.SetXAxis(newX);
		myTrans.SetYAxis(newY);
		myTrans.SetZAxis(newZ);

		myTrans.SetScaleAll(0.99f);
	}

	CreateTemplates(pos);

}

void MeshDistorter::Wave::SetActive(Mesh* newMesh)
{
	myMesh = newMesh;
	currLifetime = 0;
	currFront = 0;
	active = true;
}

void SortVertInds(void* Arr, int leftbound, int rightbound)
{
	struct VertInd
	{
		int index;
		float epiAxisDistance;
	};

	VertInd* actualArr = (VertInd*)Arr;

	int leftIter = leftbound, rightIter = rightbound;
	float pivotVal = actualArr[(leftIter + rightIter) / 2].epiAxisDistance;

	VertInd tmp;
	while (leftIter <= rightIter)
	{
		while (actualArr[leftIter].epiAxisDistance < pivotVal)
			++leftIter;

		while (actualArr[rightIter].epiAxisDistance > pivotVal)
			--rightIter;

		if (leftIter <= rightIter)
		{
			tmp = actualArr[leftIter];
			actualArr[leftIter] = actualArr[rightIter];
			actualArr[rightIter] = tmp;
			++leftIter;
			--rightIter;
		}
	}

	if (leftbound < rightIter)
		SortVertInds(Arr, leftbound, rightIter);
	if (leftIter < rightbound)
		SortVertInds(Arr, leftIter, rightbound);

}

void MeshDistorter::CreateTemplates(Position epicenter)
{
	struct VertInd
	{
		int index;
		float epiAxisDistance;
	};

	XMStoreFloat3(&epicenter, XMVector3Transform(XMLoadFloat3(&epicenter), XMMatrixInverse(nullptr, XMLoadFloat4x4(&GetTransform().GetWorldMatrix()))));
	XMVECTOR epicenterAxis = XMVector3Normalize(XMLoadFloat3(GetTransform().GetPosition()) - XMLoadFloat3(&epicenter));

	VertInd* toSort = new VertInd[numMeshPreserveVerts];

	bool isTorus = gameData->m_levelShape == "Torus";
	bool isRoundCube = gameData->m_levelShape == "RoundedCube";

	XMFLOAT3* currPos;

	//XMVECTOR toCurrPos;
	//float dotDist;
	for (unsigned int currVert = 0; currVert < numMeshPreserveVerts; ++currVert)
	{
		toSort[currVert].index = currVert;
		currPos = (XMFLOAT3*)&meshPreserveOriginal[currVert].position;
		
		if (isTorus)
			toSort[currVert].epiAxisDistance = XMVector3AngleBetweenVectors(XMLoadFloat3(currPos) * XMLoadFloat3(&XMFLOAT3(1, 0, 1)), XMLoadFloat3(&epicenter) * XMLoadFloat3(&XMFLOAT3(1, 0, 1))).m128_f32[0];
		//else if (isRoundCube)
		//{
		//	toCurrPos = XMVector3Normalize(XMLoadFloat3(currPos) - XMLoadFloat3(&epicenter));
		//	dotDist = XMVector3Dot(epicenterAxis, toCurrPos).m128_f32[0];
		//	toSort[currVert].epiAxisDistance = (dotDist * dotDist);
		//}
		else
			toSort[currVert].epiAxisDistance = XMVector3LengthSq(XMLoadFloat3(currPos) - XMLoadFloat3(&epicenter)).m128_f32[0];
	}

	SortVertInds(toSort, 0, int(numMeshPreserveVerts) - 1);

	for (unsigned int currVert = 0; currVert < numMeshPreserveVerts; ++currVert)
		toSort[currVert].epiAxisDistance = sqrtf(toSort[currVert].epiAxisDistance);

	float waveThickness = (isTorus ? torusWaveThickness : (isRoundCube ? roundedCubeWaveThickness : sphereWaveThickness));
	waveTemplate.Set(toSort, numMeshPreserveVerts, waveThickness);

	delete[] toSort;
}

void MeshDistorter::WaveTemplate::Set(void* Arr, unsigned int size, float waveThickness)
{
	struct VertInd
	{
		int index;
		float epiAxisDistance;
	};

	VertInd* actualArr = (VertInd*)Arr;

	float lastDistance = actualArr[0].epiAxisDistance;

	numRings = 0;

	unsigned int startIndex = 0, endIndex = 0;


	for (unsigned int currVert = 0; currVert < size; ++currVert)
	{
		if (actualArr[currVert].epiAxisDistance - lastDistance > waveThickness)
		{
			lastDistance = actualArr[currVert].epiAxisDistance;

			endIndex = currVert - 1;

			ringsUsed[numRings].Set(Arr, startIndex, endIndex);
			++numRings;

			startIndex = currVert;
		}
	}

	ringsUsed[numRings].Set(Arr, startIndex, size - 1);
	++numRings;

	if (numRings > MAXNUM_MESH_RINGS) DebugBreak();

	switchWavesRatio = (1.0f / numRings) * maxLifetime;
}

void MeshDistorter::WaveTemplate::RingTemplate::Set(void* Arr, unsigned int startInd, unsigned int endInd)
{
	struct VertInd
	{
		int index;
		float epiAxisDistance;
	};

	VertInd* actualArr = (VertInd*)Arr;

	unsigned int chunkLength = endInd - startInd;

	numInds = 0;

	for (unsigned int currInd = 0; currInd <= chunkLength; ++currInd)
	{
		indicesUsed[currInd] = actualArr[startInd + currInd].index;
		++numInds;
	}

	if (numInds > MAXNUM_INDICES_PER_RING) DebugBreak();
}


void MeshDistorter::Update(float dt)
{
	wave.Update(this, dt);
}

void MeshDistorter::Wave::Update(MeshDistorter* my, float dt)
{
	if (active)
	{
		WaveTemplate& tmpWave = my->waveTemplate;

		currLifetime += dt;

		if (currLifetime > tmpWave.GetSwitchWaveThreshold())
		{
			++currFront;

			if (currFront > tmpWave.GetNumRings() + 1)
			{
				Deactivate(my);

				my->SetDead();

				return;
			}

			currLifetime = 0;
		}

		if (int(currFront) - 2 >= 0)
			tmpWave.GetRing(currFront - 2).Reset(myMesh, my->meshPreserveOriginal);

		if (int(currFront) - 1 >= 0 && int(currFront) < tmpWave.GetNumRings())
			tmpWave.GetRing(currFront - 1).Lower(myMesh, my->meshPreserveOriginal, my->offsetAmount * dt);

		if (currFront < tmpWave.GetNumRings())
			tmpWave.GetRing(currFront).Raise(myMesh, my->meshPreserveOriginal, my->offsetAmount * dt, my->offsetAmount * 0.75f);

		SaveToMesh();
	}
}

void MeshDistorter::Wave::SaveToMesh()
{
	D3D11_MAPPED_SUBRESOURCE ms;
	globalGraphicsPointer->GetContext()->Map(myMesh->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, myMesh->m_vertexArray, sizeof(POSUVNRM_VERTEX) * myMesh->m_numVertices);
	globalGraphicsPointer->GetContext()->Unmap(myMesh->m_vertexBuffer, 0);
}

void MeshDistorter::WaveTemplate::RingTemplate::Raise(Mesh* toUse, POSUVNRM_VERTEX* origVerts, float offsAmt, float ceilingAmt)
{
	POSUVNRM_VERTEX* verts = toUse->m_vertexArray;

	XMFLOAT3* currentPosition;
	XMFLOAT3* currentNormal;

	for (unsigned int i = 0; i < numInds; ++i)
	{
		currentPosition = (XMFLOAT3*)&verts[indicesUsed[i]].position;
		currentNormal = &verts[indicesUsed[i]].normal;

		XMStoreFloat3(currentPosition, XMLoadFloat3(currentPosition) + (XMLoadFloat3(&origVerts[indicesUsed[i]].normal) * offsAmt));
		XMStoreFloat3(currentNormal, XMLoadFloat3(&origVerts[indicesUsed[i]].normal) * 2);

		if (XMVector3Length(XMLoadFloat3(currentPosition) - XMLoadFloat3((XMFLOAT3*)&origVerts[indicesUsed[i]].position)).m128_f32[0] > ceilingAmt)
			XMStoreFloat3(currentPosition, XMLoadFloat3((XMFLOAT3*)&origVerts[indicesUsed[i]].position) + (XMLoadFloat3(&origVerts[indicesUsed[i]].normal) * ceilingAmt));
	}
}

void MeshDistorter::WaveTemplate::RingTemplate::Lower(Mesh* toUse, POSUVNRM_VERTEX* origVerts, float offsAmt)
{
	POSUVNRM_VERTEX* verts = toUse->m_vertexArray;

	XMFLOAT3* currentPosition;
	XMFLOAT3* currentNormal;

	for (unsigned int i = 0; i < numInds; ++i)
	{
		currentPosition = (XMFLOAT3*)&verts[indicesUsed[i]].position;
		currentNormal = &verts[indicesUsed[i]].normal;

		XMStoreFloat3(currentPosition, XMLoadFloat3(currentPosition) - (XMLoadFloat3(&origVerts[indicesUsed[i]].normal) * offsAmt));

		if (XMVector3Dot(XMLoadFloat3(currentPosition) - XMLoadFloat3((XMFLOAT3*)&origVerts[indicesUsed[i]].position), XMLoadFloat3(currentNormal)).m128_f32[0] < 0)
			*currentPosition = *(XMFLOAT3*)&origVerts[indicesUsed[i]].position;

		XMStoreFloat3(currentNormal, XMLoadFloat3(&origVerts[indicesUsed[i]].normal));
	}
}

void MeshDistorter::WaveTemplate::RingTemplate::Reset(Mesh* toUse, POSUVNRM_VERTEX* origVerts)
{
	POSUVNRM_VERTEX* verts = toUse->m_vertexArray;

	for (unsigned int i = 0; i < numInds; ++i)
		verts[indicesUsed[i]] = origVerts[indicesUsed[i]];
}
