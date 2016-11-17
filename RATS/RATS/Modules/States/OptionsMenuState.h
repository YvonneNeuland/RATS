#pragma once
#include "BaseState.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"
#include "../Debug Keys/DebugKeys.h"

class OptionsMenuState : public BaseState
{
public:
	//Interface Functions
	OptionsMenuState();
	~OptionsMenuState();

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
	//Private Functions
	void CreateHud(void);

	//Input Helper Functions
	void SetSelectButton(void);
	bool CheckMouseMove();
	bool Handle360Input();
	unsigned int GetCurrentButton(void){ return m_unCurrentButton; }
	void SetCurrentButton(int numButton);


	void ShrinkAllButtons();
	void GrowSelectedButton(GameObject* obj);

	//Input Manipulation Variables
	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::MAIN_MENU_STATE;
	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;

	//Debug Listener
	DebugListener m_debugListener;


	//Private Data Members
	BitmapFontManager* m_pBitmapFontManager;

	GameObject* m_pMenuBackground					= nullptr;
	GameObject* m_SoundOptionsButton				= nullptr;
	GameObject* m_GraphicsOptionsButton				= nullptr;
	GameObject* m_KeybindingsOptionsButton			= nullptr;
	GameObject* m_pCreditsButton					= nullptr;
	GameObject* m_pCreditsSelectedButton			= nullptr;
	GameObject* m_BackToMainMenuButton				= nullptr;
	GameObject* m_SelectButton						= nullptr;
	GameObject* m_pMenuNavKeyboardInstr				= nullptr;
	GameObject* m_pMenuNavGamepadInstr				= nullptr;
	GameObject* m_SoundOptionsButtonSelected		= nullptr;
	GameObject* m_GraphicsOptionsButtonSelected		= nullptr;
	GameObject* m_KeybindingsOptionsButtonSelected	= nullptr;
	GameObject* m_BackToMainMenuButtonSelected		= nullptr;

	bool m_bGamepadInstructionsOn = false;
	
};

