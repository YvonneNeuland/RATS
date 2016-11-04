#pragma once

#include "Modules/Renderer/Renderer.h"

#include "Modules/Input/InputManager.h"
#include "Modules/Input/InputController.h"
//#include "Modules/Input/ObjectRegistry.h"
#include "Modules/Input/IBaseModule.h"
#include "Modules/States/StateManager.h"
#include "Modules/Input/ThreeSixty.h"
#include "Modules/Upgrade System/GameData.h"
#include "XTime.h"

class Game
{
public:
	Game();
	~Game();

	void Initialize(HINSTANCE hInstance);
	bool Run(MSG uMsg);
	void Shutdown();

	void Test(const Events::CGeneralEventArgs<float>& args);
	void LoadSounds();
	
	
	
	//ObjectRegistry* m_objects;

private:

	Renderer* m_Renderer;
	// audio stuff


	float				volume;
	//Adjust Audio Scale
	//NOTE: only do this to get the placeholder bank integrated with your engine,
	//once you are ready to recieve a real soundbank tell the Sound Engineer the scale you are using
	float				AudioScaling;

	StateManager* m_StateManager;

	

};

