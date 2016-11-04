#pragma once
#include "BaseComponent.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/InputManager.h"
#include "../Input/MessageSystem.h"
#include "../Input/ThreeSixty.h"
class InputComponent :
	public BaseComponent
{

public:
	InputComponent();
	~InputComponent();
	
	void Update(float dt);

	void HandleMovement(float _HorizAxis, float _VertAxis);
	void HandleMovement(char possibleVar);
	
	
	void HandleShoot(float _HorizAxis, float _VertAxis);
	void HandleDash();
	void HandleShoot();
	void HandleEMP();
	void HandleKeyDown(const Events::CGeneralEventArgs2<unsigned char, float>& args);
	void HandleKeyUp(const Events::CGeneralEventArgs<unsigned char>& args);
	void ChangeVelocity(bool _GamePad = false);
	void UpdateGamePad(GamePad* _newInfo);
	void SetupClients();
	void RemoveClients();

	void ResetVals();
	void Reset();

	bool IsMenuMode() const { return m_bMenuMode; }
	void SetMenuMode(bool val) { m_bMenuMode = val; }
private:
	short m_flags;
	bool m_bMenuMode;
	// 0000 up-down-left-right
	GamePad m_cntrollerState;
	enum eDIRECTION { Up = 1, Down = 2, Left = 4, Right = 8 };
	bool m_bLastInputGamepad;
	// stores the deltatime information from earlier, for use in lerping the keyboard controls.
	float lastDT = 0;

	// the remaining dashing time
	float dashTime = 0;
	float m_fSpeed;
	bool bWasSpinning;
	
	float dashCooldown = 0;
};

