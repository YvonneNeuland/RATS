#include "stdafx.h"
#include "InputComponent.h"
#include "../Object Manager/GameObject.h"
#include <iostream>
#include "../Renderer/D3DGraphics.h"
#include "../AI/AICompBase.h"
#include "../Audio/FFmod.h"
#include "../Upgrade System/GameData.h"
#include "../Object Manager/ObjectManager.h"
#include "../Input/ThreeSixty.h"

extern D3DGraphics* globalGraphicsPointer;
extern	FFmod*		g_AudioSystem;
extern GameData* gameData;
extern ThreeSixty* gamePad;

using namespace DirectX;

#define TURNDASHBIT_ON m_flags |= 16
#define TURNDASHBIT_OFF m_flags &= ~16
#define DASHTIME 0.21f
#define DASHCOOLDOWN 0.35f
#define DASHSPEEDMAX 350
#define DASHSPEEDMIN 60

InputComponent::InputComponent()
{
	m_flags = 0;
	m_bMenuMode = true;
	m_bLastInputGamepad = false;
	m_fSpeed = DASHSPEEDMIN;
}


InputComponent::~InputComponent()
{
}

void InputComponent::HandleMovement(float _HorizAxis, float _VertAxis)
{

}

// parameter will be KeyUp/KeyDown; Hold on to up/down keys as flags
void InputComponent::HandleMovement(char PossibleVar)
{
	XMMATRIX matrix;
	XMFLOAT4X4 mat4;

	matrix = XMMatrixIdentity();

	// Based on current flag settings, rotate matrix to correct orientation	
	// Let's say UP
	m_pOwner->GetTransform().GetLocalMatrix();
	m_pOwner->SetVelocity(DirectX::XMFLOAT3(0, 0, 1));

	// Down


	// Left


	// Right

}

void InputComponent::HandleShoot(float _HorizAxis, float _VertAxis)
{

}



void InputComponent::HandleShoot()
{

}

void InputComponent::HandleDash()
{

}

void InputComponent::HandleEMP()
{

}

void InputComponent::Update(float dt)
{
	lastDT = dt;

	if (dashTime > 0)
	{
		dashTime -= dt;

		if (dashTime <= 0)
		{
			TURNDASHBIT_OFF;
			m_pOwner->ClearFlags(Dashing);
			//RibbonEffect* ribeff = GetOwner()->GetTrail();
			//ribeff->SetNextHalfWidth(ribeff->GetLowerHalfWidth());
		}
		ChangeVelocity(m_bLastInputGamepad);
	}
	else
	{
		if (dashCooldown > 0)
			dashCooldown -= dt;

		if (m_fSpeed > DASHSPEEDMIN)
		{
			m_fSpeed = max(DASHSPEEDMIN, m_fSpeed - (dt*2));
			ChangeVelocity(m_bLastInputGamepad);

		}
	}

	if (!bWasSpinning && GetOwner()->IsSpinningOut())
		bWasSpinning = true;

	if (bWasSpinning && !GetOwner()->IsSpinningOut())
	{
		bWasSpinning = false;
		ChangeVelocity(m_bLastInputGamepad);
	}

}

void InputComponent::HandleKeyDown(const Events::CGeneralEventArgs2<unsigned char, float>& args)
{
	if (GetOwner() == nullptr)
		return;

	if (gameData->m_bUsingGamepad)
		return;

	switch (args.GetData())
	{
	case 'W':
		m_flags |= eDIRECTION::Up;
		break;
	case 'A':
		m_flags |= eDIRECTION::Left;
		break;
	case 'S':
		m_flags |= eDIRECTION::Down;
		break;
	case 'D':
		m_flags |= eDIRECTION::Right;
		break;
	case VK_SPACE:
		if (dashCooldown <= 0 && !GetOwner()->IsSpinningOut())
		{
			TURNDASHBIT_ON;
			GetOwner()->SetFlags(Dashing);
			dashTime = DASHTIME;
			dashCooldown = DASHCOOLDOWN;

			GetOwner()->m_ObjManager->m_Molecules->BuildEmitter(GetOwner(), 100, "Player Boost");

			//RibbonEffect* ribeff = GetOwner()->GetTrail();
			//ribeff->SetNextHalfWidth(ribeff->GetUpperHalfWidth());
			//g_AudioSystem->PostEvent(AK::EVENTS::PLAY_RATS_DASH, &GetOwner()->GetTransform());
			g_AudioSystem->PlaySound("RATS_Dash", m_pOwner);

			if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
			{
				gamePad->Vibrate(0, 0.3f, 0.5f);
			}
		}
		break;
	default:
		break;
	}
	//std::cout << "DOWN: " << m_flags << "\n";
	m_bLastInputGamepad = false;
	ChangeVelocity();
}

