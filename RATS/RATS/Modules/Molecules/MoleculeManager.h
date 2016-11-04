#pragma once

/************************

*************************/

// INCLUDES
#include <list>
#include <map>
#include <string>
#include "MoleculeEmitter.h"


// FORWARD DECLARES
class Molecule;
class GameObject;
//class MoleculeEmitter;

// Some quality of life typedef(s)
typedef DirectX::XMFLOAT3 float3;


// DEFINES
#define NUM_MOLECULES 100000
#define NUM_EMITTERS 5000



class MoleculeManager
{
public:
	MoleculeManager();
	~MoleculeManager();

	// Molecule Management
	

	// A rather standard update function
	void Update(float dt);

	// Build Emitter
	MoleculeEmitter* BuildEmitter(GameObject *target, unsigned short size = 100, std::string type = "DebugMolecule");
	MoleculeEmitter* BuildPosEmitter( DirectX::XMFLOAT3 pos, unsigned short size = 100, std::string type = "DebugMolecule" );

	void CreateEmitter(std::string name, MoleculeEmitter &emit);
	std::list<MoleculeEmitter*> *GetLiveEmitters() { return &m_plLiveEmitters; }
	
	// A file will be passed to this function and eventually 
	bool LoadEmitter(std::string file);

	// Reset Manager's live list
	void ResetEmitters();

private:

	// List of Emitters
	std::list<MoleculeEmitter*> m_plEmitters;
	std::list<MoleculeEmitter*> m_plLiveEmitters;

	// List of Emitter Data
	std::list<EmitterData*> m_plEmitterData;

	// Array of Molecules
	Molecule *m_pMoleculeBank = nullptr;

	std::map<std::string, MoleculeEmitter> m_mEmitterTypes;

	
	// Private Molecule management
	bool SetupEmitter(unsigned short size, MoleculeEmitter* emit, std::string type);
	void ReclaimMolecules(MoleculeEmitter* emit);


	// Helper Functions
	std::list<EmitterData*>::iterator FindIter(MoleculeEmitter *emit);

	void DebugMe();

};

