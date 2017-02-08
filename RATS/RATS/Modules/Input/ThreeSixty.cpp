#include "stdafx.h"
#include "ThreeSixty.h"
#pragma comment(lib, "Xinput9_1_0.lib")
#include "../Components/InputComponent.h"
#include "../Upgrade System/GameData.h"
#include "../Renderer/D3DGraphics.h"

extern GameData* gameData;
extern D3DGraphics* globalGraphicsPointer;

ThreeSixty::ThreeSixty()
{
}


ThreeSixty::~ThreeSixty()
{
}

void ThreeSixty::Initialize()
{
	m_numActivePlayers = 1;
	for (int i = 0; i < 4; i++)
	{
		//ZeroMemory(m_Controllers + i, sizeof(GamePad));
		memset(m_Controllers + i, 0, sizeof(GamePad));
		m_vListeners[i] = 0;
		for (int j = 0; j < Num_Buttons; j++)
		{
			m_arrButtonPressTimers[i][j] = 0.0f;
		}
	}
	memset(&m_grunt, 0, sizeof(XINPUT_STATE));
	memset(&m_vibeReset, 0, sizeof(XINPUT_VIBRATION));
	m_fDCTimer = 2.5f;
	memset(m_fVibeTimer, 0, sizeof(float)* 4);
	m_fWaiting = 0;

	dwResult = XInputGetState(0, &m_grunt);

	if (dwResult == ERROR_SUCCESS)
	{
		m_bAtLeast1 = true;
		m_Controllers[0].state = m_grunt;
		std::cout << "360 CONNECTED\n";
		gameData->m_bUsingGamepad = true;
		globalGraphicsPointer->SetCrosshairShowing(false);
	}
	else
	{
		m_bAtLeast1 = false;
		m_fWaiting = m_fDCTimer;
		std::cout << "360 NOT CONNECTED\n";
		gameData->m_bUsingGamepad = false;
	}
	
	bLostCOnnection = false;
}

void ThreeSixty::Update(float _dt)
{
	m_Lastdt = _dt;

	for (int n = 0; n < m_numActivePlayers; n++)
	{
		if (m_fVibeTimer[n] > 0)
		{
			m_fVibeTimer[n] -= _dt;
			if (m_fVibeTimer[n] <= 0)
			{
				m_fVibeTimer[n] = 0;
				XInputSetState(n, &m_vibeReset);
				//std::cout << "360 VIRATE OFF\n";
			}
		}
	}

	// NO CONTROLLERS CONNECTED
	if (!m_bAtLeast1)
	{
		m_fWaiting -= _dt;
		if (m_fWaiting <= 0)
		{
			m_fWaiting = 0;

			dwResult = XInputGetState(0, &m_grunt);

			if (dwResult == ERROR_SUCCESS)
			{
				m_bAtLeast1 = true;
				m_Controllers[0].state = m_grunt;
				NormalizeNewInput();
				std::cout << "360 CONNECTED\n";
				//gameData->m_bUsingGamepad = true;
				bLostCOnnection = false;
			}
			else
			{
				//m_bAtLeast1 = false;
				m_fWaiting = m_fDCTimer;
				//std::cout << "360 NOT FOUND\n";
			}
		}

		return;
	}

	// AT LEAST PLAYER 1 CONNECTED

	for (int i = 0; i < m_numActivePlayers; i++)
	{
		dwResult = XInputGetState(i, &m_grunt);

		if (dwResult == ERROR_SUCCESS)
		{
			//m_bAtLeast1 = true;
			if (m_Controllers[i].state.dwPacketNumber != m_grunt.dwPacketNumber)
			{
				m_Controllers[i].state = m_grunt;
				NormalizeNewInput(i);
				// SEND MESSAGES here?
				if (m_vListeners[i])
				{
					m_vListeners[i]->UpdateGamePad(m_Controllers + i);
				}
			}
			
		}
		else
		{
			//m_bAtLeast1 = false;
			if (i == 0)
			{
				m_bAtLeast1 = false;
				m_fWaiting = m_fDCTimer;
				std::cout << "360 LOST CONNECTion\n";
				gameData->m_bUsingGamepad = false;
				bLostCOnnection = true;
			}
				
		}

		HandleButtonTiming(i);
		HandleStickTiming(i);
	}

}

