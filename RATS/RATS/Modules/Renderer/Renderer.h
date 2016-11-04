#pragma once

#include <windows.h>
#include "D3DGraphics.h"

class MoleculeEmitter;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Initialize(HINSTANCE hInstance);
	void Shutdown();
	bool Frame(float deltaTime);
	//void SetRenderList(std::vector<std::vector<std::list<BaseComponent*>>>* list);
	void SetRenderList(std::vector<std::vector<std::vector<BaseComponent*>>>* list, std::vector<std::vector<BaseComponent*>>* glowList);
	void SetRenderSizes(std::vector<std::vector<int>> *sizes, std::vector<int> * glowSizes);
	void SetRenderEmitters(std::list<MoleculeEmitter*> *arr);
	void SetCamera(GameObject* camera);

private:
	bool Render(float deltaTime);

	D3DGraphics* m_Graphics;
	HWND hWnd;
};

