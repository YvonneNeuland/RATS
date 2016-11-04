#pragma once
#include "../BitmapFont/BitmapFontManager.h"
//#include "../Real Messages/MessageHandler.h"
#include "../Renderer/Renderer.h"
#include "../Object Manager/ObjectManager.h"

enum STATE_TYPE
{
	EXIT_GAME = -1,
	GAME_PLAY_STATE = 0,
	MAIN_MENU_STATE,
	OPTIONS_MENU_STATE,
	HOW_TO_PLAY_STATE,
	LOAD_MENU_STATE,
	SOUND_MENU_STATE,
	GRAPHICS_MENU_STATE,
	KEYBINDINGS_MENU_STATE,
	UPGRADE_SHIP_STATE,
	PAUSE_MENU_STATE,
	RESULT_STATE,
	LEVEL_SELECT_STATE,
	SPLASH_SCREEN_STATE,
	CREDITS_STATE,
	SAVE_MENU_STATE,
	STATE_ENUM_SIZE
};

class BaseState
{
public:
	BaseState() = default;
	virtual ~BaseState() = default;

	virtual void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true) = 0;
	virtual void Exit() = 0;

	virtual void Initialize(Renderer* renderer) = 0;
	virtual void Update(float dt) = 0;
	virtual void Terminate() = 0;	

	Renderer* m_Renderer;
	ObjectFactory m_ObjectFactory;
	ObjectManager m_ObjectManager;

	STATE_TYPE m_NextState = STATE_TYPE::SPLASH_SCREEN_STATE;



	bool m_bTImeSlow = false;

};

