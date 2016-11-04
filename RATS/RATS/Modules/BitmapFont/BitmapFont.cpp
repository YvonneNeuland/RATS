#include "stdafx.h"
#include "BitmapFont.h"
#include <cassert>
#include <fstream>
#include "../TinyXml/tinystr.h"
#include "../TinyXml/tinyxml.h"


BitmapFont::BitmapFont()
{
}


BitmapFont::~BitmapFont()
{
}

bool BitmapFont::LoadFontFile(string filePath)
{
	assert(filePath.c_str() != nullptr);

	TiXmlDocument doc;

	if (doc.LoadFile(filePath.c_str()) == false)
	{
		return false;
	}

	TiXmlElement* pFont = doc.RootElement();

	if (pFont == nullptr)
	{
		return false;
	}

	TiXmlElement* pLetters = pFont->FirstChildElement();
	int debug = 0;
	while (pLetters != nullptr)
	{

		if (pLetters == nullptr)
		{
			return false;
		}

		TiXmlElement* pLetter = pLetters->FirstChildElement();

		while (pLetter != nullptr)
		{
			//std::cout << ++debug << std::endl;

			CharacterInfo* letter = new CharacterInfo;
			pLetter->Attribute("id", &letter->id);
			pLetter->Attribute("x", &letter->x);
			pLetter->Attribute("y", &letter->y);
			pLetter->Attribute("width", &letter->width);
			pLetter->Attribute("height", &letter->height);
			pLetter->Attribute("xoffset", &letter->Xoffset);
			pLetter->Attribute("yoffset", &letter->Yoffest);
			pLetter->Attribute("xadvance", &letter->Xadvance);
			pLetter->Attribute("page", &letter->page);
			pLetter->Attribute("chnl", &letter->chnl);

			m_vLoadedLetters[letter->id] = letter;

			pLetter = pLetter->NextSiblingElement();
		}

		pLetters = pLetters->NextSiblingElement();
	}

	return true;
}

void BitmapFont::Terminate(void)
{
	if (m_vLoadedLetters.size())
	{
		//Create an iterator to traverse the LoadedLetters map
		auto iter = m_vLoadedLetters.begin();

		//Kill off all the memory leaks
		for (; iter != m_vLoadedLetters.end(); ++iter)
		{
			delete m_vLoadedLetters[iter->first];
			m_vLoadedLetters[iter->first] = nullptr;
		}

		m_vLoadedLetters.clear();
	}
}

string BitmapFont::GetFontName(void)
{
	return m_szFontName;
}

void BitmapFont::SetFontName(string fontName)
{
	m_szFontName = fontName;
}
