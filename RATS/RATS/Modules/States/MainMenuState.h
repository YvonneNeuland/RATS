#pragma once
#include "BaseState.h"
//#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

class GameObject;

class MainMenuState :
	public BaseState
{
public:
	MainMenuState();
	~MainMenuState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

	
	unsigned int GetCurrentButton(void){ return m_unCurrentButton; }
	void SetCurrentButton(int numButton);

	// MSG SYS FUNCS
	void OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args);

private:
	void CreateHud(void);
	void SetSelectButton(void);
	bool CheckMouseMove();
	bool Handle360Input();


	void ShrinkAllButtons();
	void GrowSelectedButton(GameObject* obj);

	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;
	float fStickHeldTimer;
	//Events::CGeneralEventArgs2<unsigned char, float> tmpArgs;

	BitmapFontManager* m_pBitmapFontManager;

	GameObject* m_pMenuBackground			= nullptr;
	GameObject* m_NewGameButton				= nullptr;
	GameObject* m_NewGameButtonSelected		= nullptr;
	GameObject* m_HowToPlayButton			= nullptr;
	GameObject* m_HowToPlayButtonSelected	= nullptr;
	GameObject* m_QuitButton				= nullptr;
	GameObject* m_QuitButtonSelected		 = nullptr;
	GameObject* m_LoadGameButton			= nullptr;
	GameObject* m_LoadGameButtonSelected	= nullptr;
	GameObject* m_OptionsButton				= nullptr;
	GameObject* m_OptionsButtonSelected		= nullptr;

	GameObject* m_SelectButton				= nullptr;
	GameObject* m_pMenuNavKeyboardInstr		= nullptr;
	GameObject* m_pMenuNavGamepadInstr		= nullptr;

	GameObject* m_whiteSquare = nullptr;

	GameObject* m_gameName = nullptr;


	bool m_bGamepadInstructionsOn			= false;

	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::GAME_PLAY_STATE;
};

