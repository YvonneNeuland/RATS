#pragma once
#include "BaseState.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

enum GRAPHIC_MODE
{
	NO_MODE = 0,
	SCREEN_RESOLUTION,
	WINDOW_MODE,
	MSAA,
	VSYNC,
	GAMMA
};

class GraphicsOptionsState : public BaseState
{
public:

	//Interface Functions
	GraphicsOptionsState();
	~GraphicsOptionsState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

	//Input Functions
	void OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);

	void SaveGraphicsSettings();
	bool LoadGraphicsSettings();

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
	void ResetButtonColors();
	void SetObjectColor(GameObject* obj, XMFLOAT4 color);
	void SetResolution(int index);
	void SetWindowMode(int index);
	void SetMSAALevel(int index);
	void SetVSync(int index);
	void SetKey(int index);
	void ChangeStringText(GameObject* obj, string newString);
	void TurnGammaUp(void);
	void TurnGammaDown(void);
	int CapGamma(int gamma);
	void UpdateSliderBar(void);

	//Input Manipulation Variables
	vector<int> m_vButtons;
	unsigned int m_unCurrentButton = 0;
	STATE_TYPE m_SelectedState = STATE_TYPE::GRAPHICS_MENU_STATE;
	float fMouseX, fMouseY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;


	//Private Data Members
	BitmapFontManager* m_pBitmapFontManager			= nullptr;

	GameObject* m_pMenuBackground					= nullptr;

	GameObject* m_pResolutionButton					= nullptr;
	GameObject* m_pResolutionButtonSelected			= nullptr;
	GameObject* m_pResolutionLeftArrow				= nullptr;
	GameObject* m_pResolutionRightArrow				= nullptr;
	GameObject* m_pResolution						= nullptr;
	int m_nResolutionIndex							= 0;

	GameObject* m_pWindowModeButton					= nullptr;
	GameObject* m_pWindowModeButtonSelected			= nullptr;
	GameObject* m_pWindowModeLeftArrow				= nullptr;
	GameObject* m_pWindowModeRightArrow				= nullptr;
	GameObject* m_pWindowMode						= nullptr;
	int m_nWindowModeIndex							= 0;

	GameObject* m_pMSAALevelButton					= nullptr;
	GameObject* m_pMSAALevelButtonSelected			= nullptr;
	GameObject* m_pMSAALevelLeftArrow				= nullptr;
	GameObject* m_pMSAALevelRightArrow				= nullptr;
	GameObject* m_pMSAALevel						= nullptr;
	int m_nMSAALevelIndex							= 0;

	GameObject* m_pVSyncButton						= nullptr;
	GameObject* m_pVSyncButtonSelected				= nullptr;
	GameObject* m_pVSyncLeftArrow					= nullptr;
	GameObject* m_pVSyncRightArrow					= nullptr;
	GameObject* m_pVSync							= nullptr;
	int m_nVSyncIndex								= 0;

	GameObject* m_pBackButton						= nullptr;
	GameObject* m_pApplyButton						= nullptr;
	GameObject* m_pBackButtonSelected				= nullptr;
	GameObject* m_pApplyButtonSelected				= nullptr;
	int m_nKeyIndex									= 0;

	GameObject* m_pGammaButton						= nullptr;
	GameObject* m_pGammaButtonSelected				= nullptr;
	GameObject* m_pGammaLeftArrow					= nullptr;
	GameObject* m_pGammaRightArrow					= nullptr;
	GameObject* m_pGammaSliderBar					= nullptr;
	GameObject* m_pGammaSliderBarIndicator			= nullptr;
	int m_nGamma									= 50;
	float m_fGamma									= 0.5f;
	float m_fOldMouseXGamma							= 0.0f;

	GRAPHIC_MODE currentMode						= GRAPHIC_MODE::SCREEN_RESOLUTION;
};

