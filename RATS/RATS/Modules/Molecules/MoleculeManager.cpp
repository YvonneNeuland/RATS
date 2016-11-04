#include "stdafx.h"
#include "MoleculeManager.h"
#include "MoleculeEmitter.h"

// Temp Include
#include "Molecule.h"

// Debug Include
#include <fstream>



MoleculeManager::MoleculeManager()
{
	// Allocate bank of molecules
	m_pMoleculeBank = new Molecule[NUM_MOLECULES];

	// Allocate Emitter List
	m_plEmitters.resize(NUM_EMITTERS);
	auto it = m_plEmitters.begin();
	while (it != m_plEmitters.end())
	{
		(*it) = new MoleculeEmitter();
		it++;
	}

	LoadEmitter("Fake String");
}


MoleculeManager::~MoleculeManager()
{
	delete[] m_pMoleculeBank;

	auto it = m_plEmitters.begin();
	while (it != m_plEmitters.end())
	{
		delete (*it);
		it++;
	}

	it = m_plLiveEmitters.begin();
	while (it != m_plLiveEmitters.end())
	{
		delete (*it);
		it++;
	}
}

void MoleculeManager::Update(float dt)
{
	auto iter = m_plLiveEmitters.begin();
	while (iter != m_plLiveEmitters.end())
	{
		if(false)
		{
			// Call Debug Function
			DebugMe();
		}


		if ((*iter)->Update(dt))
		{
			auto remove = iter;
			iter++;
			ReclaimMolecules(*remove);
			(*remove)->Reset();
			m_plEmitters.splice(m_plEmitters.begin(), m_plLiveEmitters, remove);
		}
		else
			iter++;
	}
}

MoleculeEmitter* MoleculeManager::BuildEmitter(GameObject *target, unsigned short size, std::string type)
{
	// We ran out of Emitters :(
	if (m_plEmitters.size() == 0)
	{
		std::cout<< "No Emitters avail\n";
		return nullptr;
	}
	// Otherwise if we are able to setup an appropriately sized emitter
	MoleculeEmitter *setup = *m_plEmitters.begin();
	if (SetupEmitter(size, setup, type))
	{
		// Splice the emitter into livelist
		setup->m_attach = target;
		setup->InitEmitter();
		m_plLiveEmitters.splice(m_plLiveEmitters.begin(), m_plEmitters, m_plEmitters.begin());
		return setup;
	}

	// Only hits this return if SetupEmitter fails, meaning we haven't enough space in our particles to assign it
	std::cout<< "SetupEmitter() failed (BuildEmitter) \n";
	setup->Reset();
	return nullptr;
}

MoleculeEmitter* MoleculeManager::BuildPosEmitter(DirectX::XMFLOAT3 pos, unsigned short size, std::string type)
{
	// We ran out of Emitters :(
	if (m_plEmitters.size() == 0)
	{
		std::cout<< "No Emitters avail\n";
		return nullptr;
	}

	// Otherwise if we are able to setup an appropriately sized emitter
	MoleculeEmitter *setup = *m_plEmitters.begin();
	if (SetupEmitter(size, setup, type))
	{
		// Splice the emitter into livelist
		setup->m_position = pos;
		setup->m_targetPosition = pos;
		setup->InitEmitter();
		m_plLiveEmitters.splice(m_plLiveEmitters.begin(), m_plEmitters, m_plEmitters.begin());
		return setup;
	}

	// Only hits this return if SetupEmitter fails, meaning we haven't enough space in our particles to assign it
	std::cout<< "SetupEmitter() failed (BuildPosEmitter) \n";
	return nullptr;
}

// Returns true if memory space was found
// Returns false if there was not enough room
bool MoleculeManager::SetupEmitter(unsigned short size, MoleculeEmitter* emit, std::string type)
{
	// First attempt to find an empty spot in the memory
	auto iter = m_plEmitterData.begin();
	unsigned short curIndex = 0;


	while (iter != m_plEmitterData.end())
	{
		// If these are equal, we are running into consecutive memory allocations
		// If not equal, we have found an index with a free block of memory and need to check if it's big enough to use
		if (curIndex != (*iter)->index)
		{

			// There is enough size between allocations
			if (((*iter)->index - curIndex) >= size)
			{
				EmitterData* toAdd;
				toAdd = &emit->m_data;
				toAdd->index = curIndex;
				toAdd->mArray = &m_pMoleculeBank[curIndex];
				m_plEmitterData.emplace(iter, toAdd);
				*emit = m_mEmitterTypes[type.c_str()];
				toAdd->size = size;

				return true;
			}
			// else  there is not enough to allocate, keep looking
		}
		curIndex = (*iter)->index + (*iter)->size;
		iter++;
	}


	// We are at the end of the allocations, check to see if the remaining memory is big enough
	// Assign to end of current allocations!
	if (NUM_MOLECULES - curIndex >= size)
	{
		// Add this to the current list of data
		EmitterData* toAdd;
		toAdd = &emit->m_data;
		toAdd->index = curIndex;
		toAdd->mArray = &m_pMoleculeBank[curIndex];
		m_plEmitterData.emplace(iter, toAdd);
		*emit = m_mEmitterTypes[type.c_str()];
		toAdd->size = size;

		return true;
	}


	return false;
}

