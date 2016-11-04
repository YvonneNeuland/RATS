#pragma once
#include "BaseState.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

enum SOUND_TYPE
{
	BGM = 0,
	SFX
};

class SoundOptionState : public BaseState
{
public:

	//Interface Functions
	SoundOptionState();
	~SoundOptionState();

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

private:
	//Private Functions
	void CreateHud(void);
	void UpdateSliderBars(void);
	int CapVolume(int volume);
	void TurnVolumeDown();
	void TurnVolumeUp();
	void SaveSoundSettings();
	void LoadSoundSettings();
	void ResetButtonColors();

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
	STATE_TYPE m_SelectedState = STATE_TYPE::OPTIONS_MENU_STATE;
	SOUND_TYPE m_SelectSoundType = SOUND_TYPE::BGM;
	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;
	float m_fOldMouseXBGM = 0.0f;
	float m_fOldMouseXSFX = 0.0f;

	//Private Data Members
	BitmapFontManager* m_pBitmapFontManager;
	GameObject* m_pMenuBackground			= nullptr;
	GameObject* m_pBGMButton				= nullptr;
	GameObject* m_pBGMButtonSelected		= nullptr;
	GameObject* m_pBGMLeftButton			= nullptr;
	GameObject* m_pBGMRightButton			= nullptr;
	GameObject* m_pBGMSliderBar				= nullptr;
	GameObject* m_pBGMSliderIndicator		= nullptr;

	GameObject* m_pSFXButton				= nullptr;
	GameObject* m_pSFXButtonSelected		= nullptr;
	GameObject* m_pSFXLeftButton			= nullptr;
	GameObject* m_pSFXRightButton			= nullptr;
	GameObject* m_pSFXSliderBar				= nullptr;
	GameObject* m_pSFXSliderIndicator		= nullptr;

	GameObject* m_pBackButton				= nullptr;
	GameObject* m_pBackButtonSelected		= nullptr;
	GameObject* m_pSelectButton				= nullptr;

	GameObject* m_pMenuNavKeyboardInstr		= nullptr;
	GameObject* m_pMenuNavGamepadInstr		= nullptr;

	bool m_bGamepadInstructionsOn = false;

	//Sound Variables
	int m_nBGMVolume = 50;
	int m_SFXVolume = 60;




};

