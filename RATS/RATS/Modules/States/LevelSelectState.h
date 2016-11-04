#pragma once

#include "BaseState.h"
#include "../Level Manager/LevelManager.h"
#include "../Debug Keys/DebugKeys.h"
// NOTE: If you update this, you should also update
// the levelButtonIDs array inside of InitHUD();
// and also the levelShapes array insode of EnterLevel();


struct LevelButton
{
	GameObject* m_pButton			= nullptr;
	GameObject* m_pStar1			= nullptr;
	GameObject* m_pStar2			= nullptr;
	GameObject* m_pStar3			= nullptr;
	GameObject* m_pButtonText		= nullptr;
	GameObject* m_pLock				= nullptr;
	GameObject* m_pShapeIcon		= nullptr;

	bool m_bLevelUnlocked			= true;
	bool m_bStar1Earned				= true;
	bool m_bStar2earned				= false;
	bool m_bStar3earned				= false;

	unsigned int m_unRow			= 0;
	unsigned int m_unCol			= 0;
	unsigned int m_unLevelNumber	= 0;

};

class LevelSelectState : public BaseState
{
public:
	LevelSelectState();
	~LevelSelectState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();


	void OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args);

private:

	void CreateHUD();
	bool FindButton(float xPos, float yPos);
	bool Handle360Input();

	void EnterLevel(unsigned int level);
	void ExitMenu();
	bool CheckMouseMove();
	void SwitchButton(unsigned int row, unsigned int col);
	void SetObjectColor(GameObject* obj, XMFLOAT4 color);
	void ResetButtonColors();
	void SetRow(int newRow);
	void SetCol(int newCol);
	void UpdateButtons();
	RenderComp* GetButtonRenderComp(unsigned int row, unsigned int col);
	void ShrinkButton(GameObject* obj);
	void GrowButton(GameObject* obj, float height, float width);
	void MoveButton(GameObject* obj, float x, float y);

	// allan pls add to BaseState
	BitmapFontManager* m_pBitmapFontManager;

	// Buttons
	std::vector<std::vector<LevelButton>> m_vButtons;
	
	unsigned int m_unLastLevelPlayed	= 0;
	unsigned int m_unSelectedLevel		= 0;
	unsigned int m_unCurrentRow			= 0;
	unsigned int m_unCurrentCol			= 0;
	unsigned int m_unLastRow			= 0;
	unsigned int m_unLastCol			= 0;
	unsigned int m_unTotalNumRows		= 4;
	unsigned int m_unTotalNumCols		= 6;
	unsigned int m_unNumLevels			= 0;
	LevelManager m_LevelManager;
	GameObject* m_pMenuBackground		= nullptr;
	GameObject* m_pBackButton			= nullptr;
	GameObject* m_pBackButtonSelected	= nullptr;
	GameObject* m_pSelectedButton		= nullptr;

	DebugListener m_debugListener;

	float m_fGamepadTimer;

	DWORD m_ulCurrPacketNo = 0;

	bool m_bStickHeld = false;

	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;

	// TODO: dividing levels into sections?

};