#pragma once
#include "BaseState.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

class HowToPlayState : public BaseState
{
public:
	HowToPlayState();
	~HowToPlayState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

	
	//Input Functions
	void OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);

private:
	void CreateHud(void);
	void EnlargeInstructionScreen(GameObject* instructionScreen);
	void ShrinkInstructionScreen(GameObject* instructionScreen);
	void ShrinkAllInstructionScreens();
	void ResetButtonColors();
	void SetObjectColor(GameObject* obj, XMFLOAT4 color);

	void ShrinkAllButtons();
	void GrowSelectedButton(GameObject* obj);

	//Input Helper Functions
	void SetSelectButton(void);
	void SetInstructionScreen(void);
	bool CheckMouseMove();
	bool Handle360Input();
	unsigned int GetCurrentButton(void){ return m_unCurrentButton; }
	void SetCurrentButton(int numButton);
	void SetCurrentInstructionScreen(int numInstructionScreen);

	//Input Manipulation Variables
	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	unsigned int m_unCurrentInstructionScreen = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::HOW_TO_PLAY_STATE;
	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;

	BitmapFontManager* m_pBitmapFontManager;

	//Hud objects
	
	GameObject* m_pInstructions1			= nullptr;
	GameObject* m_pInstructions2			= nullptr;
	GameObject* m_pInstructions3			= nullptr;
	GameObject* m_pInstructions4			= nullptr;
	GameObject* m_pInstructions5			= nullptr;
	GameObject* m_pInstructions6			= nullptr;
	GameObject* m_pInstructions7			= nullptr;
	GameObject* m_pInstructions8			= nullptr;
	GameObject* m_pInstructions9			= nullptr;
	GameObject* m_pNextButton				= nullptr;
	GameObject* m_pPreviousButton			= nullptr;
	GameObject* m_pBackToMainMenu			= nullptr;
	GameObject* m_pNextButtonSelected		= nullptr;
	GameObject* m_pPreviousButtonSelected	= nullptr;
	GameObject* m_pBackToMainMenuSelected	= nullptr;

};

