#pragma once
#include "BaseState.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

class KeybindingsOptionsState : public BaseState
{
public:
	//Interface Functions
	KeybindingsOptionsState();
	~KeybindingsOptionsState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

	//Input Functions
	void OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args);

private:
	//Private Functions
	void CreateHud(void);
	void ShrinkAllButtons();
	void GrowSelectedButton(GameObject* obj);
	void SetInputMode(int index);
	void ChangeStringText(GameObject* obj, string newString);
	//Input Helper Functions
	void SetSelectButton(void);
	bool CheckMouseMove();
	bool Handle360Input();
	unsigned int GetCurrentButton(void){ return m_unCurrentButton; }
	void SetCurrentButton(int numButton);

	//Input Manipulation Variables
	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::MAIN_MENU_STATE;
	float fMouseX, fMouseY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;


	//Private Data Members
	BitmapFontManager* m_pBitmapFontManager;

	GameObject* m_pMenuBackground			= nullptr;

	GameObject* m_pBackButton				= nullptr;
	GameObject* m_pBackButtonSelected		= nullptr;

	GameObject* m_pInputButton				= nullptr;
	GameObject* m_pInputButtonSelected		= nullptr;
	GameObject* m_pInputLeftArrow			= nullptr;
	GameObject* m_pInputRightArrow			= nullptr;
	GameObject* m_pInput					= nullptr;

	string keyboardMode = "Keyboard";
	string gamepadMode = "Gamepad";
	int m_nInputIndex						= 0;
};