void ThreeSixty::Vibrate(int _whichGamePad, float _time, float _speed, float _otherSpeed /*= -1.0f*/)
{
	m_fVibeTimer[_whichGamePad] = _time;
	XINPUT_VIBRATION vibration = m_vibeReset;
	//memset(&vibration, 0, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = (WORD)(_speed * 65535);
	if (_otherSpeed != -1.0f)
		vibration.wRightMotorSpeed = (WORD)(_otherSpeed * 65535);
	else
		vibration.wRightMotorSpeed = (WORD)(_speed * 65535);
	XInputSetState(_whichGamePad, &vibration);
}

void ThreeSixty::NormalizeNewInput(int _whichOne)
{
	XINPUT_STATE state = m_Controllers[_whichOne].state;

#pragma region LeftStick
	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;

	//determine how far the controller is pushed
	float magnitude = sqrt(LX*LX + LY*LY);

	//determine the direction the controller is pushed
	float normalizedLX = LX / magnitude;
	float normalizedLY = LY / magnitude;

	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

		m_Controllers[_whichOne].normLS[0] = normalizedLX;
		m_Controllers[_whichOne].normLS[1] = normalizedLY;
		m_Controllers[_whichOne].normMag[0] = normalizedMagnitude;
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
		m_Controllers[_whichOne].normLS[0] = 0;
		m_Controllers[_whichOne].normLS[1] = 0;
		m_Controllers[_whichOne].normMag[0] = 0;
	}

#pragma endregion
	
#pragma region RightStick
	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;

	//determine how far the controller is pushed
	 magnitude = sqrt(RX*RX + RY*RY);

	//determine the direction the controller is pushed
	 normalizedLX = RX / magnitude;
	 normalizedLY = RY / magnitude;

	 normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > 32767) magnitude = 32767;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

		m_Controllers[_whichOne].normRS[0] = normalizedLX;
		m_Controllers[_whichOne].normRS[1] = normalizedLY;
		m_Controllers[_whichOne].normMag[1] = normalizedMagnitude;
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
		m_Controllers[_whichOne].normRS[0] = 0;
		m_Controllers[_whichOne].normRS[1] = 0;
		m_Controllers[_whichOne].normMag[1] = 0;
	}

#pragma endregion


