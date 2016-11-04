//--------------------------------------------------------------------------------
// InputManager
// Written By: Justin Murphy
//
// Processes windows messages to keep track of the key states and mouse information
// Note: in order for a key to be updated properly it must be activated first 
// if using events this manager will activate the key for you
//--------------------------------------------------------------------------------
#ifndef InputManager_h
#define InputManager_h
//--------------------------------------------------------------------------------
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
//--------------------------------------------------------------------------------
#include <Windows.h>
#include <vector>
#include "../../Wwise files/Event.h"
#include "IWindProc.h"
//--------------------------------------------------------------------------------
class CInputManager :public IWindProc {
public:
	// default state is KEY_CLEAR. 
	// once pressed it will show up as KEY_PRESS
	// if no key-up message was sent the status
	// will change to KEY_DOWN on the next update
	// key-up message changes to KEY_RELEASE status
	// which will be set to KEY_UP after one update
	enum eKeyStates { KEY_CLEAR = 0, KEY_PRESS, KEY_DOWN, KEY_RELEASE, KEY_UP };
	struct tMouseInput {
		unsigned int x,y,prevX,prevY;
		int m_nScrollPos;
	};
	struct tKey {
		eKeyStates		nKeyState;
		float			fHeldDuration;
		unsigned int	unRefCount;
	};

	// when anykey tracking is enabled an "AnyKeyPressed","AnyKeyDown", or "AnyKeyReleased" 
	// event will be sent with the keycode pressed along with duration for the down event
	bool m_bAnyKeyEnabled;
//public: functions
	~CInputManager();
	bool Init() override;
	// sends events for current key states and then updates the keys for next frame
	// Note: if multiple keys were pressed between the frame the events won't match the order they were pressed.
	void Update(float fDeltaTime) override;
	bool ShutDown() override;
	//static CInputManager* GetInstance();
	//static void CreateInstance();
	//static void DestroyInstance();

	// starts tracking the key
	void ActivateKey(const unsigned char VKcode);
	// activates the key if needed
	// if an event already exists it will return the old event (easier for multiple event handlers)
	Events::EVENTID BindKeyEvent(const unsigned char VKcode, eKeyStates keyState, Events::EVENTID Event);
	Events::EVENTID BindKeyPressEvent(const unsigned char VKcode, Events::EVENTID Event);
	Events::EVENTID BindKeyDownEvent(const unsigned char VKcode, Events::EVENTID Event);
	Events::EVENTID BindKeyReleaseEvent(const unsigned char VKcode, Events::EVENTID Event);
	// will stop throwing the event bound to this keystate
	void UnBindKeyEvent(const unsigned char VKcode, eKeyStates keyState);
	// will unbind all events and no longer track the key
	bool DeActivateKey(const unsigned char VKcode);
	
	inline const tMouseInput& GetMouseInput() const { return m_tMouse; }

	void GetMousePosition(float &_x, float &_y);
	void GetMousePreviousPosition(float &_x, float &_y);

	void EnableAnyKeyTracking() { m_bAnyKeyEnabled = true; }
	void DisableAnyKeyTracking() { m_bAnyKeyEnabled = false; }
	bool GetAnyKeyTrackingEnabled()const { return m_bAnyKeyEnabled; }

	// used for when losing window focus
	void ResetInput();

	// this is not the most efficient way to do things but is very simple for testing purposes
	// Note: the key to be checked must be active in order for this to return an accurate state
	const eKeyStates GetKeyState(const unsigned char VKcode)const;
	const bool GetKeyDown(const unsigned char VKcode)const;
	const bool GetKeyUp(const unsigned char VKcode)const;
	const bool GetKeyRelease(const unsigned char VKcode)const;
	const bool GetKeyPress(const unsigned char VKcode)const;
	CInputManager();
	LRESULT WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) override;
private:
	//static CInputManager* s_pInstance;
	tKey m_Keys[256];
	tMouseInput m_tMouse;
	std::vector<unsigned char> m_ActiveKeys;
	std::vector<Events::EVENTID> m_ActiveKeyPressEvents;
	std::vector<Events::EVENTID> m_ActiveKeyDownEvents;
	std::vector<Events::EVENTID> m_ActiveKeyReleaseEvents;
	// for tracking any key useful for typing or waking up from attract states
	unsigned char	m_ucLastKeyCode;
	tKey			m_LastKeyState;

	// helper functions
	void KeyDown(const unsigned char VKcode);
	void KeyUp(const unsigned char VKcode);
	void ScrollUp() { ++m_tMouse.m_nScrollPos; }
	void ScrollDown() { m_tMouse.m_nScrollPos--; }
	
	

	
	CInputManager(const CInputManager &);
	CInputManager & operator=(const CInputManager &);

	// Need to friend the window class for access to the message loop
	friend class CApplication;
};
//--------------------------------------------------------------------------------
//inline CInputManager* InputManager() {	return CInputManager::GetInstance(); }
//--------------------------------------------------------------------------------
#endif // InputManager_h
//--------------------------------------------------------------------------------