void MoleculeManager::ReclaimMolecules(MoleculeEmitter *emit)
{
	// Reset Memory, if necessary.  Useful for debugging, finding particles that have 0 values when they shouldn't
	//ZeroMemory( (void*)&m_pMoleculeBank[emit->m_data.index], emit->m_data.size * sizeof(Molecule) );

	// Don't apparently need to find the iterator for removal, just takes the pointer value
	m_plEmitterData.remove(&emit->m_data);

}

// This function actually may not be necessary, as noted inside ReclaimMolecules
std::list<EmitterData*>::iterator MoleculeManager::FindIter(MoleculeEmitter *emit)
{
	auto it = m_plEmitterData.begin();
	while (it != m_plEmitterData.end())
	{
		if ((*it) == &emit->m_data)
			return it;
		it++;
	}

	// Should never reach here if code is working properly
	// But if it does, we return the end of the list
	return it;
}

// Will load from file when that is ready
bool MoleculeManager::LoadEmitter(std::string file)
{
	// Faking Emitter Loads

	// TEST 1
	MoleculeEmitter load;
	// Emission
	load.m_startColor = float4(1, 0, 0, 1);
	load.m_endColor = float4(0.5f, 0.5f, 0, 0.7f);
	load.m_startScale = 1.0f;
	load.m_endScale = 1.0f;
	load.m_maxLife = 1.5f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(5.0f, 5.0f, 0.0f);
	//load.m_endVel			= float3(0.5f, 0.5f, 0.0f);
	load.m_offset = float3(3.0f, 3.0f, 3.0f);
	load.m_spawnNum = 5000;
	load.m_spawnCount = 25;
	load.m_spawnTimer = 0.1f;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 1.0f;
	load.m_endSpeed = 1.0f;


	load.m_render.SetID("Basic Molecule");
	m_mEmitterTypes["Test1"] = load;
	//////////////////////////////////////////////////////////////////////////

	load.Reset();

	// Smoke Trail (Semi-test emitter)
	load.m_startColor = float4(0.85f, 0.35f, 0.35f, 1.0f);
	load.m_endColor = float4(0.85f, 0.65f, 0.65f, 1.0f);
	load.m_startScale = 7.0f;
	load.m_endScale = 7.0f;
	load.m_maxLife = 0.25f;
	load.m_minLife = 0.25f;
	load.m_constDirection = float3(0.0f, 0.0f, -1.5f);
	load.m_randDirection = float3(0.50f, 2.50f, -1.0f);
	load.m_offset = float3(0.0f, 0.0f, -5.0f);
	load.m_spawnNum = 500;
	load.m_spawnCount = 25;
	load.m_spawnTimer = 0.1f;
	load.m_spawnTime = 0.03f;
	load.m_startSpeed = 10.0f;
	load.m_endSpeed = 25.0f;
	load.m_flags = NegativeX | NegativeY | AddParentVelocity;
	load.m_data.timer = 30.0f;
	load.m_degrees_rotation = 1080.0f;
	load.loop = true;

	load.m_render.SetID("Spark Particle2");
	//m_mEmitterTypes["SmokeTrail"] = load;
	CreateEmitter("SmokeTrail", load);

	load.Reset();
	// Enemy Spwan Trail
	load.m_startColor = float4(0.85f, 0.85f, 0.85f, 0.85f);
	load.m_endColor = float4(0.6f, 0.4f, 0.1f, 0.6f);
	load.m_startScale = 0.2f;
	load.m_endScale = 0.2f;
	load.m_maxLife = 0.5f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, -10.5f);
	load.m_randDirection = float3(4.5f, 0.5f, 0.0f);
	load.m_offset = float3(0.0f, 0.0f, -4.0f);
	load.m_spawnNum = 200;
	load.m_spawnCount = 20;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.05f;
	load.m_startSpeed = 3.0f;
	load.m_endSpeed = 3.0f;
	load.m_data.timer = 0.5f;
	load.loop = true;
	load.m_flags = NegativeX | NegativeY;

	load.m_render.SetID("Basic Molecule");
	m_mEmitterTypes["SpawnTrail"] = load;
	//////////////////////////////////////////////////////////////////////////


	// Mine Trigger 
	load.m_startColor = float4(0.95f, 0.85f, 0.5f, 0.85f);
	load.m_endColor = float4(0.95f, 0.85f, 0.5f, 0.85f);
	load.m_startScale = 0.3f;
	load.m_endScale = 0.3f;
	load.m_maxLife = 0.3f;
	load.m_minLife = 0.4f;
	load.m_constDirection = float3(0, 7, 0);
	load.m_randDirection = float3(5, 5, 5);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 200;
	load.m_spawnCount = 50;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.35f;
	load.m_startSpeed = 5.0f;
	load.m_endSpeed = 6.0f;
	load.m_data.timer = 1;
	load.loop = false;
	load.m_flags = AllRandom;

	load.m_render.SetID("Basic Molecule");
	m_mEmitterTypes["MineTrigger"] = load;
	//////////////////////////////////////////////////////////////////////////

	// Inner Star 
	load.m_startColor = float4(1.2f, 0.9f, 0.3f, 0.5f);
	load.m_endColor = float4(1.2f, 0.5f, 0.2f, 1.0f);
	load.m_startScale = 45;
	load.m_endScale = 35;
	load.m_maxLife = 11;
	load.m_minLife = 10;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(5, 5, 5);
	load.m_offset = float3(25.0f, 25.0f, 25.0f);
	load.m_spawnNum = 500;
	load.m_spawnCount = 15;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.2f;
	load.m_startSpeed = 25;
	load.m_endSpeed = 10;
	load.m_degrees_rotation = 1420;
	load.m_data.timer = 5;
	load.loop = true;
	load.m_flags = AllRandom | ShapeEmitter | WorldSpace;

	load.m_render.SetID("Spark Particle4");
	m_mEmitterTypes["InnerStar"] = load;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Energy Pickup 
	load.m_startColor = float4(1, 1, 0.9f, 1);
	load.m_endColor = float4(1, 1, 0.5f, 1.0f);
	load.m_startScale = 5;
	load.m_endScale = 5;
	load.m_maxLife = 0.03f;
	load.m_minLife = 0.03f;
	load.m_constDirection = float3(0, 1, 0);
	load.m_randDirection = float3(0.1f, 0.1f, 0.1f);
	load.m_offset = float3(0, 0, 0);
	load.m_spawnNum = 90;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.01f;
	load.m_startSpeed = 0.0f;
	load.m_endSpeed = 0.0f;
	load.m_degrees_rotation = 0;
	load.m_data.timer = 5;
	load.loop = true;
	load.m_flags = NegativeX | NegativeZ;

	load.m_render.SetID("Energy Battery");
	m_mEmitterTypes["Energy Pickup"] = load;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Energy Pickup Part 2
	load.m_startColor = float4(1, 1, 0.9f, 1);
	load.m_endColor = float4(1, 1, 0.5f, 1.0f);
	load.m_startScale = 1.5f;
	load.m_endScale = 0;
	load.m_maxLife = 0.9f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(2.9f, 2.9f, 2.9f);
	load.m_offset = float3(0, 0, 0);
	load.m_spawnNum = 90;
	load.m_spawnCount = 10;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.25f;
	load.m_startSpeed = 5.0f;
	load.m_endSpeed = 3.0f;
	load.m_degrees_rotation = 0;
	load.m_data.timer = 5;
	load.loop = true;
	load.m_flags = AllRandom;

	load.m_render.SetID("Energy Battery");
	m_mEmitterTypes["Energy Pickup Aura"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Health Pickup 
	load.m_startColor = float4(0.9f, 1, 0.9f, 1);
	load.m_endColor = float4(0.8f, 1, 0.8f, 1.0f);
	load.m_startScale = 5;
	load.m_endScale = 5;
	load.m_maxLife = 0.03f;
	load.m_minLife = 0.03f;
	load.m_constDirection = float3(0, 1, 0);
	load.m_randDirection = float3(0.1f, 0.1f, 0.1f);
	load.m_offset = float3(0, 0, 0);
	load.m_spawnNum = 90;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.01f;
	load.m_startSpeed = 0.0f;
	load.m_endSpeed = 0.0f;
	load.m_degrees_rotation = 0;
	load.m_data.timer = 5;
	load.loop = true;
	load.m_flags = NegativeX | NegativeZ;

	load.m_render.SetID("Health Battery");
	m_mEmitterTypes["Health Pickup"] = load;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Health Pickup Part 2
	load.m_startColor = float4(0.9f, 1, 0.9f, 1);
	load.m_endColor = float4(0.8f, 1, 0.8f, 1.0f);
	load.m_startScale = 1.5f;
	load.m_endScale = 0;
	load.m_maxLife = 0.9f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(2.9f, 2.9f, 2.9f);
	load.m_offset = float3(0, 0, 0);
	load.m_spawnNum = 90;
	load.m_spawnCount = 10;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.25f;
	load.m_startSpeed = 5.0f;
	load.m_endSpeed = 3.0f;
	load.m_degrees_rotation = 0;
	load.m_data.timer = 5;
	load.loop = true;
	load.m_flags = AllRandom;

	load.m_render.SetID("Health Battery");
	m_mEmitterTypes["Health Pickup Aura"] = load;

	//////////////////////////////////////////////////////////////////////////

	// Whip Bullet Death
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 3.0f);
	load.m_endColor = float4(0, 1, 1, 3.00f);
	load.m_startScale = 2;
	load.m_endScale = 0;
	load.m_maxLife = 0.5f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(1, 1, 1);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 30;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.00f;
	load.m_startSpeed = 10;
	load.m_endSpeed = 10;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.5f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("White Particle");

	m_mEmitterTypes["Whip Bullet Death"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Missile Bullet Death
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 3.0f);
	load.m_endColor = float4(1, 0, 1, 3.00f);
	load.m_startScale = 4;
	load.m_endScale = 0;
	load.m_maxLife = 0.5f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(1, 1, 1);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 150;
	load.m_spawnCount = 150;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.00f;
	load.m_startSpeed = 20;
	load.m_endSpeed = 20;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.5f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("White Particle");

	m_mEmitterTypes["Missile Bullet Death"] = load;

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////

	// Player Trail
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.5f);
	load.m_endColor = float4(1, 1, 1, 1.5f);
	load.m_startScale = 5;
	load.m_endScale = 0;
	load.m_maxLife = 0.2f;
	load.m_minLife = 0.2f;
	load.m_constDirection = float3(0, 0, -1);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, -3.5f);
	load.m_spawnNum = 100;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.015f;
	load.m_startSpeed = 0;
	load.m_endSpeed = 0;
	load.m_flags = AllRandom | NoNormalize;
	load.m_data.timer = 2.0f;
	load.loop = true;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("Fire Particle");

	m_mEmitterTypes["Player Trail"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Player Boost
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.5f);
	load.m_endColor = float4(1, 1, 1, 1.5f);
	load.m_startScale = 10;
	load.m_endScale = 0;
	load.m_maxLife = 0.25f;
	load.m_minLife = 0.25f;
	load.m_constDirection = float3(0, 0, -1);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, -3.5f);
	load.m_spawnNum = 100;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.0075f;
	load.m_startSpeed = 0;
	load.m_endSpeed = 0;
	load.m_flags = AllRandom | NoNormalize;
	load.m_data.timer = 0.25f;
	load.loop = false;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("Fire Particle");

	m_mEmitterTypes["Player Boost"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Spread Bullet Trail
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.5f);
	load.m_endColor = float4(1, 1, 1, 1.5f);
	load.m_startScale = 2;
	load.m_endScale = 0;
	load.m_maxLife = 0.1f;
	load.m_minLife = 0.1f;
	load.m_constDirection = float3(0, 0, -1);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, -1);
	load.m_spawnNum = 30;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.015f;
	load.m_startSpeed = 0;
	load.m_endSpeed = 0;
	load.m_flags = AllRandom | NoNormalize;
	load.m_data.timer = 2.0f;
	load.loop = true;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("Gold Particle");

	m_mEmitterTypes["Spread Bullet Trail"] = load;

	//////////////////////////////////////////////////////////////////////////


	// Enemy Smimple Bullet Trail
	load.Reset();
	load.m_startColor = float4(1, 0, 0, 1.5f);
	load.m_endColor = float4(1, 0, 0, 1.5f);
	load.m_startScale = 2;
	load.m_endScale = 0;
	load.m_maxLife = 0.3f;
	load.m_minLife = 0.3f;
	load.m_constDirection = float3(0, 0, -1);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, -1.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.03f;
	load.m_startSpeed = 20;
	load.m_endSpeed = 20;
	load.m_flags = AddParentVelocity;
	load.m_data.timer = 2.0f;
	load.loop = true;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("White Particle");

	m_mEmitterTypes["Enemy Simple Bullet Trail"] = load;

	//////////////////////////////////////////////////////////////////////////

	

	// Missile Bullet Trail
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 3);
	load.m_endColor = float4(1, 0, 1, 3);
	load.m_startScale = 2;
	load.m_endScale = 0;
	load.m_maxLife = 0.15f;
	load.m_minLife = 0.15f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, -1.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.01f;
	load.m_startSpeed = 0;
	load.m_endSpeed = 0;
	load.m_flags = AllRandom;
	load.m_data.timer = 2.5f;
	load.loop = true;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("White Particle");

	m_mEmitterTypes["Missile Bullet Trail"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Orange fire explosion
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.8f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 10;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360 * 3;
	load.m_render.SetID("Fire Particle");


	m_mEmitterTypes["Orange Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 6;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("Red Fire Particle");


	m_mEmitterTypes["Red Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 8;
	load.m_endScale = 0;
	load.m_maxLife = 4.5f;
	load.m_minLife = 4.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 30;
	load.m_endSpeed = 10;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360;
	load.m_render.SetID("Fire Particle");


	m_mEmitterTypes["Player Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 6;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360 * 3;
	load.m_render.SetID("Yellow Fire Particle");


	m_mEmitterTypes["Yellow Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 6;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360 * 3;
	load.m_render.SetID("Cyan Fire Particle");


	m_mEmitterTypes["Cyan Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 6;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360 * 3;
	load.m_render.SetID("Orangey Fire Particle");


	m_mEmitterTypes["Orange Orange Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 6;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360 * 3;
	load.m_render.SetID("Green Fire Particle");


	m_mEmitterTypes["Green Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 6;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360 * 3;
	load.m_render.SetID("Purple Fire Particle");


	m_mEmitterTypes["Purple Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Red fire explosion
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 6;
	load.m_endScale = 0;
	load.m_maxLife = 2.75f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 400;
	load.m_spawnCount = 400;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360 * 3;
	load.m_render.SetID("Pink Fire Particle");


	m_mEmitterTypes["Pink Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Orange fire explosion
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.8f);
	load.m_endColor = float4(1, 1, 1, 1.0f);
	load.m_startScale = 10;
	load.m_endScale = 0;
	load.m_maxLife = 1.5f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0.2f, 0.2f, 0.2f);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 250;
	load.m_spawnCount = 250;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 80;
	load.m_endSpeed = 17.5f;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.1f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360;
	load.m_render.SetID("Fire Particle");


	m_mEmitterTypes["Small Orange Fire Explosion"] = load;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Tornado
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 0.2f);
	load.m_endColor = float4(1, 1, 1, 0);
	load.m_startScale = 30;
	load.m_endScale = 60;
	load.m_maxLife = 1.0f;
	load.m_minLife = 1.0f;
	load.m_constDirection = float3(0, 1, 0);
	load.m_randDirection = float3(0.15f, 0, 0.15f);
	//load.m_randDirection = float3(0, 1, 0);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 200;
	load.m_spawnCount = 2;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.01f;
	load.m_startSpeed = 50;
	load.m_endSpeed = 50;
	load.m_flags = AddParentVelocity | NegativeX | NegativeZ | NoNormalize;
	load.m_data.timer = 0.5f;
	load.loop = true;
	load.m_degrees_rotation = 600;
	load.m_render.SetID("Purple Particle");



	m_mEmitterTypes["Tornado"] = load;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// Whip Bullet Trail
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 3.0f);
	load.m_endColor = float4(1, 1, 1, 0.0f);
	load.m_startScale = 3;
	load.m_endScale = 0;
	load.m_maxLife = 0.1f;
	load.m_minLife = 0.1f;
	load.m_constDirection = float3(0, 0, -1);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 5;
	load.m_spawnTimer = 0.005f;
	load.m_spawnTime = 0;
	load.m_startSpeed = 0.0f;
	load.m_endSpeed = 0.0f;
	load.m_flags = AllRandom;
	load.m_data.timer = 1.5f;
	load.loop = false;
	load.m_degrees_rotation = 0;
	load.m_render.SetID("Fire Particle");

	m_mEmitterTypes["Whip Bullet Trail"] = load;

	//////////////////////////////////////////////////////////////////////////

	// Prominence Environmental Hazard 
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1);
	load.m_endColor = float4(0.7f, 1.0f, 0.0f, 1);
	load.m_startScale = 30;
	load.m_endScale = 0;
	load.m_maxLife = 0.6f;
	load.m_minLife = 0.6f;
	load.m_constDirection = float3(0, 1, 0);
	load.m_randDirection = float3(0.1f, 0, 0.1f);
	load.m_offset = float3(0.0f, 0, 0.0f);
	load.m_spawnNum = 200;
	load.m_spawnCount = 2;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.03f;
	load.m_startSpeed = 50;
	load.m_endSpeed = 50;
	load.m_degrees_rotation = 45;
	load.m_data.timer = 5;
	load.loop = true;
	load.m_flags = AllRandom | NoNormalize | EmitterFlags::SquareEmission;

	load.m_render.SetID("Fire Particle");
	m_mEmitterTypes["ProminenceMolecule"] = load;

	//////////////////////////////////////////////////////////////////////////

	// Prominence Bullet
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.5f);
	load.m_endColor = float4(1, 1, 1, 1.5f);
	load.m_startScale = 15;
	load.m_endScale = 0;
	load.m_maxLife = 0.6f;
	load.m_minLife = 0.6f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, 0);
	load.m_spawnNum = 100;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.03f;
	load.m_startSpeed = 0;
	load.m_endSpeed = 0;
	load.m_flags = AllRandom;
	load.m_data.timer = 2.0f;
	load.loop = true;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Fire Particle");
	m_mEmitterTypes["ProminenceBulletMolecule"] = load;
	//////////////////////////////////////////////////////////////////////////

	// Spread Bullet Deatg
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.5f);
	load.m_endColor = float4(1, 1, 1, 1.5f);
	load.m_startScale = 2;
	load.m_endScale = 0;
	load.m_maxLife = 0.5f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(1, 1, 1);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 30;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.00f;
	load.m_startSpeed = 15;
	load.m_endSpeed = 15;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.5f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Gold Particle");

	m_mEmitterTypes["Spread Bullet Death"] = load;


	/////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	// Enemy Simple Bullet Deatg
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.5f);
	load.m_endColor = float4(1, 0, 0, 1.5f);
	load.m_startScale = 2;
	load.m_endScale = 0;
	load.m_maxLife = 0.5f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(1, 1, 1);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 30;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.00f;
	load.m_startSpeed = 15;
	load.m_endSpeed = 15;
	load.m_flags = AllRandom;
	load.m_data.timer = 0.5f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("White Particle");

	m_mEmitterTypes["Enemy Simple Bullet Death"] = load;


	/////////////////////////////////////////////////////////
	// Test Emitter for using ShapeEmitter flag
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.0f);
	load.m_endColor = float4(1, 1, 1, 1.0f);
	load.m_startScale = 5;
	load.m_endScale = 10;
	load.m_maxLife = 1.2f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 10.0f, 0);
	load.m_randDirection = float3(10, 0, 10);
	load.m_offset = float3(6.0f, 0.0f, 6.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 25;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.05f;
	load.m_startSpeed = 25;
	load.m_endSpeed = 10;
	load.m_flags = NegativeX | NegativeZ | ShapeEmitter | SquareEmission | AddParentVelocity;
	load.m_data.timer = 0.5f;
	load.loop = true;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Spark Particle2");

	m_mEmitterTypes["Circle Test"] = load;

	/////////////////////////////////////////////////////////
	// Environmental Hazard Warning particle effect
	/*load.Reset();
	load.m_startColor = float4(1.5f, 0.0f, 0.0f, 1.0f);
	load.m_endColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	load.m_startScale = 10;
	load.m_endScale = 5;
	load.m_maxLife = 1.5f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0.0f, 0.0f, 0.0f);
	load.m_randDirection = float3(0.0f, 0.0f, 0.0f);
	load.m_offset = float3(3.0f, 0.5f, 3.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 100;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.01f;
	load.m_startSpeed = 25;
	load.m_endSpeed = 10;
	load.m_flags = NegativeX | NegativeZ | ShapeEmitter;
	load.m_data.timer = 1.5f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 360;*/

	load.Reset();
	load.m_startColor = float4(1.5f, 0.0f, 0.0f, 1.0f);
	load.m_endColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	load.m_startScale = 7;
	load.m_endScale = 9;
	load.m_maxLife = 1.5f;
	load.m_minLife = 1.5f;
	load.m_constDirection = float3(0.0f, 0.5f, 0.0f);
	load.m_randDirection = float3(0.0f, 0.0f, 0.0f);
	load.m_offset = float3(3.0f, 0.5f, 3.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 1;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 1.55f;
	load.m_startSpeed = 0;
	load.m_endSpeed = 0;
	load.m_flags = NegativeX | NegativeZ | ShapeEmitter | ShellEmitter;
	load.m_data.timer = 1.5f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Warning Skull");

	m_mEmitterTypes["Hazard Warning"] = load;
	/////////////////////////////////////////////////////////

	// Golaith Level up
	load.Reset();
	load.m_startColor = float4(1.2f, 1.2f, 1.2f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 7;
	load.m_endScale = 0.5f;
	load.m_maxLife = 0.5f;
	load.m_minLife = 0.4f;
	load.m_constDirection = float3(0, 7, 0);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 100;
	load.m_spawnCount = 10;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.05f;
	load.m_startSpeed = 4;
	load.m_endSpeed = 4;
	load.m_degrees_rotation = 270;
	load.m_data.timer = 0.5f;
	load.loop = false;
	load.m_flags = EmitterFlags::NegativeX | EmitterFlags::NegativeZ;

	load.m_render.SetID("Yellow Fire Particle");
	m_mEmitterTypes["GoliathLevelUp"] = load;

	//////////////////////////////////////////////////////////////////////////

	// Golaith Level up
	load.Reset();
	load.m_startColor = float4(1.2f, 1.2f, 1.2f, 1.5f);
	load.m_endColor = float4(0.4f, 0.4f, 0.4f, 0.7f);
	load.m_startScale = 7;
	load.m_endScale = 0.5f;
	load.m_maxLife = 0.5f;
	load.m_minLife = 0.4f;
	load.m_constDirection = float3(0, 7, 0);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 100;
	load.m_spawnCount = 10;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.05f;
	load.m_startSpeed = 4;
	load.m_endSpeed = 4;
	load.m_degrees_rotation = 270;
	load.m_data.timer = 0.5f;
	load.loop = false;
	load.m_flags = EmitterFlags::NegativeX | EmitterFlags::NegativeZ;

	load.m_render.SetID("Basic Molecule");
	m_mEmitterTypes["GoliathLevelDone"] = load;

	//////////////////////////////////////////////////////////////////////////

	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.0f);
	load.m_endColor = float4(1, 1, 1, 1.0f);
	load.m_startScale = 3.0f;
	load.m_endScale = 3.0f;
	load.m_maxLife = 100.0f;
	load.m_minLife = 100.0f;
	load.m_constDirection = float3(0, 0.0f, 0);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(3.0f, 3.0f, 3.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 500;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.1f;
	load.m_startSpeed = 25;
	load.m_endSpeed = 10;
	load.m_flags = AllRandom | ShellEmitter | ShapeEmitter | AddParentVelocity | RandomColor;
	load.m_data.timer = 0.5f;
	load.loop = true;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Spark Particle2");

	m_mEmitterTypes["Shell Test"] = load;

	//////////////////////////////////////////////////////////sssssssss
	// Singularity Particle Effect
	load.Reset();
	load.m_startColor = float4(1.5f, 1.5f, 1.5f, 1);
	load.m_endColor = float4(0.2f, 0.2f, 0.2f, 1.0f);
	load.m_startScale = 0;
	load.m_endScale = 3;
	load.m_maxLife = 2.0f;
	load.m_minLife = 1.0f;
	load.m_constDirection = float3(1.0f, 1.0f, 1.0f);
	load.m_randDirection = float3(0.0f, 0.0f, 0.0f);
	load.m_offset = float3(25.0f, 6, 25.0f);
	load.m_spawnNum = 0;
	load.m_spawnCount = 6;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.03f;
	load.m_startSpeed = 10;
	load.m_endSpeed = 25;
	load.m_flags = ShapeEmitter | MoveToPosition | ShellEmitter | AllRandom | AddParentVelocity;
	load.m_data.timer = 1.5f;
	load.loop = true;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 900;

	load.m_render.SetID("Spark Particle4");

	m_mEmitterTypes["Singularity Effect"] = load;
	/////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// Singularity Particle Effect
	load.Reset();
	load.m_startColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	load.m_endColor = float4(1.5f, 1.5f, 1.5f, 1.0f);
	load.m_startScale =100;
	load.m_endScale = 0.5f;
	load.m_maxLife = 2.0f;
	load.m_minLife = 1.0f;
	load.m_constDirection = float3(10.0f, 10.0f, 10.0f);
	load.m_randDirection = float3(0.0f, 0.0f, 0.0f);
	load.m_offset = float3(2500.0f, -0.5f, 2500.0f);
	load.m_spawnNum = 0;
	load.m_spawnCount = 6;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.03f;
	load.m_startSpeed = 10;
	load.m_endSpeed = 25;
	load.m_flags = ShapeEmitter | MoveToPosition | ShellEmitter | NegativeX | NegativeZ | AddParentVelocity;
	load.m_data.timer = 1.5f;
	load.loop = true;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Spark Particle4");

	m_mEmitterTypes["Inner Star Effect"] = load;
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	// Test Emitter for using ShapeEmitter flag
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 1.0f);
	load.m_endColor = float4(1, 1, 1, 1.0f);
	load.m_startScale = 5;
	load.m_endScale = 10;
	load.m_maxLife = 0;
	load.m_minLife = 0;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(5, 0, 5);
	load.m_offset = float3(3.0f, 0.0f, 3.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 25;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.05f;
	load.m_startSpeed = 25;
	load.m_endSpeed = 10;
	load.m_flags = NegativeX | NegativeZ | ShapeEmitter | SquareEmission ;
	load.m_data.timer = 0.5f;
	load.loop = true;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Spark Particle2");

	m_mEmitterTypes["Spawn Locale"] = load;
	//////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	// Fireworks for Level Win
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 2.0f);
	load.m_endColor = float4(1, 1, 1, 1.0f);
	load.m_startScale = 3.5f;
	load.m_endScale = 0.5f;
	load.m_maxLife = 1.0f;
	load.m_minLife = 0.5f;
	load.m_constDirection = float3(0, 0, 0);
	load.m_randDirection = float3(5, 5, 5);
	load.m_offset = float3(0.0f, 0.0f, 0.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 150;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 1.1f;
	load.m_startSpeed = 65;
	load.m_endSpeed = 35;
	load.m_flags = AllRandom | RandomColor | ShapeEmitter;
	load.m_data.timer = 5.0f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 530;

	load.m_render.SetID("Spark Particle2");

	m_mEmitterTypes["FireWork"] = load;
	//
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// Fireworks for Level Win
	load.Reset();
	load.m_startColor = float4(1, 1, 1, 2.0f);
	load.m_endColor = float4(1, 1, 1, 1.0f);
	load.m_startScale = 3.5f;
	load.m_endScale = 1.5f;
	load.m_maxLife = 0.2f;
	load.m_minLife = 0.2f;
	load.m_constDirection = float3(1, 1, 1);
	load.m_randDirection = float3(0, 0, 0);
	load.m_offset = float3(3.0f, 0.5f, 3.0f);
	load.m_spawnNum = 30;
	load.m_spawnCount = 150;
	load.m_spawnTimer = 0;
	load.m_spawnTime = 0.25f;
	load.m_startSpeed = 50;
	load.m_endSpeed = 50;
	load.m_flags = NegativeX | NegativeZ | ShapeEmitter | MoveToPosition | ShellEmitter | AddParentVelocity | MoveAwayFromPosition;
	load.m_data.timer = 0.2f;
	load.loop = false;
	load.m_attach = nullptr;
	load.m_degrees_rotation = 0;

	load.m_render.SetID("Spark Particle2");

	m_mEmitterTypes["Pulse"] = load;

	//////////////////////////////////////////////////////////

	load.Reset();
	load.loop = true;
	load.m_constDirection = float3(0, 100, 100);
	load.m_endScale = 0.7f;
	load.m_startScale = 0.7f;
	load.m_spawnTime = 0.05f;
	load.m_spawnCount = 5;
	load.m_minLife = 20.0f;

	load.m_render.SetID("Basic Molecule");
	m_mEmitterTypes["DebugMolecule"] = load;
	return false;
}

// Allows anyone to create an emitter, pass it into this function with a string for reuse later
void MoleculeManager::CreateEmitter(std::string name, MoleculeEmitter &emit)
{
	if (m_mEmitterTypes.count(name))
	{
		return;
	}
	m_mEmitterTypes[name] = emit;
}

void MoleculeManager::ResetEmitters()
{
	auto it = m_plLiveEmitters.begin();

	while (it != m_plLiveEmitters.end())
	{
		auto remove = it;
		it++;
		ReclaimMolecules((*remove));
		(*remove)->Reset();
		// now splice
		m_plEmitters.splice(m_plEmitters.begin(), m_plLiveEmitters, remove);

	}
}


void MoleculeManager::DebugMe()
{
	std::fstream file;
	file.open("DebugDump.txt", std::ios_base::app);
	
	if(!file.is_open())
	{
		// Well crap
		std::cout << "Sorry, didn't open \n";
		return;
	}


	file.write("=== Emitter Data ===\n", 21);
	int index = 0;
	auto it = m_plLiveEmitters.begin();


	while(it != m_plLiveEmitters.end())
	{
		file.write((const char*)&(*it)->m_data.index, 2);
		file.write("   ", 4);
		file.write((const char*)&(*it)->m_data.size, 2);
		file.write("\n", 1);
		
		it++;
		index++;
	}

	file.close();
}