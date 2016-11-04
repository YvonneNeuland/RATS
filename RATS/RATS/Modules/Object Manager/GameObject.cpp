#include "stdafx.h"
#include "GameObject.h"
#include "../Components/HealthComponent.h"
#include <typeinfo>
#include "../Renderer/D3DGraphics.h"
#include "ObjectManager.h"
#include "../Molecules/MoleculeManager.h"
//#include "../../Wwise files/AudioSystemWwise.h"

#include "../Components/Movement/GroundFollowComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/InputComponent.h"

//extern AudioSystemWwise* g_AudioSystem;
extern D3DGraphics* globalGraphicsPointer;
using namespace DirectX;

GameObject::GameObject()
{
	Reset();
	//g_AudioSystem->RegisterEntity(&m_transform, "");
}

GameObject::~GameObject()
{
	//TODO: How do the clear listy thingy?
	//delete [] m_compList;
	//m_compList.clear();	
	//g_AudioSystem->UnRegisterEntity(&m_transform);
}

// GetComponent("BaseComponent")
BaseComponent* GameObject::GetComponent(char* type)
{
	//std::string desiredString = type;
	std::string entryType;


	if (strcmp("GroundFollowComponent", type) == 0)
	{
		if (moveComp != nullptr)
		{
			return moveComp;
		}
	}
	else if (strcmp("HealthComponent", type) == 0)
	{
		if (healthComp != nullptr)
		{
			return healthComp;
		}
	}
	else if (strcmp("ColliderComp", type) == 0)
	{
		if (colliderComp != nullptr)
		{
			return colliderComp;
		}
	}
	else if (strcmp("InputComponent", type) == 0)
	{
		if (inputComp != nullptr)
		{
			return inputComp;
		}
	}


	if (m_compList.empty())
	{
		return nullptr;
	}

	std::list<BaseComponent*>::iterator currComp = m_compList.begin();

	while (currComp != m_compList.end())
	{
		entryType = typeid(**currComp).name();
		if (entryType.find(type) != entryType.npos)
		{
			if (strcmp("GroundFollowComponent", type) == 0)
			{
				moveComp = *currComp;
			}
			else if (strcmp("HealthComponent", type) == 0)
			{
				healthComp = *currComp;
			}
			else if (strcmp("ColliderComp", type) == 0)
			{
				colliderComp = *currComp;
			}
			else if (strcmp("InputComponent", type) == 0)
			{
				inputComp = *currComp;
			}

			return (*currComp);
		}
		++currComp;
	}

	return nullptr;
}

