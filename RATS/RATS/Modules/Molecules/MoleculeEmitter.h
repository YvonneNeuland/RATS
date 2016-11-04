#pragma once

#include "../Renderer/RenderComp.h"
#include "../../Wwise files/Transform.h"

class Molecule;
class GameObject;

typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;

// Organizational Structure
struct EmitterData
{
	unsigned short index;
	unsigned short size;
	float timer = 1.0f;

	// Pointer to array partition of molecules
	Molecule *mArray;
};

// Negative XYZ:			These flags are for randomizing the ranges of random direction to include negative values equal to their value (15 is -15 to 15)

// AllRandom				Turns on all Negative axes flags

// AddParentVelocity		Should probably have a better name, existing molecules move with with their parent object

// WorldSpace				Determines if position and direction of moelcules use worldspace or local space (relative to position or object they are attached to)

// ShapeEmitter				Allows the use of the Offset value to shape the emitter into a 3D volume.  Molecules will spawn inside this volume
		//					Currently uses Negative Axes flags to determine range of offset spawning, same as direction values
		//					For a "flat" shape, sugh as a shadow or a wall, set the appropriate axis offset to 0.  5,0,5 would create a flat circle, for example.

// SquareEmission			Randomization of the offset is not normalized, so they will spawn in angular shapes

// ShellEmitter				Only spawns particles on the "surface" of the volume.  Setting this without ShapeEmitter will get you no where.  The "shape" is a point.

// NoNormalize				Does not normalize rand velocity values

// MoveToPosition			All molecules move to designated position (m_targetPosition), in local space to the emitter.  So 0,0,0 moves to the emitter
		//					m_targetPosition defaults to emitter position

// RandomColor				If implimented, will randomize color

enum EmitterFlags { EmitterOff = 1, NegativeX = 2, NegativeY = 4, NegativeZ = 8, AllRandom = 14,
					AddParentVelocity = 16, WorldSpace = 32, 
					ShapeEmitter = 64, SquareEmission = 128, ShellEmitter = 256, NoNormalize = 512,
					MoveToPosition = 1024, RandomColor = 2048, MoveAwayFromPosition = 4096
};




class MoleculeEmitter
{
public:
	MoleculeEmitter();
	~MoleculeEmitter();

	bool Update(float dt);

	unsigned short num_live_particles = 0;

	MoleculeEmitter& operator=(const MoleculeEmitter &rhs);

	void SetFlags(unsigned int flags);
	void ClearFlags();
	bool GetFlags(unsigned int flags);
	void ToggleFlag(unsigned int flag);
	void KillEmitter();

//private:
	// Either the emitter is attached to an object
	GameObject *m_attach;
	float3 m_position;

	// Color
	float4 m_endColor;
	float4 m_startColor;

	// Scale
	float m_startScale;
	float m_endScale;

	// Rotation over lifetime
	float m_degrees_rotation;

	// Lifetime
	float m_maxLife;
	float m_minLife;

	// Offsets
	float3 m_offset;

	//Direction
	float3 m_constDirection;
	float3 m_randDirection;
	float3 m_targetPosition;

	//Speed
	float m_startSpeed;
	float m_endSpeed;

	// Spawning Data
	
	// Number to spawn, and how often, and total number to spawn
	unsigned short m_spawnCount = 0;
	unsigned int m_spawnNum;		// Currently unused, was designed to be a total spawn count as a way to terminate emitter
	float m_spawnTimer;
	float m_spawnTime;

	unsigned int m_flags;
	bool loop = true;

	// Render Component for instanced data
	RenderComp m_render;
	
	// Holds data related to what the Manager needs to know
	EmitterData m_data;

	// Matrix for setting/rendering particles in the world; LocalMatrix is for scaling things to local space
	Transform m_Matrix, m_LocalMatrix;

	// Functiony things
	void SetMolecule(Molecule *mole);
	void Reset();
	void InitEmitter();

private:

	// Internal use functions
	void SetVelocity(Molecule *mol, float percent_life, float dt);
	void SpawnMolecule();
	void AddRandomDirection(float3 &vel);
	float3 SetSpawnPosition(const float3 &pos, const Transform &mat);
	float3 SetSpawnVelocity(const Transform &mat, float3 pos = float3(0,0,0));
	float3 ScaleVectorToLocal(const float3 &vel, const Transform &mat);
	void AdjustOffset(float3 &off);
	void SetPosition();

	DirectX::XMFLOAT4X4 TransformPointToMatrix(float3 pos);

};

