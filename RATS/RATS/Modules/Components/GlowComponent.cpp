#include "stdafx.h"
#include "GlowComponent.h"


GlowComponent::GlowComponent()
{
	// No Glow
	m_szGlowID = "";
	m_type = Glow;
}


GlowComponent::~GlowComponent()
{

}

void GlowComponent::Update(float dt)
{
	// Nothing in here to update
}

void GlowComponent::Reset()
{
	m_szGlowID = "";
	m_pOwner = nullptr;
}