void GameObject::Update(float dt)
{
	// 	if (!GetFlag(Spwaning) && this->m_type != eOBJECT_TYPE::Player_Obj)
	// 		return;


	BaseComponent* mov = GetComponent<GroundFollowComponent>();


	if (mov && !GetFlag(Spwaning) && m_type != Landing_Indicator)
	{
		mov->Update(dt);
	}

	mov = GetComponent<HealthComponent>();
	if (mov)
	{
		mov->Update(dt);
	}

	if (GetType() == eOBJECT_TYPE::Player_Obj)
	{
		GetComponent<InputComponent>()->Update(dt);
	}

	XMVECTOR veloc = XMLoadFloat3(&XMFLOAT3(0, 0, 0));

	if (m_fSpinTImer > 0)
	{
		m_fSpinTImer -= dt;
		veloc = XMLoadFloat3(&m_SpinOutDir) * 5;
		GetTransform().RotateBy(1, XM_PI, 3*dt);
		if (m_fSpinTImer <= 0)
		{
			m_fSpinTImer = 0;
			SetVelocityInWorldSpace(false);
		}
	}
	else if (m_VelocityInWorldSpace)
		veloc = XMLoadFloat3(&m_Velocity);
	else
	{
		veloc += XMVector3Normalize(XMLoadFloat3(GetTransform().GetXAxis())) * m_Velocity.x;
		veloc += XMVector3Normalize(XMLoadFloat3(GetTransform().GetYAxis())) * m_Velocity.y;
		veloc += XMVector3Normalize(XMLoadFloat3(GetTransform().GetZAxis())) * m_Velocity.z;
	}


	XMVECTOR impulseVec = DirectX::XMLoadFloat3(&m_Impulse);

	if (DirectX::XMVector3Dot(impulseVec, impulseVec).m128_f32[0] > FLT_EPSILON)
	{
		impulseVec = impulseVec * 0.7f;
		DirectX::XMStoreFloat3(&m_Impulse, impulseVec);
		if (DirectX::XMVector3LengthEst(impulseVec).m128_f32[0] <= 0.5f)
		{
			m_Impulse = DirectX::XMFLOAT3(0, 0, 0);
		}
		veloc += impulseVec;
	}

	//if (m_transform.GetPosition()->x == )

	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, XMLoadFloat3(m_transform.GetPosition()) + (veloc*dt));


	m_transform.SetPosition(newPos);


	//m_transform.RotateBy(2, 0.5f * dt, true);
	//m_transform.RotateBy(0, 0.5f * dt, true);

	if (m_fDmgTimer > 0)
	{
		m_fDmgTimer -= dt;
		if (m_fDmgTimer < 0)
			m_fDmgTimer = 0;

		if (m_type == Player_Obj)
			FlashWhite(dt);
	}

	// Update rotation, if there is any
	if (rotRad > 0 && rotAx > -1)
	{
		// Update Transform for rotation
		GetTransform().RotateBy(rotAx, rotRad, dt);
	}

	//Update orbit, if there is any
	if (isOrbiting && GetType() == Scenery_Obj)
	{
		XMFLOAT3 nextPos = (*GetTransform().GetPosition());
		nextPos.x -= pointToOrbit.x;
		nextPos.y -= pointToOrbit.y;
		nextPos.z -= pointToOrbit.z;

		XMStoreFloat3(&nextPos, XMVector3Rotate(XMLoadFloat3(&nextPos), XMQuaternionRotationAxis(XMLoadFloat3(&orbitAxis), orbitSpeed*dt)));

		nextPos.x += pointToOrbit.x;
		nextPos.y += pointToOrbit.y;
		nextPos.z += pointToOrbit.z;

		GetTransform().SetPosition(nextPos);
	}

	if (m_pChannel)
	{
		m_FmodVec.x = m_transform.GetPosition()->x;
		m_FmodVec.y = m_transform.GetPosition()->y;
		m_FmodVec.z = m_transform.GetPosition()->z;
		m_pChannel->set3DAttributes(&m_FmodVec, 0);
		bool isPlaying;
		// 		int looping;
		// 		m_pChannel->getLoopCount(&looping);
		m_pChannel->isPlaying(&isPlaying);
		// 		
		if (!isPlaying && m_type == Audio_OneShot)
			SetFlags(MostlyDead);
		// 			m_pChannel = nullptr;
	}

	if (m_bShielded)
	{
		m_fShieldTimer -= dt;
		if (m_fShieldTimer <= 0)
		{
			m_fShieldTimer = 0;
			m_bShielded = false;

			globalGraphicsPointer->SetPlayerShieldShowing(false);
		}
	}

	if (m_fFireTimer > 0)
	{
		m_fFireTimer -= dt;
		if (m_fFireTimer <= 0)
		{
			m_fFireTimer = 0;
			m_pFireParticle = nullptr;
		}
			

		if (healthComp != nullptr)
		{
			reinterpret_cast<HealthComponent*>(healthComp)->SetHealth(reinterpret_cast<HealthComponent*>(healthComp)->GetHealth() - 0.1f);
		}
	}

}

bool GameObject::TakeDamage(float val)
{
	if (m_fDmgTimer > 0 || GetFlag(Spwaning))
		return false;

	HealthComponent* myComp = reinterpret_cast<HealthComponent*>(GetComponent("HealthComponent"));
	if (myComp == nullptr)
		return false;

	if (myComp->GetInvincibility())
		return false;


	myComp->SetHealth(myComp->GetHealth() - val);

	if (m_type == eOBJECT_TYPE::Player_Obj)
		m_fDmgTimer = 0.5f;
	else
		m_fDmgTimer = 0.1f;

	return true;
}

void GameObject::SetFlags(unsigned char val)
{
	m_StatusFlags |= val;
}

// Clear individual flag(s) based on passed value
void GameObject::ClearFlags(unsigned char val)
{
	m_StatusFlags &= ~(val);
}

bool GameObject::GetFlag(unsigned char val)
{
	if (m_StatusFlags & val) return true;

	return false;
}

