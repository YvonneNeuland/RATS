#pragma once

#include "../VisualEffect/VisualEffect.h"

#define MAXNUM_MESH_RINGS 800
#define MAXNUM_INDICES_PER_RING 6000

struct Mesh;
struct POSUVNRM_VERTEX;

class ObjectManager;

void SortVertInds(void* Arr, int leftbound, int rightbound);

class MeshDistorter : public VisualEffect
{
public:

	MeshDistorter();
	~MeshDistorter();

	void Update(float dt);

	void Shutdown();

	void Reset(Mesh* newToDeform);

	void SetOwner(ObjectManager* newOwner) { owner = newOwner; }

	void SetEpicenter(Position pos);

protected:

	virtual void CreateTemplates(Position Epicenter);

	MeshDistorter(const MeshDistorter&) = delete;
	MeshDistorter(const MeshDistorter&&) = delete;
	MeshDistorter& operator=(const MeshDistorter&) = delete;
	MeshDistorter& operator=(const MeshDistorter&&) = delete;

	struct WaveTemplate
	{
	public:
		void Set(void* Arr, unsigned int size, float waveThickness);

		float GetSwitchWaveThreshold() { return switchWavesRatio; }
		const float GetMaxLife() const { return maxLifetime; }

		struct RingTemplate
		{
		public:
			void Set(void* Arr, unsigned int startInd, unsigned int endInd);

			void Raise(Mesh* toUse, POSUVNRM_VERTEX* origVerts, float offsAmt, float ceilingAmt);
			void Lower(Mesh* toUse, POSUVNRM_VERTEX* origVerts, float offsAmt);
			void Reset(Mesh* toUse, POSUVNRM_VERTEX* origVerts);
		private:
			unsigned int GetNumInds() { return numInds; }

			unsigned int indicesUsed[MAXNUM_INDICES_PER_RING];
			unsigned int numInds = 0;
		};

		RingTemplate& GetRing(unsigned int index) { return ringsUsed[index]; }

		unsigned int GetNumRings() { return numRings; }

	private:
		RingTemplate ringsUsed[MAXNUM_MESH_RINGS];
		unsigned int numRings = 0;

		float switchWavesRatio = 0;
		const float maxLifetime = 2.0f; //6.0f; //30.0f;
	};

	WaveTemplate waveTemplate;

	
	struct Wave
	{
	public:
		void Update(MeshDistorter* my, float dt);

		float GetLifetime() { return currLifetime; }

		void SetActive(Mesh* newMesh);

		void Deactivate(MeshDistorter* my);

	private:

		void SaveToMesh();

		unsigned int currFront = 0;

		Mesh* myMesh = nullptr;

		float currLifetime = -1;

		bool active = false;
	};

	Wave wave;

	ObjectManager* owner = nullptr;

	// how much tolerance is allowed between verts to be included in a single wave?
	const float sphereWaveThickness = 0.5f;
	const float torusWaveThickness = 0.03f;
	const float roundedCubeWaveThickness = 2.5f;

	//Mesh* worldMesh;
	POSUVNRM_VERTEX* meshPreserveOriginal = nullptr;
	unsigned int numMeshPreserveVerts;

	const float offsetAmount = 60.0f;

private:

};
