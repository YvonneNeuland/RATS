#pragma once

#include <string>
#include <unordered_map>
using namespace std;

struct CharacterInfo
{
	int id;
	int x, y;
	int width, height;
	int Xoffset, Yoffest;
	int Xadvance;
	int page;
	int chnl;

	CharacterInfo() : x(0), y(0), width(0), height(0), Xoffset(0), Yoffest(0), Xadvance(0){}
};

class BitmapFont
{
public:
	BitmapFont();
	~BitmapFont();

	bool LoadFontFile(string filePath);
	void Terminate(void);
	string GetFontName(void);
	void SetFontName(string fontName);

	unordered_map<int, CharacterInfo*> m_vLoadedLetters;


private:
	string m_szFontName;
	
};

