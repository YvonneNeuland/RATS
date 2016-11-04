#include "stdafx.h"
#include "BehavBoss.h"


#include "AICompBase.h"
#include "../Object Manager/GameObject.h"
#include "../Object Manager/ObjectManager.h"
#include "../Components/HealthComponent.h"
#include "../Audio/FFmod.h"
#include "../Molecules/MoleculeManager.h"
#include "../Renderer/RenderComp.h"
#include "../Renderer/D3DGraphics.h"
#include "../Components/GlowComponent.h"
#include "../Object Manager/ObjectManager.h"
#include "../Object Manager/ObjectFactory.h"
#include "../States/GamePlayState.h"

extern	FFmod*		g_AudioSystem;
extern D3DGraphics* globalGraphicsPointer;

#define BASE_SHOOT_DELAY 0.125f

BehavBoss::BehavBoss()
{
	m_logicType = bBOSS;
	Reset();
}


BehavBoss::~BehavBoss()
{
}

void BehavBoss::DoLogic(float _dt)
{
	HealthComponent* bossHealth = reinterpret_cast<HealthComponent*>(m_Owner->GetOwner()->GetComponent("HealthComponent"));


	if (bFirst)
	{
		//GetOwner()->GetOwner()->SetShielded(true);
		m_shootTransform = GetOwner()->GetOwner()->GetTransform();
		m_fShieldTimer = 12;
		bossHealth->SetInvincibility(true);
		bFirst = false;

		globalGraphicsPointer->SetBossShieldShowing(true);


		//std::cout << bossHealth->GetHealth() << "\n";
	}

	if (m_fShieldTimer > 0)
	{
		m_fShieldTimer -= _dt;
		if (m_fShieldTimer <= 0)
		{
			m_fShieldTimer = 0;
			bossHealth->SetInvincibility(false);
			//reinterpret_cast<RenderComp*>(m_Owner->GetOwner()->GetComponent("RenderComp"))->SetGlowID("Boss Glow2");
			//SwitchShield("Boss Glow", "Boss Glow2");

			globalGraphicsPointer->SetBossShieldShowing(false);


			if (nStage > 1)
			{
				SendMinions(nStage * 2);
			}
			
		}


		if (m_fShootTimer > 0)
		{
			m_fShootTimer -= _dt;
			if (m_fShootTimer <= 0)
			{
				m_fShootTimer = BASE_SHOOT_DELAY ;
				XMFLOAT3 tmpBack;
				if (nStage == 5)
				{
					ShootSpread(3, *m_shootTransform.GetZAxis());
					tmpBack = *m_shootTransform.GetZAxis();
					tmpBack.x *= -1;
					tmpBack.y *= -1;
					tmpBack.z *= -1;
					ShootSpread(3, tmpBack);
				
					ShootSpread(3, *m_shootTransform.GetXAxis());
					tmpBack = *m_shootTransform.GetXAxis();
					tmpBack.x *= -1;
					tmpBack.y *= -1;
					tmpBack.z *= -1;
					ShootSpread(3, tmpBack);
				
				
				}
				else
				{
					ShootSpread(nStage + 3, *m_shootTransform.GetZAxis());
					tmpBack = *m_shootTransform.GetZAxis();
					tmpBack.x *= -1;
					tmpBack.y *= -1;
					tmpBack.z *= -1;
					ShootSpread(nStage + 3, tmpBack);
				}

				
			}
		}

		if (nStage > 2 && nStage < 5)
		{
			m_fMissileTimer -= _dt;

			if (m_fMissileTimer <= 0)
			{
				m_fMissileTimer = BASE_SHOOT_DELAY * 12;

				m_shootTransform.RotateBy(1, 90, 1, true);
				ShootHSM();
				m_shootTransform.RotateBy(1, 180, 1, true);
				ShootHSM();
				m_shootTransform.RotateBy(1, 90, 1, true);
			}
		}
	}




	if (m_fLastHealth > bossHealth->GetHealth())
	{
		m_fDmgTaken += m_fLastHealth - bossHealth->GetHealth();
		m_fLastHealth = bossHealth->GetHealth();
	}

	if (m_fDmgTaken >= 400)
	{
		nStage++;
		m_fDmgTaken = 0;
		m_fShieldTimer = 12;
		m_fShootTimer = 2;
		m_fMissileTimer = 2;
		bossHealth->SetInvincibility(true);

		std::cout << bossHealth->GetHealth() << "\n";

		//SwitchShield("Boss Glow2", "Boss Glow");

		globalGraphicsPointer->SetBossShieldShowing(true);

		ShootHSM();
		m_shootTransform.RotateBy(1, 90, 1, true);
		ShootHSM();
		m_shootTransform.RotateBy(1, 90, 1, true);
		ShootHSM();
		m_shootTransform.RotateBy(1, 90, 1, true);
		ShootHSM();
		m_shootTransform.RotateBy(1, 90, 1, true);

		//reinterpret_cast<RenderComp*>(m_Owner->GetOwner()->GetComponent("RenderComp"))->SetGlowID("Boss Glow");
		//GetOwner()->GetOwner()->m_ObjManager->m_Molecules->BuildPosEmitter(*GetOwner()->GetOwner()->GetTransform().GetPosition(), 150, "Orange Fire Explosion");
	}


	UpdateFiringDirection(_dt);
	m_shootTransform.SetPosition(*GetOwner()->GetOwner()->GetTransform().GetPosition());

}

