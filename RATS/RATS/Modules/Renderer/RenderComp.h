#pragma once
#include "../Components/BaseComponent.h"
#include <string>
#include <DirectXMath.h>

class RenderComp :
	public BaseComponent
{
	std::string m_szID;
	eRENDERCOMP_TYPE m_renderType;

	//std::string m_glowID;

	RenderComp(const RenderComp&) = delete;
	RenderComp(const RenderComp&&) = delete;
	RenderComp& operator=(const RenderComp&&) = delete;

public:
	RenderComp();
	~RenderComp();

	void Update(float dt);
	int GetRenderType() { return m_renderType; }
	void SetRenderType(int renderType) { m_renderType = (eRENDERCOMP_TYPE)renderType; }

	std::string GetID() const { return m_szID; }
	void SetID(std::string val) { m_szID = val; }
	void Reset();

	//std::string GetGlowID() const { return m_glowID; }
	//void SetGlowID(std::string val) { m_glowID = val; }

	RenderComp& operator=(const RenderComp& rhs);


	//color is RGBA
	DirectX::XMFLOAT4 color;

	
	//  All 0 to 1 ratios of the screens height and width
	//
	//		(0, 0)________________________(1, 0)
	//			 |						  |
	//		     |						  |
	//		     |         Screen		  |
	//		     |						  |
	//		     |________________________|
	//		(0, 1)						  (1, 1)
	//
	//										-Mark

	float m_posX = 0.0f;
	float m_posY = 0.0f;
	float m_width = 0.0f;
	float m_height = 0.0f;



	//For fonts, width and height are scalar values
	// 1 will be small text
	// 5 will be large

	void* m_renderInfo;
};

