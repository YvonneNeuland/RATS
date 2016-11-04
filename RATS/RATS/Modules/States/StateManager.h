#pragma once
#include "BaseState.h"
#include "../Input/InputController.h"
//#include "../BitmapFont/BitmapFontManager.h"
//#include "../Real Messages/MessageHandler.h"
//#include "../Renderer/Renderer.h"

class BitmapFontManager;
class Renderer;
class CameraObject;
#include <vector>
using namespace std;

class StateManager
{
public:
	StateManager();
	~StateManager();

	bool SetState(STATE_TYPE stateType);
	void RemoveState();
	const BaseState* GetCurrentState();

	void ClearStates();
	bool Update(float dt);
	void Initialize(Renderer* renderer);
	void Terminate();

	bool IsTimeSlow() const { return m_bTimeSlow; }
	void SetTimeSlow(bool val) { m_bTimeSlow = val; }

private:
	STATE_TYPE m_CurrentState = STATE_TYPE::SPLASH_SCREEN_STATE;
	vector<BaseState*>m_vStateStack;

	BitmapFontManager* m_BitmapFontManager;
	Renderer* m_Renderer;
	CameraObject* m_Camera;

	STATE_TYPE m_LastState = STATE_TYPE::SPLASH_SCREEN_STATE;

	bool m_bTimeSlow = false;

	// Renderer Memory Dump Helper
	void SetRenderInfosNull();

};

