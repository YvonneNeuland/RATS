#pragma once
#include "../Object Manager/BaseObject.h"

// FWRD DECL
class GameObject;

// Declare a namespace for naming conflict resolution if necessary

// Primary Comp Types being derived from
enum eCOMPONENT_TYPE { Render = 0, Health, Movement, AI, Input, Collision, Glow, MAX_Comp };

enum eRENDERCOMP_TYPE {  Transparent = 0, Opaque, Render2D_Back, Render2D_Middle, Render2D_Middle2, Render2D_Middle3, Render2D_Front,
	Render2D_Text, RenderLightning, RenderParticle, RenderAnimTex, RenderRibbon, MAX_RenderComp };

// Unused Types
enum eAI_TYPE { TurnTo = 0, LookAt, Follow, MAX_AIComp};
enum eMOVECOMP_TYPE { MAX_MoveComp };
enum eINPUT_TYPE { MAX_InputComp, NUM_Input_Allocate = 5 };
enum eHEALTH { MAX_HealthComp, NUM_Health_Allocate = 500 };
//enum eVFX_TYPE { Lightning, Particle, MAX_VFXComp };

class BaseComponent : public BaseObject
{
protected:
	GameObject* m_pOwner;
	int m_type;

public:
	BaseComponent();
	virtual ~BaseComponent();

	virtual void Reset() = 0;

	GameObject* GetOwner() const { return m_pOwner; }
	void SetOwner(GameObject* val) { m_pOwner = val; }

	void SetType(int type) { m_type = type; }
	int GetType() {return m_type;}
};

