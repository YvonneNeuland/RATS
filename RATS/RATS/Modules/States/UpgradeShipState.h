#pragma once
#include "BaseState.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"
#include "../Debug Keys/DebugKeys.h"

class GameObject;

enum UPGRADE_BUTTONS {	WhipDmgDown = 0, WhipDmgUp, WhipSpdDown, WhipSpdUp,					// 0 - 3
						SpreadDmgDown, SpreadDmgUp, SpreadNumDown, SpreadNumUp, 			// 4 - 7
						MissileDmgDown, MissileDmgUp, MissileNumDown, MissileNumUp,			// 8 - 11
						Save_USS, Exit_USS, MAX_UPGRADE_BUTTONS 							// 12 - 14(max)
					};

class UpgradeShipState : 
	public BaseState
{
public:
	UpgradeShipState();
	~UpgradeShipState();


	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

	void CreateHud(void);
	void ResetUpgrades(void);

	// MSG SYS FUNCS
	void OnArrowUp(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowRight(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnArrowLeft(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEnter(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnEscape(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void OnMouseClick(const Events::CGeneralEventArgs2<unsigned char, float>& args);

protected:
	friend class DebugListener;

	// Should have been added to BaseState
	BitmapFontManager* m_pBitmapFontManager;

	DebugListener m_debugListener;

	// Useable Buttons
	std::vector<RenderComp*> m_vButtons;
	
	// Text Displays
	std::string m_DisplayDmgUpgrade;
	std::string m_DisplaySpdUpgrade;
	std::string m_DisplayNumUpgrade;
	std::string m_Count;
	std::string m_UpgradeCost;


	RenderComp* UpCostDisplay;
	RenderComp* WhipDmgDisplay;
	RenderComp* WhipSpdDisplay;
	RenderComp* SpreadDmgDisplay;
	RenderComp* SpreadNumDisplay;
	RenderComp* MissileDmgDisplay;
	RenderComp* MissileNumDisplay;
	RenderComp* NumPoints;

	GameObject* m_pMenuBackground = nullptr;

	RenderComp* starToSpend;

	bool Created = false;
	
	// Gamepad & input things
	float fMouseX, fMouseY, fMousePrevX, fMousePrevY, fCheckTimer, fGamepadTimer;
	DWORD dwPacketNo;
	bool bStickHeld;
	UPGRADE_BUTTONS m_selected_button;

	bool Handle360Input();

	// Functions handling button clicks
	bool CheckButton(int index, float x, float y);
	void HilightButton(int button);
	void ChangeButton(int index);
	
	// Choices
	void DoWorkFunction(int button);

	// Upgrades Functions for Whip
	void UpgradeWhipDmg();
	void DowngradeWhipDmg();
	void UpgradeWhipSpd();
	void DowngradeWhipSpd();

	// Upgrade Functions for Spread
	void UpgradeSpreadDmg();
	void DowngradeSpreadDmg();
	void UpgradeSpreadNum();
	void DowngradeSpreadNum();

	// Upgrade Functions for Missiles
	void UpgradeMissileDmg();
	void DowngradeMissileDmg();
	void UpgradeMissileNum();
	void DowngradeMissileNum();

	// Helper
	void UpdatePoints();
	bool CheckMouseMove();
	bool EnoughPoints(const int&);
	int  CalculatePointsNeded(const int&);
	int  CalculatePointsReturned(const int&);
	void AdjustPoints(const int&);
	void ReturnPoints(const int&);
	int HowManyPoints();

	// Save & Exit
	void SaveUpgrades();
	void ExitState();


	// Temp variables for upgrade stats
	int whip_dmg, whip_spd, spread_dmg, spread_num, missile_dmg, missile_num;
	
	int starting_points = 0;
};

