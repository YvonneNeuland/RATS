#pragma once

#include "BaseState.h"
//#include "../Renderer/RenderComp.h"


#include <string>
#include <vector>

class RenderComp;
class GameObject;

class CreditsState :
	public BaseState
{
public:
	CreditsState();
	~CreditsState();

	void Enter(BitmapFontManager* bitmapFontManager, bool resetSound = true);
	void Exit();

	void Initialize(Renderer* renderer);
	void Update(float dt);
	void Terminate();

private:
	BitmapFontManager* m_pBitmapFontManager;

	std::vector<std::string> m_NameList;
	std::vector<RenderComp*> m_NameRender;
	std::list<std::string> m_ScrollingList;

	float leaveTimer = 0;


	// Will need RenderComps and Strings

	// Test Items
	GameObject *test_object;
	RenderComp *test_comp;
	std::string test_string;

	// Data Members for handling the scrolling

	float m_fNextTextTimer		= 0.0f;

	int m_unNextTextIndex		= 0;

	// Text Manipulators that were going to be their own class
	void InitText(std::string, RenderComp *);
	void ScrollText(float dt);


	void Names();
	void RenderNames();
	void ClearNames();

	// Key Registers
	void OnEnter( const Events::CGeneralEventArgs2<unsigned char, float>& args );
	void OnEscape( const Events::CGeneralEventArgs2<unsigned char, float>& args );
	void OnMouseClick( const Events::CGeneralEventArgs2<unsigned char, float>& args );
};