void BehavBoss::Reset()
{
	m_fDmgTaken = 0;
	nStage = 0;
	m_fLastHealth = 2400;
	bFirst = true;
	m_fShieldTimer = 0;
	m_fShootTimer = 2;
	m_fMissileTimer = 2;
	globalGraphicsPointer->SetBossShieldShowing(false);

}

void BehavBoss::ShootSpread(int numBullets, DirectX::XMFLOAT3 firingDir)
{
	ObjectManager* objMan = GetOwner()->GetOwner()->m_ObjManager;
	float angle;
	//int numBullets = gameData->m_Spread.fire_rate_upgrade_num + 2;
	angle = (XM_PI * 0.3f) + (nStage == 4 ? 0.0f : 0.1f * nStage);//XM_PIDIV4 * (nStage + 1);//(45 + 7 * (nStage + 1)) * ((XM_PIDIV4 * (nStage + 1)) / 180.0f);
	//m_pPlayer = GetPlayer();

	GameObject* actualObject = GetOwner()->GetOwner();

	XMVECTOR startingDir =
		XMVector3Rotate(XMLoadFloat3(&firingDir), XMQuaternionRotationAxis(XMLoadFloat3(actualObject->GetTransform().GetYAxis()), -(angle*0.5f)));

	XMVECTOR quat = XMQuaternionRotationAxis(XMLoadFloat3(actualObject->GetTransform().GetYAxis()), angle / (numBullets - 1));

	GameObject* newBullet;
	XMFLOAT3 newZ, newY, newX;

	for (int currBullet = 0; currBullet < numBullets; ++currBullet)
	{
		
		newBullet = objMan->CreateObject(EvilBullet_Simple, *(actualObject)->GetTransform().GetPosition());
			newBullet->SetVelocityInWorldSpace(false);
		reinterpret_cast<RenderComp*>(newBullet->GetComponent("RenderComp"));

		XMStoreFloat3(&newZ, startingDir);
		XMStoreFloat3(&newX, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(actualObject->GetTransform().GetYAxis()), XMLoadFloat3(&newZ))));
		XMStoreFloat3(&newY, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&newZ), XMLoadFloat3(&newX))));

		newBullet->GetTransform().SetXAxis(newX);
		newBullet->GetTransform().SetYAxis(newY);
		newBullet->GetTransform().SetZAxis(newZ);

		startingDir = XMVector3Rotate(startingDir, quat);

		
		newBullet->SetVelocity(DirectX::XMFLOAT3(0, 0, 70));
		


		objMan->m_Molecules->BuildEmitter(newBullet, 30, "Spread Bullet Trail");

	}

	g_AudioSystem->PlaySound("RATS_EvilBullet", newBullet);
}