void InputComponent::HandleKeyUp(const Events::CGeneralEventArgs<unsigned char>& args)
{
	if (gameData->m_bUsingGamepad)
		return;

	switch (args.GetData())
	{
	case 'W':
		m_flags &= ~(eDIRECTION::Up);
		break;
	case 'A':
		m_flags &= ~(eDIRECTION::Left);
		break;
	case 'S':
		m_flags &= ~(eDIRECTION::Down);
		break;
	case 'D':
		m_flags &= ~(eDIRECTION::Right);
		break;
	default:
		break;
	}
	m_bLastInputGamepad = false;
	//std::cout << "UP: " << m_flags << "\n";
	ChangeVelocity();
}

void InputComponent::ChangeVelocity(bool _GamePad)
{
	if (GetOwner()->IsSpinningOut())
	{
		//m_flags = 0;
		bWasSpinning = true;
		m_pOwner->SetVelocity(XMFLOAT3(0, 0, 0));
		return;
	}
		

	if (dashTime > DASHTIME * 0.4f)
	{
		m_fSpeed = min(DASHSPEEDMAX,m_fSpeed + ( (DASHSPEEDMAX - DASHSPEEDMIN) * lastDT * 6));
		//std::cout << m_fSpeed << "\n";
	}
	else 
	{
		m_fSpeed = max(DASHSPEEDMIN, m_fSpeed - ((DASHSPEEDMAX - DASHSPEEDMIN) * lastDT * 6));
		//std::cout << m_fSpeed << "\n";
	}

	
	bool UDused = ((m_flags & eDIRECTION::Up) ^ ((m_flags & eDIRECTION::Down) >> 1)) != 0,
		 LRused = ((m_flags &eDIRECTION::Left) ^ ((m_flags & eDIRECTION::Right) >> 1)) != 0;

	if (_GamePad)
	{
		// CONTROLLER STUFF
		float currentAngle = m_pOwner->GetTransform().GetRotation(1);

		XMVECTOR moveVec, scaledX, scaledY, currX = XMLoadFloat3(m_pOwner->GetTransform().GetXAxis()), currY = XMLoadFloat3(m_pOwner->GetTransform().GetYAxis());
		scaledX = DirectX::XMVectorMultiply(currX, XMLoadFloat3(&XMFLOAT3(m_cntrollerState.normLS[0], m_cntrollerState.normLS[0], m_cntrollerState.normLS[0])));
		scaledY = DirectX::XMVectorMultiply(currY, XMLoadFloat3(&XMFLOAT3(m_cntrollerState.normLS[1], m_cntrollerState.normLS[1], m_cntrollerState.normLS[1])));

		moveVec = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(scaledX, scaledY));

		float signage = DirectX::XMVector3Dot(currX, moveVec).m128_f32[0];

		if (signage != 0)
			signage /= abs(signage);
// 		else if (m_cntrollerState.normLS[1] < 0)
// 			signage = 1;

		float ang = DirectX::XMVector3AngleBetweenVectors(currY, moveVec).m128_f32[0] * signage;

		//XMVECTOR rotVal = DirectX::XMVectorATan2(XMLoadFloat3(&XMFLOAT3(m_cntrollerState.normLS[1], m_cntrollerState.normLS[1], m_cntrollerState.normLS[1])), XMLoadFloat3(&XMFLOAT3(m_cntrollerState.normLS[0], m_cntrollerState.normLS[0], m_cntrollerState.normLS[0])));

		m_pOwner->GetTransform().RotateBy(1, ang - currentAngle);

	//	float magnitude = (abs(m_cntrollerState.normLS[0]) + abs(m_cntrollerState.normLS[1])) * 0.5f;
		//std::cout /*<< m_cntrollerState.normLS[0] << ", " */<< m_cntrollerState.normMag << "\n";

		XMFLOAT3 zAxis = XMFLOAT3(0, 0, gameData->m_PlayerShip.speedScaler * m_fSpeed * m_cntrollerState.normMag[0]);

		m_pOwner->SetVelocity(zAxis);

		globalGraphicsPointer->SetCrosshairShowing(false);

	}
	else if (m_flags && (UDused || LRused))
	{
		// KEYBOARD STUFF

		float UD = 0, LR = 0, nextAngle = 0, currentAngle = m_pOwner->GetTransform().GetRotation(1);
		float *biggerOne = nullptr, *smallerOne = nullptr;

		if (m_flags & eDIRECTION::Up)
		{
			UD = 0;
		}
		else if (m_flags & eDIRECTION::Down)
		{
			UD = XM_PI;
		}

		if (m_flags & eDIRECTION::Left)
		{
			LR = 3 * XM_PIDIV2;
			if (UDused && !UD)
				UD = XM_2PI;
		}
		else if (m_flags & eDIRECTION::Right)
		{
			LR = XM_PIDIV2;
		}

		biggerOne = &(UD > LR ? UD : LR);
		smallerOne = &(biggerOne == &UD ? LR : UD);
		if (UDused && LRused)
			nextAngle = ((*biggerOne - *smallerOne) * 0.5f) + *smallerOne;
		else
		{
			if (UDused)
				nextAngle = UD;
			else
				nextAngle = LR;
		}

		m_pOwner->GetTransform().RotateBy(1, nextAngle - currentAngle);

		XMFLOAT3 zAxis = XMFLOAT3(0, 0, m_fSpeed * gameData->m_PlayerShip.speedScaler);

		m_pOwner->SetVelocity(zAxis);

		globalGraphicsPointer->SetCrosshairShowing(true);
	}
	else
	{
		m_pOwner->SetVelocity(XMFLOAT3(0,0,0));
		return;
	}

}

