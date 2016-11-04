#include "stdafx.h"
#include "RenderComp.h"


RenderComp::RenderComp()
{
	m_szID = "";
	m_height = 0.1f;
	m_width = 0.1f;
	m_posX = 0.1f;
	m_posY = 0.1f;
	m_renderInfo = nullptr;

	color = DirectX::XMFLOAT4(1, 1, 1, 1);

}


RenderComp::~RenderComp()
{
}

void RenderComp::Update(float dt)
{
}

RenderComp& RenderComp::operator=(const RenderComp& rhs)
{
	m_posX = rhs.m_posX;
	m_posY = rhs.m_posY;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_renderInfo = rhs.m_renderInfo;

	m_szID = rhs.m_szID;
	m_renderType = rhs.m_renderType;

	// Base Component members
	m_type = rhs.m_type;
	m_pOwner = rhs.m_pOwner;

	return *this;
}

void RenderComp::Reset()
{
	this->color = DirectX::XMFLOAT4(1,1,1,1);
	m_posX = 0;
	m_posY = 0;
	m_width = 0;
	m_height = 0;
	m_renderInfo = nullptr;

	m_szID = "";
	m_renderType = eRENDERCOMP_TYPE::Opaque;

	// Base Component members
	m_type = 0;
	m_pOwner = nullptr;
}