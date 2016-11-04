#pragma once
#include "BaseState.h"
#include "../Input/MessageSystem.h"
#include "../Object Manager/GameObject.h"

class SplashScreenState : public BaseState
{
public:
	SplashScreenState();
	~SplashScreenState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

	//Input Functions
	void OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args);

private:


	//Private Functions
	void CreateHud(void);
	void SaveSplashScreenSettings();
	void LoadSplashScreenSettings();

	//Private DataMembers
	BitmapFontManager* m_pBitmapFontManager;
	//bool m_bLogo1Played = false;
	//bool m_bFade1Played = false;
	//bool m_bFade2Played = false;
	//float m_fLogo1Timer = 0.0f;
	//float m_fLogo2Timer = 0.0f;
	GameObject* m_pRatsLogo = nullptr;
	GameObject* m_pHFGLogo = nullptr;
	GameObject* m_pGPGamesLogo = nullptr;

	RenderComp* gpComp = nullptr;
	RenderComp* hfgComp = nullptr;
	RenderComp* ratsComp = nullptr;

	bool firstFade = false;
	bool secondFade = false;

	float timer = 0;

	bool m_bFirstTimePlayed = false;

	//Input Manipulation Variables
	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::MAIN_MENU_STATE;
	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;
};

