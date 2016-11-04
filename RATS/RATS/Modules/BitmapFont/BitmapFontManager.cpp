#include "stdafx.h"
#include "BitmapFontManager.h"
#include <iostream>


BitmapFontManager::BitmapFontManager()
{
}


BitmapFontManager::~BitmapFontManager()
{
	Terminate();
}

bool BitmapFontManager::LoadFont(string fontName, string fileName)
{
	BitmapFont* newFont = new BitmapFont;

	std::cout << "FONT LOADED\n";

	newFont->SetFontName(fontName);
	if (newFont->LoadFontFile(fileName) && newFont != nullptr)
	{
		m_vLoadedFonts[fontName] = newFont;
		return true;
	}
	else
	{
		delete newFont;
		return false;
	}
	
}

void BitmapFontManager::Terminate(void)
{
	if (m_vLoadedFonts.size())
	{
		//Create an iterator to traverse the LoadedFonts map
		auto iter = m_vLoadedFonts.begin();

		//Kill off all the memory leaks
		for (; iter != m_vLoadedFonts.end(); ++iter)
		{
			/*m_vLoadedFonts[iter->first]->Terminate();
			delete m_vLoadedFonts[iter->first];
			m_vLoadedFonts[iter->first] = nullptr;*/

			iter->second->Terminate();
			delete iter->second;
			iter->second = nullptr;
		}

		m_vLoadedFonts.clear();
	}
}

BitmapFont* BitmapFontManager::GetFont(string fontName)
{
	return m_vLoadedFonts[fontName];
}