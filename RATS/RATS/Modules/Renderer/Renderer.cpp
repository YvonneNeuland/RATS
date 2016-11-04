#include "stdafx.h"
#include "Renderer.h"



Renderer::Renderer()
{
	m_Graphics = nullptr;
}


Renderer::~Renderer()
{
	//Shutdown();
}


bool Renderer::Initialize(HINSTANCE hInstance)
{
	bool result;

	m_Graphics = new D3DGraphics();

	if (!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(hInstance);

	if (!result)
	{
		//MessageBox(hWnd, L"Could not intialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Renderer::Shutdown()
{
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}
}

bool Renderer::Frame(float deltaTime)
{
	bool result;

	result = Render(deltaTime);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Renderer::Render(float deltaTime)
{

	m_Graphics->Frame(deltaTime);

	return true;
}

//void Renderer::SetRenderList(std::vector<std::vector<std::list<BaseComponent*>>>* list)
//{
//	m_Graphics->SetRenderList(list);
//}

void Renderer::SetRenderList(std::vector<std::vector<std::vector<BaseComponent*>>>* list, std::vector<std::vector<BaseComponent*>>* glowList)
{
	m_Graphics->SetRenderList(list, glowList);
}

void Renderer::SetRenderSizes(std::vector<std::vector<int>> *sizes, std::vector<int> * glowSizes)
{
	m_Graphics->SetRenderSizes(sizes, glowSizes);
}

void Renderer::SetCamera(GameObject* camera)
{
	m_Graphics->SetCamera(camera);
}

void Renderer::SetRenderEmitters(std::list<MoleculeEmitter*> *list)
{
	m_Graphics->SetRenderEmitters(list);
}