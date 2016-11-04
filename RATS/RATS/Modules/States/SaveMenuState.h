#pragma once
#include "BaseState.h"
class SaveMenuState :
	public BaseState
{
public:
	SaveMenuState();
	~SaveMenuState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

private:

	BitmapFontManager* m_pBitmapFontManager;
};

