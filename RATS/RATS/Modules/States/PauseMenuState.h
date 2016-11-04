#pragma once
#include "BaseState.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"


class PauseMenuState :	public BaseState
{
public:
	PauseMenuState();
	~PauseMenuState();

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

private:
	BitmapFontManager* m_pBitmapFontManager;

	//Private Functions
	void CreateHud(void);

	//Input Helper Functions
	void SetSelectButton(void);
	bool CheckMouseMove();
	bool Handle360Input();
	void ChangeButton();
	unsigned int GetCurrentButton(void){ return m_unCurrentButton; }
	void SetCurrentButton(int numButton);
	void ResetButtonColors();

	void ShrinkAllButtons();
	void GrowSelectedButton(GameObject* obj);

	//Input Manipulation Variables
	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::MAIN_MENU_STATE;
	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;

	//HUD objects
	GameObject* m_pMenuBackground					= nullptr;
	GameObject* m_pResumeGameButton					= nullptr;
	GameObject* m_pResumeGameButtonSelected			= nullptr;

	GameObject* m_pQuitButton						= nullptr;
	GameObject* m_pQuitButtonSelected				= nullptr;

	GameObject* m_pOptionsButton					= nullptr;
	GameObject* m_pOptionsButtonSelected			= nullptr;

	GameObject* m_pMenuNavKeyboardInstr				= nullptr;
	GameObject* m_pMenuNavGamepadInstr				= nullptr;

	bool m_bGamepadInstructionsOn					= false;

};