#pragma region Triggers
	float rTrig = state.Gamepad.bRightTrigger;
	float LTrig = state.Gamepad.bLeftTrigger;

	if (LTrig > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		if ( LTrig > INT8_MAX)
			m_Controllers[_whichOne].normTriggers[0] = 1;
		else
			m_Controllers[_whichOne].normTriggers[0] = LTrig / (INT8_MAX - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}
	else
		m_Controllers[_whichOne].normTriggers[0] = 0;

	if (rTrig > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		if (rTrig > INT8_MAX)
			m_Controllers[_whichOne].normTriggers[1] = 1;
		else
			m_Controllers[_whichOne].normTriggers[1] = LTrig / (INT8_MAX - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}
	else
		m_Controllers[_whichOne].normTriggers[1] = 0;


#pragma endregion


}

void ThreeSixty::HandleButtonTiming(int _whichOne /*= 0*/)
{

	XINPUT_STATE state = m_Controllers[_whichOne].state;

#pragma region BUTTON CHECKS

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		if (m_Controllers[_whichOne].buttons[DPAD_UP] == bUp)
		{
			m_Controllers[_whichOne].buttons[DPAD_UP] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_UP] == bPress)
		{
			m_Controllers[_whichOne].buttons[DPAD_UP] = bHolding;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_UP] == bHolding)
		{
			m_arrButtonPressTimers[_whichOne][DPAD_UP] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][DPAD_UP] >= 0.4f)
				m_Controllers[_whichOne].buttons[DPAD_UP] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[DPAD_UP] = bUp;
		m_arrButtonPressTimers[_whichOne][DPAD_UP] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		if (m_Controllers[_whichOne].buttons[DPAD_DOWN] == bUp)
		{
			m_Controllers[_whichOne].buttons[DPAD_DOWN] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_DOWN] == bPress)
		{
			m_Controllers[_whichOne].buttons[DPAD_DOWN] = bHolding;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_DOWN] == bHolding)
		{
			m_arrButtonPressTimers[_whichOne][DPAD_DOWN] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][DPAD_DOWN] >= 0.4f)
				m_Controllers[_whichOne].buttons[DPAD_DOWN] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[DPAD_DOWN] = bUp;
		m_arrButtonPressTimers[_whichOne][DPAD_DOWN] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		if (m_Controllers[_whichOne].buttons[DPAD_LEFT] == bUp)
		{
			m_Controllers[_whichOne].buttons[DPAD_LEFT] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_LEFT] == bPress)
		{
			m_Controllers[_whichOne].buttons[DPAD_LEFT] = bHolding;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_LEFT] == bHolding)
		{
			m_arrButtonPressTimers[_whichOne][DPAD_LEFT] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][DPAD_LEFT] >= 0.4f)
				m_Controllers[_whichOne].buttons[DPAD_LEFT] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[DPAD_LEFT] = bUp;
		m_arrButtonPressTimers[_whichOne][DPAD_LEFT] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		if (m_Controllers[_whichOne].buttons[DPAD_RIGHT] == bUp)
		{
			m_Controllers[_whichOne].buttons[DPAD_RIGHT] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_RIGHT] == bPress)
		{
			m_Controllers[_whichOne].buttons[DPAD_RIGHT] = bHolding;
		}
		else if (m_Controllers[_whichOne].buttons[DPAD_RIGHT] == bHolding)
		{
			m_arrButtonPressTimers[_whichOne][DPAD_RIGHT] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][DPAD_RIGHT] >= 0.4f)
				m_Controllers[_whichOne].buttons[DPAD_RIGHT] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[DPAD_RIGHT] = bUp;
		m_arrButtonPressTimers[_whichOne][DPAD_RIGHT] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
	{
		if (m_Controllers[_whichOne].buttons[START] == bUp)
		{
			m_Controllers[_whichOne].buttons[START] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[START] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][START] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][START] >= 0.2f)
				m_Controllers[_whichOne].buttons[START] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[START] = bUp;
		m_arrButtonPressTimers[_whichOne][START] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
	{
		if (m_Controllers[_whichOne].buttons[BACK] == bUp)
		{
			m_Controllers[_whichOne].buttons[BACK] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[BACK] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][BACK] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][BACK] >= 0.2f)
				m_Controllers[_whichOne].buttons[BACK] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[BACK] = bUp;
		m_arrButtonPressTimers[_whichOne][BACK] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	{
		if (m_Controllers[_whichOne].buttons[LEFT_THUMB] == bUp)
		{
			m_Controllers[_whichOne].buttons[LEFT_THUMB] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[LEFT_THUMB] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][LEFT_THUMB] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][LEFT_THUMB] >= 0.2f)
				m_Controllers[_whichOne].buttons[LEFT_THUMB] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[LEFT_THUMB] = bUp;
		m_arrButtonPressTimers[_whichOne][LEFT_THUMB] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		if (m_Controllers[_whichOne].buttons[RIGHT_THUMB] == bUp)
		{
			m_Controllers[_whichOne].buttons[RIGHT_THUMB] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[RIGHT_THUMB] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][RIGHT_THUMB] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][RIGHT_THUMB] >= 0.2f)
				m_Controllers[_whichOne].buttons[RIGHT_THUMB] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[RIGHT_THUMB] = bUp;
		m_arrButtonPressTimers[_whichOne][RIGHT_THUMB] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		if (m_Controllers[_whichOne].buttons[LEFT_BUMPER] == bUp)
		{
			m_Controllers[_whichOne].buttons[LEFT_BUMPER] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[LEFT_BUMPER] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][LEFT_BUMPER] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][LEFT_BUMPER] >= 0.2f)
				m_Controllers[_whichOne].buttons[LEFT_BUMPER] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[LEFT_BUMPER] = bUp;
		m_arrButtonPressTimers[_whichOne][LEFT_BUMPER] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		if (m_Controllers[_whichOne].buttons[RIGHT_BUMPER] == bUp)
		{
			m_Controllers[_whichOne].buttons[RIGHT_BUMPER] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[RIGHT_BUMPER] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][RIGHT_BUMPER] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][RIGHT_BUMPER] >= 0.2f)
				m_Controllers[_whichOne].buttons[RIGHT_BUMPER] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[RIGHT_BUMPER] = bUp;
		m_arrButtonPressTimers[_whichOne][RIGHT_BUMPER] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		if (m_Controllers[_whichOne].buttons[A] == bUp)
		{
			m_Controllers[_whichOne].buttons[A] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[A] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][A] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][A] >= 0.1f)
				m_Controllers[_whichOne].buttons[A] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[A] = bUp;
		m_arrButtonPressTimers[_whichOne][A] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		if (m_Controllers[_whichOne].buttons[B] == bUp)
		{
			m_Controllers[_whichOne].buttons[B] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[B] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][B] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][B] >= 0.2f)
				m_Controllers[_whichOne].buttons[B] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[B] = bUp;
		m_arrButtonPressTimers[_whichOne][B] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
	{
		if (m_Controllers[_whichOne].buttons[X] == bUp)
		{
			m_Controllers[_whichOne].buttons[X] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[X] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][X] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][X] >= 0.2f)
				m_Controllers[_whichOne].buttons[X] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[X] = bUp;
		m_arrButtonPressTimers[_whichOne][X] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
	{
		if (m_Controllers[_whichOne].buttons[Y] == bUp)
		{
			m_Controllers[_whichOne].buttons[Y] = bPress;
		}
		else if (m_Controllers[_whichOne].buttons[Y] == bPress)
		{
			m_arrButtonPressTimers[_whichOne][Y] += m_Lastdt;
			if (m_arrButtonPressTimers[_whichOne][Y] >= 0.2f)
				m_Controllers[_whichOne].buttons[Y] = bHeld;
		}

	}
	else
	{
		m_Controllers[_whichOne].buttons[Y] = bUp;
		m_arrButtonPressTimers[_whichOne][Y] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////


#pragma endregion
}

