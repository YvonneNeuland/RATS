#pragma once
#include "BaseState.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

class LoadMenuState : public BaseState
{
public:
	LoadMenuState();
	~LoadMenuState();

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
	void SetCurrentButton(int numButton);
	unsigned int GetCurrentButton(void){ return m_unCurrentButton; }

	//Input Manipulation Variables
	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::LOAD_MENU_STATE;
	float fMouseX, fMouseY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;

	//Private Data Members
	BitmapFontManager* m_pBitmapFontManager;

	GameObject* m_pSlot1Button;
	GameObject* m_pSlot2Button;
	GameObject* m_pSlot3Button;

	GameObject* m_pBackButton;
	GameObject* m_pSelectButton;
};

