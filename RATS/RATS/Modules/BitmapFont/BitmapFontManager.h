#pragma once

#include "BitmapFont.h"

#include <string>
#include <map>
using namespace std;

class BitmapFontManager
{
public:
	BitmapFontManager();
	~BitmapFontManager();

	bool LoadFont(string fontName, string fileName);
	void Terminate(void);
	BitmapFont* GetFont(string fontName);

private:
	map<string, BitmapFont*> m_vLoadedFonts;
};