void BehavBoss::ShootHSM()
{
	ObjectManager* objMan = GetOwner()->GetOwner()->m_ObjManager;

	GameObject* newBullet = objMan->CreateObject(EvilBullet_HeatSeek, *m_shootTransform.GetPosition());
	DirectX::XMFLOAT3 newPos;

	newBullet->GetTransform() = m_shootTransform;
	DirectX::XMStoreFloat3(&newPos, DirectX::XMLoadFloat3(m_shootTransform.GetPosition()) + (DirectX::XMLoadFloat3(m_shootTransform.GetZAxis()) * 3));
	newBullet->GetTransform().SetPosition(newPos);
	reinterpret_cast<AICompBase*>(newBullet->GetComponent("AICompBase"))->SetShooter(GetOwner());
	reinterpret_cast<AICompBase*>(newBullet->GetComponent("AICompBase"))->SetTurnSpeed(0.6f);
	g_AudioSystem->PlaySound("RATS_MissileLoop", newBullet, false, true);
}


void BehavBoss::UpdateFiringDirection(float _dt)
{

	static float currTime;

	currTime += _dt;
	if (nStage % 2 != 0)
		m_shootTransform.RotateBy(1, -1.0f * (40 * (sinf(currTime) * ((1 / float(7 - nStage)) * 2) )), 2 * _dt, true);
	else
		m_shootTransform.RotateBy(1, (float)(40 * (cosf(currTime) * ((1 / float(7 - nStage)) * 2) )), 2 * _dt, true);

// 	switch (nStage)
// 	{
// 
// 	case 0:
// 
// 		break;
// 
// 	case 1:
// 
// 		break;
// 
// 	case 2:
// 
// 		break;
// 
// 	case 3:
// 
// 		break;
// 
// 	case 4:
// 
// 		break;
// 
// 
// 	default:




		//break;
// 	}
}

void BehavBoss::SwitchShield(std::string _old, std::string _new)
{
	auto _iter = GetOwner()->GetOwner()->GetComponents().begin();
	while (_iter != GetOwner()->GetOwner()->GetComponents().end())
	{
		if ((*_iter)->GetType() == eCOMPONENT_TYPE::Glow)
		{
			if (reinterpret_cast<GlowComponent*>(*_iter)->GetGlowID() != "")
				break;
		}

		_iter++;
	}
	if (_iter != GetOwner()->GetOwner()->GetComponents().end())
	{
		GetOwner()->GetOwner()->m_ObjManager->m_pFactory->RemoveGlowEffect((*_iter), globalGraphicsPointer->HashString(_old));
		GetOwner()->GetOwner()->m_ObjManager->m_pFactory->AddGlowEffect(GetOwner()->GetOwner(), _new);
	}
}

void BehavBoss::SendMinions(int numEmemies)
{
	float rotVal = 360.0f / (float)numEmemies;
	ObjectManager* pObjMan = GetOwner()->GetOwner()->m_ObjManager;
	GameObject* neewMinion = nullptr;
	DirectX::XMFLOAT3 newPos;

	pObjMan->m_pGPS->m_unNumEnemiesToKill += numEmemies;

	for (int i = 0; i < numEmemies; i++)
	{
		neewMinion = pObjMan->CreateObject(Enemy_Bomber, *m_shootTransform.GetPosition());
		neewMinion->GetTransform() = m_shootTransform;
		DirectX::XMStoreFloat3(&newPos, DirectX::XMLoadFloat3(m_shootTransform.GetPosition()) + (DirectX::XMLoadFloat3(m_shootTransform.GetZAxis()) * 3));
		neewMinion->GetTransform().SetPosition(newPos);
		m_shootTransform.RotateBy(1, rotVal, 1, true);
	}

}
