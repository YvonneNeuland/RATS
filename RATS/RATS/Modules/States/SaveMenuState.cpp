#include "stdafx.h"
#include "SaveMenuState.h"


SaveMenuState::SaveMenuState()
{
}


SaveMenuState::~SaveMenuState()
{
}

void SaveMenuState::Enter(BitmapFontManager* bitmapFontManager, bool resetSound)
{
	m_Renderer->SetRenderList(m_ObjectFactory.GetRenderBuckets(), m_ObjectFactory.GetGlowBuckets());
	m_Renderer->SetRenderSizes(m_ObjectFactory.GetRenderSizes(), m_ObjectFactory.GetGlowSizes());
}

void SaveMenuState::Exit()
{

}

void SaveMenuState::Initialize(Renderer* renderer)
{

}

void SaveMenuState::Update(float dt)
{

}

void SaveMenuState::Terminate()
{

}