void InputComponent::SetupClients()
{
	using namespace Events;

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DirForward"), this, &InputComponent::HandleKeyDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DirBack"), this, &InputComponent::HandleKeyDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DirLeft"), this, &InputComponent::HandleKeyDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "DirRight"), this, &InputComponent::HandleKeyDown);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyPress", "PlayerDash"), this, &InputComponent::HandleKeyDown);

	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyUp", "UpForward"), this, &InputComponent::HandleKeyUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyUp", "UpBack"), this, &InputComponent::HandleKeyUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyUp", "UpLeft"), this, &InputComponent::HandleKeyUp);
	EventManager()->RegisterClient(MessageSystem::GetInstance()->Call<const EVENTID&, const EVENTID>("GetKeyUp", "UpRight"), this, &InputComponent::HandleKeyUp);

}

void InputComponent::UpdateGamePad(GamePad* _newInfo)
{
	if (_newInfo->state.dwPacketNumber == m_cntrollerState.state.dwPacketNumber)
		return;

	if (GetOwner() == nullptr)
		return;

	m_cntrollerState = *_newInfo;

	if (m_cntrollerState.buttons[buttonList::RIGHT_BUMPER] == buttonStatus::bPress ||
		m_cntrollerState.normTriggers[1] > 0.5f)
	{
		if (dashCooldown <= 0 && !GetOwner()->IsSpinningOut())
		{
			TURNDASHBIT_ON;
			m_pOwner->SetFlags(Dashing);
			dashTime = DASHTIME;
			dashCooldown = DASHCOOLDOWN;

			//RibbonEffect* ribeff = GetOwner()->GetTrail();
			//ribeff->SetNextHalfWidth(ribeff->GetUpperHalfWidth());
			GetOwner()->m_ObjManager->m_Molecules->BuildEmitter(GetOwner(), 100, "Player Boost");

			g_AudioSystem->PlaySound("RATS_Dash", m_pOwner);

			if (gameData->m_bUsingGamepad && gameData->m_bAllowVibration)
			{
				gamePad->Vibrate(0, 0.3f, 0.5f);
			}
		}
	}
	m_bLastInputGamepad = true;
	ChangeVelocity(true);

	//std::cout << "PACKET # CHANGED\n";
	//std::cout << m_cntrollerState.normLS[0] << ", " << m_cntrollerState.normLS[1] << "\n";
}

void InputComponent::RemoveClients()
{
	using namespace Events;


	EventManager()->UnregisterClient("DirForward", this, &InputComponent::HandleKeyDown);
	EventManager()->UnregisterClient("DirBack", this, &InputComponent::HandleKeyDown);
	EventManager()->UnregisterClient("DirLeft", this, &InputComponent::HandleKeyDown);
	EventManager()->UnregisterClient("DirRight", this, &InputComponent::HandleKeyDown);
	EventManager()->UnregisterClient("PlayerDash", this, &InputComponent::HandleKeyDown);
	EventManager()->UnregisterClient("UpForward", this, &InputComponent::HandleKeyUp);
	EventManager()->UnregisterClient("UpBack", this, &InputComponent::HandleKeyUp);
	EventManager()->UnregisterClient("UpLeft", this, &InputComponent::HandleKeyUp);
	EventManager()->UnregisterClient("UpRight", this, &InputComponent::HandleKeyUp);

}

void InputComponent::ResetVals()
{
	m_flags = 0;
	dashCooldown = 0;
}

void InputComponent::Reset()
{
	m_flags = 0;
	m_bMenuMode = true;
	m_bLastInputGamepad = false;
	lastDT = 0;
	dashTime = 0;
	m_fSpeed = 0;
	dashCooldown = 0;
	bWasSpinning = false;
}