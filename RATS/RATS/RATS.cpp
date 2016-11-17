// RATS.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "RATS.h"
#include "Game.h"
#include <ctime>

#include "Modules/Input/InputController.h"
#include "Modules/Input/InputManager.h"
#include "Modules/Audio/FFmod.h"
#include "XTime.h"
#include "Modules/Upgrade System/GameData.h"
#include "Modules/Achievements/AchManager.h"
//#include <vld.h>
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
//#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "Log Files\Log.h"

GameData *gameData;
Game gameApp;
D3DGraphics* globalGraphicsPointer = 0;
ThreeSixty* gamePad = 0;
FFmod*	g_AudioSystem = 0;
CAchManager* g_SteamAchievements = 0;
InputController* m_controller = 0;
CInputManager* m_inputMGR = 0;
XTime* m_Xtime = 0;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool HasMessages();
bool MessagePump();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int){


	//LogSetUp(L"RATS");

#ifdef _DEBUG


	//This makes the console window
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG



	MSG msg;
	srand((unsigned int)(time(0)));
	gameApp.Initialize(hInstance);

	// Main message loop:
	ZeroMemory(&msg, sizeof(msg));

	bool running = true;

	while (running)
	{
		while (HasMessages())
		{
			if (!MessagePump())
			{
				running = false;
				break;
			}

		}
		if (!gameApp.Run(msg))
			break;
			
	}

	gameApp.Shutdown();

	//LogShutDown();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//if (GetAsyncKeyState(VK_ESCAPE))
	//	message = WM_DESTROY;

	if (!m_inputMGR->WindowProc(hWnd, message, wParam, lParam))
		return false;

	switch (message)
	{
	case (WM_DESTROY) :
	{
		PostQuitMessage(0);
		break;
	}

	case (WM_SIZE) :
	{
		if (globalGraphicsPointer)
		{
			globalGraphicsPointer->HandleResize();
		}

		break;
	}

	case (WM_SYSKEYDOWN) :
	{

		if (wParam == VK_RETURN)
		{
			std::cout << "FULL SCREEN TOGGLED\n";

			globalGraphicsPointer->ToggleFullscreen();
		}
		//if (wParam == VK_F1)
		//{
		//	std::cout << "TRIGGERED NEXT RESOLUTION\n";

		//	globalGraphicsPointer->NextResolution();
		//}
		if (wParam == VK_F2)
		{
			std::cout << "BORDERS TOGGLED\n";

			globalGraphicsPointer->ToggleBorders();
		}
		break;
	}

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool HasMessages()
{
	MSG msg;
	if (PeekMessage(&msg, 0, 0u, 0u, PM_NOREMOVE) == 0)
		return false;

	return true;

}

bool MessagePump()
{
	MSG msg;
	PeekMessage(&msg, 0, 0u, 0u, PM_REMOVE);

	if (WM_QUIT == msg.message)
		return false;

	TranslateMessage(&msg);
	DispatchMessage(&msg);

	return true;
}