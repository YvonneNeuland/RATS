#pragma once
#include "BaseState.h"
#include "../../Wwise files/EventManager.h"
#include "../Input/MessageSystem.h"

class Renderer;
class BitmapFontManager;

class ResultState :
	public BaseState
{
public:
	ResultState();
	~ResultState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();


protected:
	
	//////////
	// KEYS //
	//////////

	BitmapFontManager* m_pBitmapFontManager;
	
	void OnMouseClick( const Events::CGeneralEventArgs2<unsigned char, float>& args );
	void OnEnter( const Events::CGeneralEventArgs2<unsigned char, float>& args );
	void OnEscape( const Events::CGeneralEventArgs2<unsigned char, float>& args );

	/////////////
	// Strings //
	/////////////
	std::string m_title;
	std::string m_score_needed;
	std::string m_score_total;
	std::string m_time_needed;
	std::string m_time_total;
	std::string m_level_passed;
	std::string m_upgrades_earned;

	RenderComp *Title;
	RenderComp *ScoreNeeded;
	RenderComp *TimeNeeded;
	RenderComp *LevelStatus;

	GameObject* scoreStarDead;
	GameObject* scoreStarAlive;

	GameObject* timeStarDead;
	GameObject* timeStarAlive;

	GameObject* resultStarDead;
	GameObject* resultStarAlive;


	GameObject* m_pMenuBackground;
	GameObject* m_score;
	GameObject* m_time;
	GameObject* m_levelComplete;


	float time = 240.0f;
	int score = 5000;
	int upgrades = 0;

	void CreateHud();
	void ShowHud();
	void ShowDeadHud();
	void ShowStrings();
	void Handle360Input();
	void NextScreen();


};