bool GameObject::isAnEnemy()
{
	// m_type > 0 && m_type < Player_obj
	//               G                         G                        G                              G                       G                           G                    G                              G
	if (m_type == Enemy_Bomber || m_type == Enemy_Boss || m_type == Enemy_Disruptor || m_type == Enemy_Dodger || m_type == Enemy_Goliath || m_type == Enemy_Koi || m_type == Enemy_Merc || m_type == Enemy_MineLayer)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GameObject::SetDead(bool dead)
{

	if ((isAnEnemy()) && dead)
	{
		globalGraphicsPointer->m_fireworkPositions.push_back(*(GetTransform().GetPosition()));

	}


	if (m_type == Enemy_Koi && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Red Fire Explosion");
	}
	else if ((m_type == Enemy_Disruptor || m_type == Enemy_Goliath) && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Yellow Fire Explosion");

	}
	else if (m_type == Enemy_Bomber && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Cyan Fire Explosion");

	}
	else if (m_type == Enemy_Dodger && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Orange Orange Fire Explosion");

	}
	else if (m_type == Enemy_MineLayer && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Purple Fire Explosion");

	}
	else if (m_type == Enemy_Merc && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Pink Fire Explosion");

	}
	else if (m_type == Enemy_Boss && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Purple Fire Explosion");
		explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Cyan Fire Explosion");
		explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Red Fire Explosion");
		explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Orange Orange Fire Explosion");
		explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Yellow Fire Explosion");
		explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 400, "Pink Fire Explosion");


	}
	else if ((m_type == eOBJECT_TYPE::Env_Prominence_Bullet || m_type == EvilBullet_HeatSeek) && dead)
	{
		MoleculeEmitter* explosion = m_ObjManager->m_Molecules->BuildPosEmitter(*(GetTransform().GetPosition()), 250, "Small Orange Fire Explosion");

	}


	if (dead)
	{
		SetFlags(MostlyDead);
		SetFlags(Waiting);
	}
	else
		ClearFlags(MostlyDead);
}

void GameObject::Reset()
{
	m_Velocity = m_Impulse = DirectX::XMFLOAT3(0, 0, 0);
	m_Destroy = false;
	m_StatusFlags = 0;
	m_fDmgTimer = 0;
	m_fFlashHelp = 0;
	rotAx = -1;
	rotRad = 0;
	m_trail = nullptr;
	m_pChannel = 0;
	m_SecondaryType = MAX_obj;

	m_bShielded = false;
	m_fShieldTimer = 0;
	bomberSucceed = false;
	m_fSpinTImer = 0;
	m_SpinOutDir = POS_ORIGIN;

	StopOrbiting();

	ResetPointers();
}

void GameObject::ResetPointers()
{
	healthComp = nullptr;
	moveComp = nullptr;
	colliderComp = nullptr;
	inputComp = nullptr;
}

void GameObject::DeathUpdate(float dt)
{
	/*SetFlags(AllDead);
	if (m_type == Player_Obj)
	globalGraphicsPointer->SetPlayerWhite(false);*/

	m_fDeathTimer -= dt;
	if (m_fDeathTimer < 0.0f)
	{
		SetFlags(AllDead);
		if (m_type == Player_Obj)
			globalGraphicsPointer->SetPlayerWhite(false);
	}

}

void GameObject::FlashWhite(float _dt)
{
	static bool isWhite = false;

	if (m_fDmgTimer == 0)
	{
		globalGraphicsPointer->SetPlayerWhite(false);
		isWhite = false;
		return;
	}

	m_fFlashHelp += _dt;
	if (m_fFlashHelp >= 0.1f)
	{
		m_fFlashHelp = 0;
		isWhite = !isWhite;
		globalGraphicsPointer->SetPlayerWhite(isWhite);
	}
}


void GameObject::SetObjManager(ObjectManager* objman)
{
	m_ObjManager = objman;
}

void GameObject::SetShielded(bool val)
{
	if (val == false)
	{
		m_bShielded = false;
		m_fShieldTimer = 0;
		return;
	}


	if (!m_bShielded)
		m_bShielded = true;


	m_fShieldTimer = 4;


}

void GameObject::SpinOut()
{
	m_SpinOutDir = *GetTransform().GetZAxis();
	m_fSpinTImer = 1.5f;
	SetVelocityInWorldSpace(true);
}

void GameObject::Ignite()
{
	m_fFireTimer = 2;
	m_pFireParticle = m_ObjManager->m_Molecules->BuildEmitter(this, 100, "ProminenceBulletMolecule");
	m_pFireParticle->m_data.timer = 2;
	m_pFireParticle->loop = false;
}