void ThreeSixty::RegisterListnener(int _whichPlayer, InputComponent* _comp)
{
	m_vListeners[_whichPlayer] = _comp;
}

void ThreeSixty::UnregisterListener(int _whichPlayer)
{
	m_vListeners[_whichPlayer] = 0;
}

GamePad ThreeSixty::GetState(int _whichController /*= 0*/)
{
	return m_Controllers[_whichController];
}

void ThreeSixty::HandleStickTiming(int _whichOne /*= 0*/)
{
	XINPUT_STATE state = m_Controllers[_whichOne].state;

	// LEFT STICK
	if (m_Controllers[_whichOne].normMag[0] > 0.75f)
	{
		// DIR LEFT
		if (m_Controllers[_whichOne].normLS[0] < -0.5f)
		{
			if (m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] == bUp)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] = bPress;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] == bPress)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] = bHolding;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] == bHolding)
			{
				m_arrStickTimers[_whichOne][0][stickDirections::sLeft] += m_Lastdt;
				if (m_arrStickTimers[_whichOne][0][stickDirections::sLeft] >= 0.4f)
				{
					m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] = bHeld;
				}
			}
		}
		else
		{
			m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] = bUp;
			m_arrStickTimers[_whichOne][0][stickDirections::sLeft] = 0;
		}

		//////////////////////////////////////////////////////////////////////////

		// DIR RIGHT
		if (m_Controllers[_whichOne].normLS[0] > 0.5f)
		{
			if (m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] == bUp)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] = bPress;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] == bPress)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] = bHolding;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] == bHolding)
			{
				m_arrStickTimers[_whichOne][0][stickDirections::sRight] += m_Lastdt;
				if (m_arrStickTimers[_whichOne][0][stickDirections::sRight] >= 0.4f)
				{
					m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] = bHeld;
				}
			}
		}
		else
		{
			m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] = bUp;
			m_arrStickTimers[_whichOne][0][stickDirections::sRight] = 0;
		}

		//////////////////////////////////////////////////////////////////////////

		// DIR down
		if (m_Controllers[_whichOne].normLS[1] < -0.5f)
		{
			if (m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] == bUp)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] = bPress;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] == bPress)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] = bHolding;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] == bHolding)
			{
				m_arrStickTimers[_whichOne][0][stickDirections::sDown] += m_Lastdt;
				if (m_arrStickTimers[_whichOne][0][stickDirections::sDown] >= 0.4f)
				{
					m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] = bHeld;
				}
			}
		}
		else
		{
			m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] = bUp;
			m_arrStickTimers[_whichOne][0][stickDirections::sDown] = 0;
		}

		//////////////////////////////////////////////////////////////////////////

		// DIR UP
		if (m_Controllers[_whichOne].normLS[1] > 0.5f)
		{
			if (m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] == bUp)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] = bPress;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] == bPress)
			{
				m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] = bHolding;
			}
			else if (m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] == bHolding)
			{
				m_arrStickTimers[_whichOne][0][stickDirections::sUp] += m_Lastdt;
				if (m_arrStickTimers[_whichOne][0][stickDirections::sUp] >= 0.4f)
				{
					m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] = bHeld;
				}
			}
		}
		else
		{
			m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] = bUp;
			m_arrStickTimers[_whichOne][0][stickDirections::sUp] = 0;
		}
	}
	else
	{
		m_Controllers[_whichOne].stickDir[0][stickDirections::sUp] = bUp;
		m_arrStickTimers[_whichOne][0][stickDirections::sUp] = 0;
		m_Controllers[_whichOne].stickDir[0][stickDirections::sDown] = bUp;
		m_arrStickTimers[_whichOne][0][stickDirections::sDown] = 0;
		m_Controllers[_whichOne].stickDir[0][stickDirections::sLeft] = bUp;
		m_arrStickTimers[_whichOne][0][stickDirections::sLeft] = 0;
		m_Controllers[_whichOne].stickDir[0][stickDirections::sRight] = bUp;
		m_arrStickTimers[_whichOne][0][stickDirections::sRight] = 0;
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

// RIGHT STICK
if (m_Controllers[_whichOne].normMag[1] > 0.75f)
{
	// DIR LEFT
	if (m_Controllers[_whichOne].normRS[0] < -0.5f)
	{
		if (m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] == bUp)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] = bPress;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] == bPress)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] = bHolding;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] == bHolding)
		{
			m_arrStickTimers[_whichOne][1][stickDirections::sLeft] += m_Lastdt;
			if (m_arrStickTimers[_whichOne][1][stickDirections::sLeft] >= 0.4f)
			{
				m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] = bHeld;
			}
		}
	}
	else
	{
		m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] = bUp;
		m_arrStickTimers[_whichOne][1][stickDirections::sLeft] = 0;
	}

	//////////////////////////////////////////////////////////////////////////

	// DIR RIGHT
	if (m_Controllers[_whichOne].normRS[0] > 0.5f)
	{
		if (m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] == bUp)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] = bPress;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] == bPress)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] = bHolding;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] == bHolding)
		{
			m_arrStickTimers[_whichOne][1][stickDirections::sRight] += m_Lastdt;
			if (m_arrStickTimers[_whichOne][1][stickDirections::sRight] >= 0.4f)
			{
				m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] = bHeld;
			}
		}
	}
	else
	{
		m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] = bUp;
		m_arrStickTimers[_whichOne][1][stickDirections::sRight] = 0;
	}

	//////////////////////////////////////////////////////////////////////////

	// DIR down
	if (m_Controllers[_whichOne].normRS[1] < -0.5f)
	{
		if (m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] == bUp)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] = bPress;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] == bPress)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] = bHolding;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] == bHolding)
		{
			m_arrStickTimers[_whichOne][1][stickDirections::sDown] += m_Lastdt;
			if (m_arrStickTimers[_whichOne][1][stickDirections::sDown] >= 0.4f)
			{
				m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] = bHeld;
			}
			
		}
	}
	else
	{
		m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] = bUp;
		m_arrStickTimers[_whichOne][1][stickDirections::sDown] = 0;
	}

	//////////////////////////////////////////////////////////////////////////

	// DIR UP
	if (m_Controllers[_whichOne].normRS[1] > 0.5f)
	{
		if (m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] == bUp)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] = bPress;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] == bPress)
		{
			m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] = bHolding;
		}
		else if (m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] == bHolding)
		{
			m_arrStickTimers[_whichOne][1][stickDirections::sUp] += m_Lastdt;
			if (m_arrStickTimers[_whichOne][1][stickDirections::sUp] >= 0.4f)
			{
				m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] = bHeld;
			}
		}
	}
	else
	{
		m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] = bUp;
		m_arrStickTimers[_whichOne][1][stickDirections::sUp] = 0;
	}
}
else
{
	m_Controllers[_whichOne].stickDir[1][stickDirections::sUp] = bUp;
	m_arrStickTimers[_whichOne][1][stickDirections::sUp] = 0;
	m_Controllers[_whichOne].stickDir[1][stickDirections::sDown] = bUp;
	m_arrStickTimers[_whichOne][1][stickDirections::sDown] = 0;
	m_Controllers[_whichOne].stickDir[1][stickDirections::sLeft] = bUp;
	m_arrStickTimers[_whichOne][1][stickDirections::sLeft] = 0;
	m_Controllers[_whichOne].stickDir[1][stickDirections::sRight] = bUp;
	m_arrStickTimers[_whichOne][1][stickDirections::sRight] = 0;
}

}


