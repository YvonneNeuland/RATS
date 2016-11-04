#pragma once
#include "Xinput.h"


class InputComponent;

enum buttonStatus
{
	bUp = 0, bPress, bHolding, bHeld
};

enum stickDirections
{
	sUp = 0, sDown, sLeft, sRight, numDir
};

enum buttonList
{
	DPAD_UP = 0, 
	DPAD_DOWN, 
	DPAD_LEFT, 
	DPAD_RIGHT, 
	START, 
	BACK, 
	LEFT_THUMB, 
	RIGHT_THUMB,
	LEFT_BUMPER, 
	RIGHT_BUMPER, 
	A, 
	B,
	X,
	Y,
	Num_Buttons
};

struct GamePad
{
	XINPUT_STATE state;
	float normLS[2];
	float normRS[2];
	float normTriggers[2];
	float normMag[2];
	buttonStatus buttons[Num_Buttons];
	buttonStatus stickDir[2][numDir];
};

class ThreeSixty
{
	DWORD dwResult;
	XINPUT_STATE m_grunt;
	bool m_bAtLeast1;
	float m_fDCTimer;
	float m_fVibeTimer[4];
	float m_fWaiting;
	int m_numActivePlayers;
	XINPUT_VIBRATION m_vibeReset;
	bool m_bMenuMode;
	float m_arrButtonPressTimers[4][Num_Buttons];
	float m_arrStickTimers[4][2][numDir];
	GamePad m_Controllers[4];
	void NormalizeNewInput(int _whichOne = 0);
	void HandleButtonTiming(int _whichOne = 0);
	void HandleStickTiming(int _whichOne = 0);
	InputComponent* m_vListeners[4];

	

	float m_Lastdt;

public:

	bool bLostCOnnection;

	ThreeSixty();
	~ThreeSixty();

	void Initialize();
	void Update(float _dt);
	void Vibrate(int _whichGamePad, float _time, float _speed, float _otherSpeed = -1.0f);
	void UpdateNumPlayers(int _n) { m_numActivePlayers = _n; }
	void RegisterListnener(int _whichPlayer, InputComponent* _comp);
	void UnregisterListener(int _whichPlayer);
	GamePad GetState(int _whichController = 0);

	bool LostConnection() { return bLostCOnnection; }

};

