#include "stdafx.h"
#include "D3DGraphics.h"
#include <fstream>
#include <algorithm>

#include "../../Resource.h"

#include "Shaders\Generic_PS.csh"
#include "Shaders\Generic_VS.csh"
#include "Shaders\InstancedGeneric_PS.csh"
#include "Shaders\InstancedGeneric_VS.csh"
#include "Shaders\InstancedBillboard_VS.csh"
#include "Shaders\Billboard_VS.csh"
#include "Shaders\ParticleBillboardZ_VS.csh"
#include "Shaders\Skybox_PS.csh"
#include "Shaders\Skybox_VS.csh"
#include "Shaders\TwoD_PS.csh"
#include "Shaders\Healthbar_PS.csh"
#include "Shaders\TwoD_VS.csh"
#include "Shaders\Sphere_PS.csh"
#include "Shaders\DistortionWave_PS.csh"
#include "Shaders\Fade_PS.csh"
#include "Shaders\ScrollingUV_PS.csh"
#include "Shaders\Bullet_PS.csh"
#include "Shaders\Line_PS.csh"
#include "Shaders\Particle_PS.csh"
#include "Shaders\Particle_VS.csh"
#include "Shaders\InitParticle_CS.csh"
#include "Shaders\PreParticleInit.csh"
#include "Shaders\Particle_CS.csh"
#include "Shaders\Ribbon_VS.csh"
#include "Shaders\Ribbon_PS.csh"
#include "Shaders\Ribbon_GS.csh"
#include "Shaders\InstancedScale_VS.csh"
#include "Shaders\InstancedWhite_PS.csh"

#include "../../XTime.h"


#include "../Object Manager/GameObject.h"
#include "../VFX/ElectricityEffect/ElectricityEffect.h"
#include "../VFX/AnimTexture/AnimTexture.h"

// Grants Testing Includes
#include "../Object Manager/ObjectFactory.h"

#include "../Molecules/Molecule.h"

#include "../Components/GlowComponent.h"



#ifdef _DEBUG
#include "../../Log Files/DXLog.h"
#else
#define DXCall 
#define DXDrawName 
#define DXName 
#define DXNameInit 
#define DXMemoryCheck 
#endif

#define SafeRelease(x)			if(x) x->Release(); x=nullptr
#define SafeDelete(x)			if(x) delete x; x=nullptr
#define SafeDeleteArray(x)		if(x) delete[] x; x=nullptr

extern D3DGraphics* globalGraphicsPointer;
extern XTime* m_Xtime;

using namespace DirectX;
using namespace std;

//Referenced http://www.rastertek.com/, http://www.steelskies.com/HLSL2.htm, and Fuz provided by Jim/James

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);


D3DGraphics::D3DGraphics()
{

//#ifdef _DEBUG
//
//	SCREEN_WIDTH = 1024;
//	SCREEN_HEIGHT = 768;
//	FULL_SCREEN = false;
//
//#endif

	SCREEN_WIDTH = 1024;
	SCREEN_HEIGHT = 768;
	FULL_SCREEN = false;

	/////////////////////
	//UNIVERSAL OBJECTS//
	/////////////////////

	m_context = nullptr;
	m_chain = nullptr;
	m_device = nullptr;

	m_rtv = nullptr;
	m_depthBuffer = nullptr;
	m_depthView = nullptr;

	m_sampleStateClampY = nullptr;
	m_sampleStateWrap = nullptr;
	//	m_additiveBlendState = nullptr;
	m_alphaBlendState = nullptr;

	m_2dDepthState = nullptr;
	m_3dDepthState = nullptr;
	m_transparentDepthState = nullptr;

	m_rastStateCullBack = nullptr;
	m_rastStateCullFront = nullptr;
	m_rastStateCullNone = nullptr;

	m_instanceBuffer = nullptr;
	m_particleInstanceBuffer = nullptr;

	/////////////////////////////
	//RENDER TO TEXTURE OBJECTS//
	/////////////////////////////

	m_rttRtv = nullptr;
	m_rttDepthBuffer = nullptr;
	m_rttDepthView = nullptr;
	m_rttTexture = nullptr;
	m_rttSrv = nullptr;

	m_blurRtv = nullptr;
	m_blurTexture = nullptr;
	m_blurSrv = nullptr;

	////////////////////
	//CONSTANT BUFFERS//
	////////////////////

	m_vertexCBuffer = nullptr;
	m_pixelCBuffer = nullptr;
	m_initComputeCBuffer = nullptr;
	m_updateComputeCBuffer = nullptr;
	m_geometryCBuffer = nullptr;

	//////////////////
	//INPUT LAYOUTS //
	//COUNT: 6      //
	//////////////////

	m_genericInputLayout = nullptr;
	m_instancedGenericInputLayout = nullptr;
	m_2dInputLayout = nullptr;
	m_skyboxInputLayout = nullptr;
	m_instancedParticleInputLayout = nullptr;
	m_ribbonInputLayout = nullptr;

	/////////////////
	//PIXEL SHADERS//
	//COUNT: 13    //
	/////////////////

	m_genericPS = nullptr;
	m_instancedGenericPS = nullptr;
	m_2dPS = nullptr;
	m_healthbarPS = nullptr;
	m_skyboxPS = nullptr;
	m_particlePS = nullptr;
	m_fadePS = nullptr;
	m_spherePS = nullptr;
	m_distortionWavePS = nullptr;
	m_bulletPS = nullptr;
	m_linePS = nullptr;
	m_ribbonPS = nullptr;
	m_instancedWhitePS = nullptr;
	m_scrollingUVPS = nullptr;


	//////////////////
	//VERTEX SHADERS//
	//COUNT: 9      //
	//////////////////

	m_genericVS = nullptr;
	m_instancedGenericVS = nullptr;
	m_instancedBillboardVS = nullptr;
	m_particleBillboardZVS = nullptr;
	m_2dVS = nullptr;
	m_skyboxVS = nullptr;
	m_particleVS = nullptr;
	m_ribbonVS = nullptr;
	m_scaleInstancedVS = nullptr;
	m_billboardVS = nullptr;

	/////////////////
	//CS/GS SHADERS//
	//COUNT: 4     //
	/////////////////

	m_preParticleInitCS = nullptr;
	m_initParticleCS = nullptr;
	m_updateParticleCS = nullptr;

	m_ribbonGS = nullptr;


	for (int i = 0; i < 4; i++)
	{
		m_distortionUsed[i] = false;
	}


	/////////
	//OTHER//
	/////////

	m_skyboxObject = nullptr;
	m_fadeComp = nullptr;
	m_mouseCursor = nullptr;

	m_displayModeList = nullptr;
	m_instanceData = nullptr;
	m_particleInstanceData = nullptr;

	///////////////////
	//NOT IN SHUTDOWN//
	///////////////////

	m_renderList = nullptr;
	m_emitterList = nullptr;
	m_currMode = 0;
	m_crosshairShowing = true;
	m_playerShieldShowing = false;
	m_bossShieldShowing = false;
	m_bitmapFont = nullptr;

	m_currentPS = nullptr;
	m_currentVS = nullptr;


	m_fadeIn = true;

	m_warpScreen = false;
}

D3DGraphics::~D3DGraphics()
{
}

MSAA_State D3DGraphics::GetMSAAState(int index)
{
	if (index < 0)
	{
		return m_msaaLevels[0];
	}
	else if (index > 3)
	{
		return m_msaaLevels[3];
	}

	return m_msaaLevels[index];
}

Screen_State D3DGraphics::GetScreenState(int index)
{
	if (index < 0)
	{
		return m_screenStates[0];
	}
	else if (index > 2)
	{
		return m_screenStates[2];
	}

	return m_screenStates[index];
}

Vsync_State D3DGraphics::GetVsyncState(int index)
{
	if (index < 0)
	{
		return m_vsyncStates[0];
	}
	else if (index > 1)
	{
		return m_vsyncStates[1];
	}

	return m_vsyncStates[index];
}

Resolution_State D3DGraphics::GetResolutionState(int index)
{
	if (index < 0)
	{
		return m_resolutionStates[0];
	}
	else if (((UINT)index) >= m_resolutionStates.size())
	{
		return m_resolutionStates[m_resolutionStates.size() - 1];
	}

	return m_resolutionStates[index];
}

UINT D3DGraphics::GetNumResolutions()
{
	return m_resolutionStates.size();
}



void D3DGraphics::ApplyGraphicsSettings(int msaaIndex, int screenIndex, int resolutionIndex, int vsyncIndex, float gamma)
{

	//First apply msaa settings
	if (msaaIndex < 0)
	{
		msaaIndex = 0;
	}
	else if (msaaIndex > 3)
	{
		msaaIndex = 3;
	}

	MSAA_LEVEL = m_msaaLevels[msaaIndex].m_msaaValue;
	HandleResize();

	//Next apply fullscreen settings
	if (screenIndex < 0)
	{
		screenIndex = 0;
	}
	else if (screenIndex > 2)
	{
		screenIndex = 2;
	}

	if (m_bordersActive != m_screenStates[screenIndex].bordersActive && m_screenStates[screenIndex].fullScreen == false && FULL_SCREEN == false)
	{
		m_bordersActive = m_screenStates[screenIndex].bordersActive;

		if (m_bordersActive)
		{
			//WINDOWED
			SetWindowLong(m_hWnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
			SetWindowPos(m_hWnd, m_hWnd, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
			ShowWindow(m_hWnd, SW_SHOWNORMAL);

		}
		else
		{
			//BORDERLESS
			SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
			ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);

		}
	}
	else
	{
		m_bordersActive = m_screenStates[screenIndex].bordersActive;

	}

	SetFullscreen(m_screenStates[screenIndex].fullScreen);

	//Next resolution settings

	SetResolution(resolutionIndex);

	//Next vsync settings

	VSYNC_ENABLED = m_vsyncStates[vsyncIndex].vsync;

	//gamma

	MapGamma(gamma);

	//TWICE FOR GOOD MEASURE

	//First apply msaa settings
	if (msaaIndex < 0)
	{
		msaaIndex = 0;
	}
	else if (msaaIndex > 3)
	{
		msaaIndex = 3;
	}

	MSAA_LEVEL = m_msaaLevels[msaaIndex].m_msaaValue;
	HandleResize();

	//Next apply fullscreen settings
	if (screenIndex < 0)
	{
		screenIndex = 0;
	}
	else if (screenIndex > 2)
	{
		screenIndex = 2;
	}

	if (m_bordersActive != m_screenStates[screenIndex].bordersActive && m_screenStates[screenIndex].fullScreen == false && FULL_SCREEN == false)
	{
		m_bordersActive = m_screenStates[screenIndex].bordersActive;

		if (m_bordersActive)
		{
			//WINDOWED
			SetWindowLong(m_hWnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
			SetWindowPos(m_hWnd, m_hWnd, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
			ShowWindow(m_hWnd, SW_SHOWNORMAL);

		}
		else
		{
			//BORDERLESS
			SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
			ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);

		}
	}
	else
	{
		m_bordersActive = m_screenStates[screenIndex].bordersActive;

	}

	SetFullscreen(m_screenStates[screenIndex].fullScreen);

	//Next resolution settings

	SetResolution(resolutionIndex);

	//Next vsync settings

	VSYNC_ENABLED = m_vsyncStates[vsyncIndex].vsync;

	//gamma

	MapGamma(gamma);

}


bool D3DGraphics::Initialize(HINSTANCE hInstance)
{




	//////////////////
	//INITIALIZATION//
	//////////////////

	MakeWindow(hInstance, (WNDPROC)WndProc);
	MakeDeviceAndChain();

	SCREEN_WIDTH = m_displayModeList[m_currMode].Width;
	SCREEN_HEIGHT = m_displayModeList[m_currMode].Height;

	if (!FULL_SCREEN)
	{
		m_chain->ResizeTarget(&m_displayModeList[m_currMode]);
	}

	//m_chain->ResizeTarget(&m_displayModeList[m_currMode]);
	//m_chain->SetFullscreenState(FULL_SCREEN, NULL);

	MakeDepthStates();
	MakeRastState();
	MakeInputLayouts();
	MakeConstantBuffers();
	MakeSampleState();
	MakeBlendState();
	MakeShaders();
	MakeInstanceBuffers();
	HandleResize();
	DisableAltEnter();


	//////////////////
	//SET UP OBJECTS//
	//////////////////

	MakeViewMatrix();

	m_assetManager.Initialize(m_device);
	m_assetManager.LoadAssets();
	MakeRenderInfos();

	m_sphereDistortionMeshes[0] = m_assetManager.m_meshes["Distortion A Sphere"];
	m_sphereDistortionMeshes[1] = m_assetManager.m_meshes["Distortion B Sphere"];
	m_sphereDistortionMeshes[2] = m_assetManager.m_meshes["Distortion C Sphere"];
	m_sphereDistortionMeshes[3] = m_assetManager.m_meshes["Distortion D Sphere"];

	m_roundedCubeDistortionMeshes[0] = m_assetManager.m_meshes["Distortion A RoundedCube"];
	m_roundedCubeDistortionMeshes[1] = m_assetManager.m_meshes["Distortion B RoundedCube"];
	m_roundedCubeDistortionMeshes[2] = m_assetManager.m_meshes["Distortion C RoundedCube"];
	m_roundedCubeDistortionMeshes[3] = m_assetManager.m_meshes["Distortion D RoundedCube"];

	m_torusDistortionMeshes[0] = m_assetManager.m_meshes["Distortion A Torus"];
	m_torusDistortionMeshes[1] = m_assetManager.m_meshes["Distortion B Torus"];
	m_torusDistortionMeshes[2] = m_assetManager.m_meshes["Distortion C Torus"];
	m_torusDistortionMeshes[3] = m_assetManager.m_meshes["Distortion D Torus"];

	MakeSkyboxObject();

	m_fadeComp = new RenderComp();
	m_fadeComp->SetID("Fade");
	m_fadeComp->m_height = 1;
	m_fadeComp->m_width = 1;
	m_fadeComp->m_posX = 0;
	m_fadeComp->m_posY = 0;
	Make2DObject(m_fadeComp);

	// m_mouseCursor->m_width =
	m_mouseCursor = new RenderComp();
	m_mouseCursor->m_width = 0.05f;
	m_mouseCursor->m_height = 0.05f;
	m_mouseCursor->SetID("Crosshair");

	Make2DObject(m_mouseCursor);

	DXGI_RGB rgb;
	rgb.Blue = 1;
	rgb.Red = 1;
	rgb.Green = 1;

	//m_gamma.Scale = rgb;

	rgb.Blue = 0;
	rgb.Red = 0;
	rgb.Green = 0;
	//m_gamma.Offset = rgb;

	//////////////////////
	//Setup Menu Vectors//
	//////////////////////
	MSAA_State mState;

	mState.m_displayString = "Off";
	mState.m_msaaValue = 1;
	m_msaaLevels.push_back(mState);

	mState.m_displayString = "2x";
	mState.m_msaaValue = 2;
	m_msaaLevels.push_back(mState);

	mState.m_displayString = "4x";
	mState.m_msaaValue = 4;
	m_msaaLevels.push_back(mState);

	mState.m_displayString = "8x";
	mState.m_msaaValue = 8;
	m_msaaLevels.push_back(mState);


	Screen_State sState;

	sState.m_displayString = "Windowed";
	sState.fullScreen = false;
	sState.bordersActive = true;
	m_screenStates.push_back(sState);

	sState.m_displayString = "Borderless";
	sState.fullScreen = false;
	sState.bordersActive = false;
	m_screenStates.push_back(sState);

	sState.m_displayString = "Fullscreen";
	sState.fullScreen = true;
	sState.bordersActive = true;
	m_screenStates.push_back(sState);


	Vsync_State vState;

	vState.m_displayString = "Off";
	vState.vsync = false;
	m_vsyncStates.push_back(vState);

	vState.m_displayString = "On";
	vState.vsync = true;
	m_vsyncStates.push_back(vState);

	////////////
	//FINALIZE//
	////////////

	globalGraphicsPointer = this;

	if (m_bordersActive)
	{
		//WINDOWED
		SetWindowLong(m_hWnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
		SetWindowPos(m_hWnd, m_hWnd, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		ShowWindow(m_hWnd, SW_SHOWNORMAL);

	}
	else
	{
		//BORDERLESS
		SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
		ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);

	}

	if (FULL_SCREEN)
	{
		SetFullscreen(FULL_SCREEN);
	}


	m_pixelShaderData.fadeAmount = 1;

	float color[4];
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	color[3] = 1;

	m_context->OMSetRenderTargets(1, &m_rtv, m_depthView);
	m_context->ClearRenderTargetView(m_rtv, color);

	m_chain->Present(0, 0);

	return true;
}

void D3DGraphics::Frame(float deltaTime)
{

	///////////////////////////////
	//UPDATE FULL SCREEN VARIABLE//
	///////////////////////////////

	BOOL fullScreen;
	m_chain->GetFullscreenState(&fullScreen, nullptr);

	if (fullScreen)
	{
		FULL_SCREEN = true;
	}
	else
	{
		FULL_SCREEN = false;
	}

	//////////////////////
	//UPDATE VIEW MATRIX//
	//////////////////////

	if (m_camera)
	{
		m_viewMatrix = m_camera->GetTransform().GetWorldMatrix();
	}
	else
	{
		m_viewMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	}
	m_2DviewMatrix._41 = m_viewMatrix._41;
	m_2DviewMatrix._42 = m_viewMatrix._42;
	m_2DviewMatrix._43 = m_viewMatrix._43;


	//////////////////////////////////////////
	//DISPLAY FRAME RATE ON WINDOW TITLE BAR//
	//////////////////////////////////////////

	//static DWORD frameCount = 0; ++frameCount;
	//static DWORD framesPast = frameCount;
	//static DWORD prevCount = GetTickCount();
	//if (GetTickCount() - prevCount > 1000) // only update every second
	//{
	//	char buffer[256];
	//	FRAME_RATE = frameCount - framesPast;
	//	sprintf_s(buffer, "R.A.T.S.     FPS: %d", FRAME_RATE);
	//	SetWindowTextA(m_hWnd, buffer);
	//	framesPast = frameCount;
	//	prevCount = GetTickCount();

	//}

	m_pixelShaderData.gamma = GAMMA_VALUE;

	//////////
	//RENDER//
	//////////

	UpdateCursor();

	static float total = 0;
	total += deltaTime;
	m_pixelShaderData.warpAmount = total;


	if (m_warpScreen)
	{
		m_pixelShaderData.screenWarp = 1;

	}
	else
	{
		m_pixelShaderData.screenWarp = 0;

	}


	//FADE EFFECT
	if (m_pixelShaderData.fadeAmount < 1)
	{
		if (m_fadeIn)
		{
			m_pixelShaderData.fadeAmount += m_fadeFactor * deltaTime;

		}

		Render(m_rttRtv, m_rttDepthView);

	}
	else if (m_warpScreen)
	{
		Render(m_rttRtv, m_rttDepthView);
	}


	Render(m_rtv, m_depthView);


	if (VSYNC_ENABLED)
	{
		m_chain->Present(1, 0);
	}
	else
	{
		m_chain->Present(0, 0);
	}

	m_pixelShaderData.currTime += deltaTime;
}


void D3DGraphics::InsertRenderInfo(RenderInfo* info, std::string id)
{
	int index = HashString(id);


	if (m_renderInfos[index] != nullptr)
	{

		//IF the conflicting object is a GLOW, ask mark he has a special trick for those :D

		std::cout << "\nYou need to rename whatever renderinfo you just made because the hash function returned a value already in use!!\n";
		std::cout << "\nYou need to rename whatever renderinfo you just made because the hash function returned a value already in use!!\n";
		std::cout << "\nYou need to rename whatever renderinfo you just made because the hash function returned a value already in use!!\n";
		std::cout << "\nYou need to rename whatever renderinfo you just made because the hash function returned a value already in use!!\n";
		std::cout << "\nYou need to rename whatever renderinfo you just made because the hash function returned a value already in use!!\n";
		std::cout << "\nYou need to rename whatever renderinfo you just made because the hash function returned a value already in use!!\n";
		__debugbreak();
	}

	m_renderInfos[index] = info;

}

void D3DGraphics::MakeRenderInfos()
{

	m_renderInfos.resize(HashSize);

	for (UINT i = 0; i < m_renderInfos.size(); i++)
	{
		m_renderInfos[i] = nullptr;
	}

	RenderInfo* temp;
	//Leave these ones as is/////////////////////////
	//m_renderInfos["Skybox"] = new RenderInfo();
	temp = new RenderInfo();
	InsertRenderInfo(temp, "Skybox");

	//m_renderInfos["Fade"] = new RenderInfo();
	//m_renderInfos["Fade"]->m_pixelShader = m_fadePS;
	//m_renderInfos["Fade"]->m_vertexShader = m_2dVS;

	temp = new RenderInfo();
	temp->m_pixelShader = m_fadePS;
	temp->m_vertexShader = m_2dVS;

	InsertRenderInfo(temp, "Fade");

	//m_renderInfos["Lightning"] = new RenderInfo();
	//m_renderInfos["Lightning"]->m_pixelShader = m_linePS;
	//m_renderInfos["Lightning"]->m_vertexShader = m_skyboxVS;

	temp = new RenderInfo();
	temp->m_pixelShader = m_linePS;
	temp->m_vertexShader = m_skyboxVS;

	InsertRenderInfo(temp, "Lightning");


	///////////////////////////////////////////////

	InitRenderInfo("Shield", m_genericPS, m_genericVS);
	InitRenderInfo("Sphere", m_spherePS, m_genericVS);
	InitRenderInfo("RoundedCube", m_spherePS, m_genericVS);
	InitRenderInfo("Torus", m_spherePS, m_genericVS);


	InitRenderInfo("Distortion A Sphere", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion B Sphere", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion C Sphere", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion D Sphere", m_distortionWavePS, m_instancedGenericVS);

	InitRenderInfo("Distortion A RoundedCube", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion B RoundedCube", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion C RoundedCube", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion D RoundedCube", m_distortionWavePS, m_instancedGenericVS);

	InitRenderInfo("Distortion A Torus", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion B Torus", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion C Torus", m_distortionWavePS, m_instancedGenericVS);
	InitRenderInfo("Distortion D Torus", m_distortionWavePS, m_instancedGenericVS);

	InitRenderInfo("Cube", m_genericPS, m_genericVS);

	/////////////////////////
	//ENVIRONMENTAL HAZARDS
	////////////////////////

	InitRenderInfo("Tornado", m_scrollingUVPS, m_instancedGenericVS);
	InitRenderInfo("Prominence", m_scrollingUVPS, m_instancedGenericVS);
	InitRenderInfo("ProminenceBullet", m_scrollingUVPS, m_instancedGenericVS);
	InitRenderInfo("Singularity", m_scrollingUVPS, m_instancedGenericVS);


	////////////////
	//2D Hud Items//
	////////////////

	InitRenderInfo("PlayerHealthBarBack", m_2dPS, m_2dVS);
	InitRenderInfo("PlayerHealthBarFill", m_2dPS, m_2dVS);
	InitRenderInfo("EmpIcon", m_2dPS, m_2dVS);

	InitRenderInfo("PlayerEnergyFill", m_2dPS, m_2dVS);
	InitRenderInfo("PlayerEnergyBarBack", m_2dPS, m_2dVS);

	InitRenderInfo("PylonEnergyFill1", m_2dPS, m_2dVS, "PylonEnergyFill");
	InitRenderInfo("PylonEnergyFill2", m_2dPS, m_2dVS, "PylonEnergyFill");
	InitRenderInfo("PylonEnergyFill3", m_2dPS, m_2dVS, "PylonEnergyFill");
	InitRenderInfo("PylonEnergyBarBack", m_2dPS, m_2dVS);

	InitRenderInfo("iconPylon1", m_2dPS, m_2dVS);
	InitRenderInfo("iconPylon2", m_2dPS, m_2dVS);
	InitRenderInfo("iconPylon3", m_2dPS, m_2dVS);

	InitRenderInfo("GameOver", m_2dPS, m_2dVS);
	InitRenderInfo("GameWon", m_2dPS, m_2dVS);

	InitRenderInfo("iconSpreadGun", m_2dPS, m_2dVS);
	InitRenderInfo("iconWhipGun", m_2dPS, m_2dVS);
	InitRenderInfo("iconMissileGun", m_2dPS, m_2dVS);
	InitRenderInfo("iconOutline", m_2dPS, m_2dVS);


	////////////////
	//Pylon Arrows
	///////////////

	InitRenderInfo("arrowLambda", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("arrowSigma", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("arrowOmega", m_instancedGenericPS, m_instancedGenericVS);

	InitRenderInfo("Landing Indicator", m_instancedGenericPS, m_instancedGenericVS);


	////////////////////////
	//Splash Screen Images
	///////////////////////
	InitRenderInfo("RatsLogo", m_2dPS, m_2dVS);
	InitRenderInfo("HFGLogo", m_2dPS, m_2dVS);
	InitRenderInfo("GPGamesLogo", m_2dPS, m_2dVS);
	InitRenderInfo("WhiteSquare", m_2dPS, m_2dVS);
	InitRenderInfo("RatsAlpha", m_2dPS, m_2dVS);


	//////////////////
	//Main Menu Items
	/////////////////

	InitRenderInfo("menuBackground", m_2dPS, m_2dVS);
	InitRenderInfo("newGameButton", m_2dPS, m_2dVS);
	InitRenderInfo("howToPlayButton", m_2dPS, m_2dVS);
	InitRenderInfo("quitButton", m_2dPS, m_2dVS);
	InitRenderInfo("selectButton", m_2dPS, m_2dVS);
	InitRenderInfo("optionsButton", m_2dPS, m_2dVS);
	InitRenderInfo("loadGameButton", m_2dPS, m_2dVS);
	InitRenderInfo("menuNavKeyboardInstr", m_2dPS, m_2dVS);
	InitRenderInfo("menuNavGamepadInstr", m_2dPS, m_2dVS);
	InitRenderInfo("newGameButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("howToPlayButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("quitButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("optionsButtonSelect", m_2dPS, m_2dVS);
	InitRenderInfo("loadGameButtonSelected", m_2dPS, m_2dVS);



	//////////////////////
	//Upgrade Menu Items//
	//////////////////////

	InitRenderInfo("SpreadDmgButton", m_2dPS, m_2dVS);
	InitRenderInfo("WhipDmgButton", m_2dPS, m_2dVS);

	/////////////////////
	//Options Menu Items
	////////////////////
	InitRenderInfo("soundOptionsButton", m_2dPS, m_2dVS);
	InitRenderInfo("graphicsOptionsButton", m_2dPS, m_2dVS);
	InitRenderInfo("keybindingsOptionsButton", m_2dPS, m_2dVS);
	InitRenderInfo("gameModeOptionsButton", m_2dPS, m_2dVS);
	InitRenderInfo("backToMainMenuButton", m_2dPS, m_2dVS);
	InitRenderInfo("creditsButton", m_2dPS, m_2dVS);
	InitRenderInfo("inputButton", m_2dPS, m_2dVS);


	InitRenderInfo("soundOptionsButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("graphicsOptionsButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("keybindingsOptionsButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("gameModeOptionsButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("backToMainMenuButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("creditsButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("inputButtonSelected", m_2dPS, m_2dVS);

	//////////////////////
	//Sound Options Items
	/////////////////////

	InitRenderInfo("bgmVolumeButton", m_2dPS, m_2dVS);
	InitRenderInfo("sfxVolumeButton", m_2dPS, m_2dVS);
	InitRenderInfo("bgmVolumeButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("sfxVolumeButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("dialogueVolumeButton", m_2dPS, m_2dVS);
	InitRenderInfo("backButton", m_2dPS, m_2dVS);
	InitRenderInfo("backButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("sliderBar", m_2dPS, m_2dVS);
	InitRenderInfo("sliderIndicator", m_2dPS, m_2dVS);
	InitRenderInfo("leftArrow", m_2dPS, m_2dVS);
	InitRenderInfo("rightArrow", m_2dPS, m_2dVS);

	/////////////////////////
	//Graphics Options Items
	////////////////////////

	InitRenderInfo("screenResolutionButton", m_2dPS, m_2dVS);
	InitRenderInfo("windowModeButton", m_2dPS, m_2dVS);
	InitRenderInfo("msaaLevelButton", m_2dPS, m_2dVS);
	InitRenderInfo("vsyncButton", m_2dPS, m_2dVS);
	InitRenderInfo("screenResolutionButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("windowModeButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("msaaLevelButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("vsyncButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("ApplyButton", m_2dPS, m_2dVS);
	InitRenderInfo("ApplyButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("gammaButton", m_2dPS, m_2dVS);
	InitRenderInfo("gammaButtonSelected", m_2dPS, m_2dVS);



	////////////////////
	//Load Screen Items
	////////////////////

	InitRenderInfo("slot1Button", m_2dPS, m_2dVS);
	InitRenderInfo("slot2Button", m_2dPS, m_2dVS);
	InitRenderInfo("slot3Button", m_2dPS, m_2dVS);

	////////////////////
	//Level Select Items
	////////////////////
	InitRenderInfo("UnlockedLevelButton", m_2dPS, m_2dVS);
	InitRenderInfo("LockedLevelButton", m_2dPS, m_2dVS);
	InitRenderInfo("EarnedStar", m_2dPS, m_2dVS);
	InitRenderInfo("UnearnedStar", m_2dPS, m_2dVS);
	InitRenderInfo("LevelButton", m_2dPS, m_2dVS);
	InitRenderInfo("LevelButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("SphereIcon", m_2dPS, m_2dVS);
	InitRenderInfo("CubeIcon", m_2dPS, m_2dVS);
	InitRenderInfo("TorusIcon", m_2dPS, m_2dVS);
	InitRenderInfo("BackgroundStatic", m_2dPS, m_2dVS);
	InitRenderInfo("Lock", m_2dPS, m_2dVS);
	InitRenderInfo("LevelSelectBackground", m_2dPS, m_2dVS);




	////////////////////
	//How To Play Items
	///////////////////

	InitRenderInfo("howToPlayInstructions", m_2dPS, m_2dVS);
	InitRenderInfo("instructions1", m_2dPS, m_2dVS);
	InitRenderInfo("instructions2", m_2dPS, m_2dVS);
	InitRenderInfo("instructions3", m_2dPS, m_2dVS);
	InitRenderInfo("instructions4", m_2dPS, m_2dVS);
	InitRenderInfo("instructions5", m_2dPS, m_2dVS);
	InitRenderInfo("instructions6", m_2dPS, m_2dVS);
	InitRenderInfo("instructions7", m_2dPS, m_2dVS);
	InitRenderInfo("instructions8", m_2dPS, m_2dVS);
	InitRenderInfo("instructions9", m_2dPS, m_2dVS);
	InitRenderInfo("nextButton", m_2dPS, m_2dVS);
	InitRenderInfo("nextButtonSelected", m_2dPS, m_2dVS);
	InitRenderInfo("previousButton", m_2dPS, m_2dVS);
	InitRenderInfo("previousButtonSelected", m_2dPS, m_2dVS);

	///////////////////
	//Pause Menu Items
	//////////////////

	InitRenderInfo("resumeGameButton", m_2dPS, m_2dVS);
	InitRenderInfo("resumeGameButtonSelected", m_2dPS, m_2dVS);


	///////////////////
	//Pause Menu Items
	//////////////////

	InitRenderInfo("levelSelectSphereButton", m_2dPS, m_2dVS);
	InitRenderInfo("levelSelectRoundedCubeButton", m_2dPS, m_2dVS);
	InitRenderInfo("levelSelectTorusButton", m_2dPS, m_2dVS);

	//////////////////
	//Effecty Stuff //
	//////////////////

	InitRenderInfo("Effect Swirl", m_particlePS, m_particleBillboardZVS);

	//////////////
	//Other Stuff
	/////////////

	InitRenderInfo("Energy Particle", m_particlePS, m_particleVS, "Energy");
	InitRenderInfo("Basic Molecule", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Red Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Green Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Purple Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Pink Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Orangey Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Yellow Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Cyan Fire Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Purple Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Ice Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Green Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Red Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("White Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Gold Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Spark Particle2", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Spark Particle4", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Smokey Particle", m_particlePS, m_particleBillboardZVS);
	//InitRenderInfo("Smokey Particle", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Energy Battery", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Health Battery", m_particlePS, m_particleBillboardZVS);
	InitRenderInfo("Warning Skull", m_particlePS, m_particleBillboardZVS);

	InitRenderInfo("Invisible", m_instancedGenericPS, m_instancedGenericVS);


	//If you change the players pixel shader, make sure you change D3DGraphics::SetPlayerWhite() too
	InitRenderInfo("Player", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("InnerStar", m_scrollingUVPS, m_instancedGenericVS);
	InitRenderInfo("InnerCube", m_scrollingUVPS, m_instancedGenericVS);
	InitRenderInfo("InnerTorus", m_scrollingUVPS, m_instancedGenericVS);
	InitRenderInfo("Pylon", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Pylon Sigma", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Pylon Omega", m_instancedGenericPS, m_instancedGenericVS);

	InitRenderInfo("Fire", m_bulletPS, m_genericVS);
	InitRenderInfo("InnerRock", m_bulletPS, m_genericVS);
	InitRenderInfo("Spark A", m_bulletPS, m_genericVS);
	InitRenderInfo("Spark B", m_bulletPS, m_genericVS);

	InitRenderInfo("Particle Fire", m_particlePS, m_particleVS);
	InitRenderInfo("Crosshair", m_2dPS, m_2dVS);

	InitRenderInfo("Energy", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Pickup Health", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Pickup Shield", m_scrollingUVPS, m_instancedGenericVS);

	InitRenderInfo("Shield Effect", m_scrollingUVPS, m_instancedGenericVS);
	InitRenderInfo("Boss Shield Effect", m_scrollingUVPS, m_instancedGenericVS);


	InitRenderInfo("Ribbon", m_ribbonPS, m_ribbonVS);

	///////////////
	//Projectiles//
	//////////////

	InitRenderInfo("Player Whip Bullet", m_bulletPS, m_instancedGenericVS);
	InitRenderInfo("Player Seeker Bullet", m_bulletPS, m_instancedGenericVS);
	InitRenderInfo("Player Spread Bullet", m_bulletPS, m_instancedGenericVS);
	InitRenderInfo("Player Missile Bullet", m_bulletPS, m_instancedGenericVS);
	InitRenderInfo("Enemy Simple Bullet", m_bulletPS, m_instancedGenericVS);

	InitRenderInfo("Mine", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Enemy Missile", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Fence Post", m_instancedGenericPS, m_instancedGenericVS);

	///////////
	//Enemies//
	///////////

	InitRenderInfo("Enemy", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Koi A", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Koi D", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Bomber", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Mine Layer", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Dodger", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Fence Layer", m_instancedGenericPS, m_instancedGenericVS);

	InitRenderInfo("Goliath", m_instancedGenericPS, m_instancedGenericVS);
	InitRenderInfo("Boss", m_instancedGenericPS, m_instancedGenericVS);




	////////
	//Glow//
	////////

	InitRenderInfo("Player Whip Bullet Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Player Spread Bullet Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Player Missile Bullet Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Player Seeker Bullet Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Pickup Shield Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Enemy Simple Bullet Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Enemy Missile Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Enemy Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Koi A Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Koi D Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Mine Layer Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Fence Layer Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Fence Post Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Goliath Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Dodger Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Bomber Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Mine Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Mine Glow2", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Energy Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Pickup Health Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Boss Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("Boss Glow2", m_instancedGenericPS, m_instancedBillboardVS);


	InitRenderInfo("arrowSigma Red Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("arrowLamda Red Glow", m_instancedGenericPS, m_instancedBillboardVS);
	InitRenderInfo("arrowOmega Red Glow", m_instancedGenericPS, m_instancedBillboardVS);
	//////////////////
	//Scenery Models//
	//////////////////

	InitRenderInfo("Asteroid01_Plain", m_genericPS, m_genericVS);
	InitRenderInfo("Asteroid01_Cyan", m_genericPS, m_genericVS);
	InitRenderInfo("Asteroid01_Red", m_genericPS, m_genericVS);
	InitRenderInfo("Asteroid02_Ice", m_genericPS, m_genericVS);
	InitRenderInfo("Asteroid02_Rock", m_genericPS, m_genericVS);
	InitRenderInfo("Planet01", m_genericPS, m_genericVS);
	InitRenderInfo("Planet02", m_genericPS, m_genericVS);
	InitRenderInfo("Planet03_Pink", m_genericPS, m_genericVS);
	InitRenderInfo("Planet03_Road", m_genericPS, m_genericVS);
	InitRenderInfo("Planet03_Black", m_genericPS, m_genericVS);
	InitRenderInfo("Planet04_Gas", m_genericPS, m_genericVS);
	InitRenderInfo("Planet04_Grass", m_genericPS, m_genericVS);
	InitRenderInfo("Planet04_Fire", m_genericPS, m_genericVS);
	InitRenderInfo("Planet04_Ice", m_genericPS, m_genericVS);
	InitRenderInfo("Planet04_Sepia", m_genericPS, m_genericVS);

}

void D3DGraphics::Shutdown()
{
	m_assetManager.Shutdown();

	if (m_chain)
	{
		m_chain->SetFullscreenState(false, nullptr);
	}

	SafeRelease(m_output);


	/////////////////////
	//UNIVERSAL OBJECTS//
	/////////////////////

	SafeRelease(m_rtv);
	SafeRelease(m_chain);
	SafeRelease(m_depthBuffer);
	SafeRelease(m_depthView);


	SafeRelease(m_sampleStateClampY);
	SafeRelease(m_sampleStateWrap);
	SafeRelease(m_alphaBlendState);
	//	SafeRelease(m_additiveBlendState);

	SafeRelease(m_2dDepthState);
	SafeRelease(m_3dDepthState);
	SafeRelease(m_transparentDepthState);


	SafeRelease(m_rastStateCullBack);
	SafeRelease(m_rastStateCullFront);
	SafeRelease(m_rastStateCullNone);

	SafeRelease(m_instanceBuffer);
	SafeRelease(m_particleInstanceBuffer);


	/////////////////////////////
	//RENDER TO TEXTURE OBJECTS//
	/////////////////////////////

	SafeRelease(m_rttRtv);
	SafeRelease(m_rttDepthBuffer);
	SafeRelease(m_rttDepthView);
	SafeRelease(m_rttTexture);
	SafeRelease(m_rttSrv);

	SafeRelease(m_blurRtv);
	SafeRelease(m_blurTexture);
	SafeRelease(m_blurSrv);

	////////////////////
	//CONSTANT BUFFERS//
	////////////////////

	SafeRelease(m_vertexCBuffer);
	SafeRelease(m_pixelCBuffer);
	SafeRelease(m_initComputeCBuffer);
	SafeRelease(m_updateComputeCBuffer);
	SafeRelease(m_geometryCBuffer);

	/////////////////
	//INPUT LAYOUTS//
	/////////////////

	SafeRelease(m_genericInputLayout);
	SafeRelease(m_instancedGenericInputLayout);
	SafeRelease(m_2dInputLayout);
	SafeRelease(m_skyboxInputLayout);
	SafeRelease(m_instancedParticleInputLayout);
	SafeRelease(m_ribbonInputLayout);

	/////////////////
	//PIXEL SHADERS//
	/////////////////

	SafeRelease(m_genericPS);
	SafeRelease(m_instancedGenericPS);
	SafeRelease(m_2dPS);
	SafeRelease(m_healthbarPS);
	SafeRelease(m_skyboxPS);
	SafeRelease(m_particlePS);

	SafeRelease(m_fadePS);
	SafeRelease(m_scrollingUVPS);
	SafeRelease(m_spherePS);
	SafeRelease(m_distortionWavePS);
	SafeRelease(m_bulletPS);
	SafeRelease(m_linePS);
	SafeRelease(m_ribbonPS);

	SafeRelease(m_instancedWhitePS);


	//////////////////
	//VERTEX SHADERS//
	//////////////////

	SafeRelease(m_genericVS);
	SafeRelease(m_instancedGenericVS);
	SafeRelease(m_instancedBillboardVS);
	SafeRelease(m_particleBillboardZVS);
	SafeRelease(m_2dVS);
	SafeRelease(m_skyboxVS);
	SafeRelease(m_particleVS);

	SafeRelease(m_scaleInstancedVS);
	SafeRelease(m_ribbonVS);
	SafeRelease(m_billboardVS);

	///////////////////
	//COMPUTE SHADERS//
	///////////////////

	SafeRelease(m_preParticleInitCS);
	SafeRelease(m_initParticleCS);
	SafeRelease(m_updateParticleCS);

	SafeRelease(m_ribbonGS);


	/////////
	//OTHER//
	/////////

	SafeDelete(m_skyboxObject);
	SafeDelete(m_fadeComp);
	SafeDelete(m_mouseCursor);

	SafeDeleteArray(m_displayModeList);
	SafeDeleteArray(m_instanceData);
	SafeDeleteArray(m_particleInstanceData);

	///////////////////////
	//SHARED RENDER INFOS//
	///////////////////////

	for (UINT i = 0; i < m_renderInfos.size(); i++)
	{
		//Iter.second is a pointer to the data being stored in the map
		SafeDelete(m_renderInfos[i]);
	}
	m_renderInfos.clear();

	///////////////////////////
	//SINGLE USE RENDER INFOS//
	///////////////////////////

	for (auto iter : m_trashInfos)
	{
		if (iter->m_mesh)
		{
			SafeRelease(iter->m_mesh->m_indexBuffer);
			SafeRelease(iter->m_mesh->m_vertexBuffer);

			SafeDeleteArray(iter->m_mesh->m_posArray);
			SafeDeleteArray(iter->m_mesh->m_indexArray);
			SafeDeleteArray(iter->m_mesh->m_vertexArray);
			SafeDeleteArray(iter->m_mesh->m_shortVertexArray);

			SafeDelete(iter->m_mesh);
		}

		SafeDelete(iter);

	}
	m_trashInfos.clear();

	///////////////////////////////
	//SINGLE USE PARTICLE BUFFERS//
	///////////////////////////////

	//for (auto iter : m_trashParticleBuffers)
	//{
	//	SafeRelease(iter->m_particleBuffer);
	//	SafeRelease(iter->m_particleInstanceStructBuffer);
	//	SafeRelease(iter->m_particleInstanceVertBuffer);
	//	SafeRelease(iter->m_particleInstanceStructDeadIndxBuffer);

	//	SafeRelease(iter->m_particleInstanceUAV);
	//	SafeRelease(iter->m_particleUAV);
	//	SafeRelease(iter->m_particleInstanceDeadIndxUAV);

	//	SafeDelete(iter);
	//}
	//m_trashParticleBuffers.clear();



	//SafeRelease(m_context);
	//SafeRelease(m_device);

	if (m_device)
		DXMemoryCheck(m_device->Release(), m_context);

	SafeRelease(m_context);

}


void D3DGraphics::FlushOldMemory()
{
	for (auto iter : m_trashInfos)
	{
		if (iter->m_mesh)
		{
			SafeRelease(iter->m_mesh->m_indexBuffer);
			SafeRelease(iter->m_mesh->m_vertexBuffer);

			SafeDeleteArray(iter->m_mesh->m_posArray);
			SafeDeleteArray(iter->m_mesh->m_indexArray);
			SafeDeleteArray(iter->m_mesh->m_vertexArray);
			SafeDeleteArray(iter->m_mesh->m_shortVertexArray);

			SafeDelete(iter->m_mesh);
		}

		SafeDelete(iter);


	}
	m_trashInfos.clear();

	m_mouseCursor->m_renderInfo = nullptr;
	m_fadeComp->m_renderInfo = nullptr;
}


#pragma region Initialization Functions

void D3DGraphics::MakeWindow(HINSTANCE hInstance, WNDPROC proc)
{

	m_appWndProc = proc;
	m_hInst = hInstance;

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	UINT numModes;
	DXGI_ADAPTER_DESC adapterDesc;

	//Finding video card and monitor information
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	factory->EnumAdapters(0, &adapter);
	adapter->EnumOutputs(0, &m_output);
	m_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	m_displayModeList = new DXGI_MODE_DESC[numModes];
	m_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, m_displayModeList);

	m_numDisplayModes = numModes;

	int backUp = 0;
	bool hitIt = false;

	for (UINT i = 0; i < numModes; i++)
	{


		if (m_displayModeList[i].Scaling == DXGI_MODE_SCALING_CENTERED)
		{
			continue;
		}

		if (m_displayModeList[i].Width == 800)
		{
			if (m_displayModeList[i].Height == 600)
			{
				backUp = i;
			}
		}


		if (m_displayModeList[i].Width == (unsigned int)SCREEN_WIDTH)
		{
			if (m_displayModeList[i].Height == (unsigned int)SCREEN_HEIGHT)
			{
				m_currMode = i;
				hitIt = true;
			}
		}

		bool insert = true;

		for (UINT k = 0; k < m_resolutionStates.size(); k++)
		{
			if (m_resolutionStates[k].height == m_displayModeList[i].Height && m_resolutionStates[k].width == m_displayModeList[i].Width)
			{
				insert = false;
				break;
			}
		}

		if (insert)
		{

			Resolution_State resState;

			resState.height = m_displayModeList[i].Height;
			resState.width = m_displayModeList[i].Width;
			resState.modeIndex = i;

			resState.m_displayString = std::to_string(resState.width);
			resState.m_displayString += "x";
			resState.m_displayString += std::to_string(resState.height);

			m_resolutionStates.push_back(resState);


		}
	}

	if (hitIt == false)
	{
		m_currMode = backUp;
	}


	adapter->GetDesc(&adapterDesc);

	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	//error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	//if (error != 0)
	//{
	//	return false;
	//}


	SafeRelease(adapter);
	SafeRelease(factory);


	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = m_appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = m_hInst;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	wndClass.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_RATS));
	//wndClass.hIconSm = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_WINLOGO));
	wndClass.hIconSm = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_RATS));
	//wc.hIconSm = (HICON)LoadImage(m_hInst, "theicon", IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	RegisterClassEx(&wndClass);

	RECT window_size = { (LONG)0, (LONG)0, (LONG)100, (LONG)100 };

	if (m_bordersActive)
	{
		AdjustWindowRect(&window_size, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, false);

		m_hWnd = CreateWindow(L"DirectXApplication", L"R.A.T.S.", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
			NULL, NULL, m_hInst, NULL);
	}
	else
	{
		AdjustWindowRect(&window_size, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, false);

		m_hWnd = CreateWindow(L"DirectXApplication", L"R.A.T.S.", WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
			NULL, NULL, m_hInst, NULL);
	}

}

HRESULT D3DGraphics::MakeDeviceAndChain()
{

	HRESULT result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferDesc = m_displayModeList[m_currMode];
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_hWnd;

	//MSAA
	swapChainDesc.SampleDesc.Count = MSAA_LEVEL;
	swapChainDesc.SampleDesc.Quality = 0;

	//FULLSCREEN
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//swapChainDesc.Flags = 0;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL	levelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT				numLevelsRequested = 1;
	D3D_FEATURE_LEVEL	levelsSupported;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXCall(result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
		&levelsRequested, numLevelsRequested, D3D11_SDK_VERSION, &swapChainDesc, &m_chain, &m_device,
		&levelsSupported, &m_context));

	DXNameInit(m_context);

	DXName(m_chain, "SwapChain");
	DXName(m_device, "Device");
	DXName(m_context, "DeviceContext");

	return result;
}

HRESULT D3DGraphics::MakeRTVs()
{
	HRESULT result;

	SafeRelease(m_rtv);

	ID3D11Texture2D* backBuffer;
	DXCall(result = m_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer));
	DXCall(result = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_rtv));

	DXName(m_rtv, "Backbuffer RTV");

	backBuffer->Release();
	backBuffer = nullptr;

	SafeRelease(m_rttRtv);
	SafeRelease(m_rttTexture);
	SafeRelease(m_rttSrv);

	SafeRelease(m_blurRtv);
	SafeRelease(m_blurTexture);
	SafeRelease(m_blurSrv);

	//Map Stuff
	D3D11_TEXTURE2D_DESC textDesc;
	textDesc.Width = (UINT)(SCREEN_WIDTH);// *1);
	textDesc.Height = (UINT)(SCREEN_HEIGHT);// *1);
	textDesc.MipLevels = 1;
	textDesc.ArraySize = 1;
	textDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textDesc.SampleDesc.Count = 1;
	textDesc.SampleDesc.Quality = 0;
	textDesc.Usage = D3D11_USAGE_DEFAULT;
	textDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textDesc.CPUAccessFlags = 0;
	textDesc.MiscFlags = 0;

	DXCall(m_device->CreateTexture2D(&textDesc, 0, &m_blurTexture));
	DXCall(m_device->CreateTexture2D(&textDesc, 0, &m_rttTexture));

	DXName(m_blurTexture, "Blur Tex2D");
	DXName(m_rttTexture, "Render-To-Texture Tex2D");


	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	DXCall(m_device->CreateRenderTargetView(m_blurTexture, &rtvDesc, &m_blurRtv));
	DXCall(m_device->CreateRenderTargetView(m_rttTexture, &rtvDesc, &m_rttRtv));

	DXName(m_blurRtv, "Blur RTV");
	DXName(m_rttRtv, "Render-To-Texture RTV");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	DXCall(m_device->CreateShaderResourceView(m_blurTexture, &srvDesc, &m_blurSrv));
	DXCall(m_device->CreateShaderResourceView(m_rttTexture, &srvDesc, &m_rttSrv));

	DXName(m_blurSrv, "Blur SRV");
	DXName(m_rttSrv, "Render-To-Texture SRV");

	return result;
}

HRESULT D3DGraphics::MakeDepthBuffers()
{
	HRESULT result;

	SafeRelease(m_depthBuffer);
	SafeRelease(m_rttDepthBuffer);

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.Height = SCREEN_HEIGHT;
	depthBufferDesc.Width = SCREEN_WIDTH;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//MSAA
	depthBufferDesc.SampleDesc.Count = MSAA_LEVEL;
	depthBufferDesc.SampleDesc.Quality = 0;

	DXCall(result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthBuffer));

	DXName(m_depthBuffer, "Depth Buffer");

	//depthBufferDesc.Height = (UINT)(SCREEN_HEIGHT * 1);
	//depthBufferDesc.Width = (UINT)(SCREEN_WIDTH * 1);
	depthBufferDesc.SampleDesc.Count = 1;

	DXCall(result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_rttDepthBuffer));

	DXName(m_rttDepthBuffer, "Render-To-Texture Depth Buffer");

	return result;
}

HRESULT D3DGraphics::MakeBlendState()
{

	SafeRelease(m_alphaBlendState);

	D3D11_BLEND_DESC blendDesc;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT result;
	DXCall(result = m_device->CreateBlendState(&blendDesc, &m_alphaBlendState));

	return result;
}

HRESULT D3DGraphics::MakeDepthStates()
{
	HRESULT result;

	SafeRelease(m_3dDepthState);
	SafeRelease(m_2dDepthState);
	SafeRelease(m_transparentDepthState);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DXCall(result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_3dDepthState));

	DXName(m_3dDepthState, "3D Depth State");

	depthStencilDesc.DepthEnable = false;
	DXCall(result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_2dDepthState));

	DXName(m_2dDepthState, "2D Depth State");

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DXCall(result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_transparentDepthState));

	DXName(m_transparentDepthState, "Transparent Depth State");

	return result;
}

HRESULT D3DGraphics::MakeDepthViews()
{
	HRESULT result;

	SafeRelease(m_depthView);
	SafeRelease(m_rttDepthView);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	//D3D11_DSV_DIMENSION_TEXTURE2DMS for MSAA
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;

	DXCall(result = m_device->CreateDepthStencilView(m_depthBuffer, &depthStencilViewDesc, &m_depthView));

	DXName(m_depthView, "Depth Buffer DSV");

	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	DXCall(result = m_device->CreateDepthStencilView(m_rttDepthBuffer, &depthStencilViewDesc, &m_rttDepthView));

	DXName(m_rttDepthView, "Render-To-Texture Depth Buffer DSV");

	return result;
}

HRESULT D3DGraphics::MakeRastState()
{
	HRESULT result;

	SafeRelease(m_rastStateCullBack);
	SafeRelease(m_rastStateCullFront);
	SafeRelease(m_rastStateCullNone);

	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.AntialiasedLineEnable = true;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthBias = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;

	//MSAA
	rastDesc.MultisampleEnable = true;

	DXCall(result = m_device->CreateRasterizerState(&rastDesc, &m_rastStateCullBack));

	DXName(m_rastStateCullBack, "Backface Cull Rasterizer State");

	rastDesc.CullMode = D3D11_CULL_FRONT;

	DXCall(result = m_device->CreateRasterizerState(&rastDesc, &m_rastStateCullFront));

	DXName(m_rastStateCullFront, "Frontface Cull Rasterizer State");

	rastDesc.CullMode = D3D11_CULL_NONE;

	DXCall(result = m_device->CreateRasterizerState(&rastDesc, &m_rastStateCullNone));

	DXName(m_rastStateCullNone, "None Cull Rasterizer State");


	return result;
}

HRESULT D3DGraphics::MakeInputLayouts()
{
	HRESULT result;


	SafeRelease(m_genericInputLayout);
	SafeRelease(m_instancedGenericInputLayout);
	SafeRelease(m_2dInputLayout);
	SafeRelease(m_skyboxInputLayout);
	SafeRelease(m_instancedParticleInputLayout);
	SafeRelease(m_ribbonInputLayout);


	D3D11_INPUT_ELEMENT_DESC gLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DXCall(result = m_device->CreateInputLayout(gLayout, 3, Generic_VS, sizeof(Generic_VS), &m_genericInputLayout));

	DXName(m_genericInputLayout, "Generic Input Layout");

	D3D11_INPUT_ELEMENT_DESC instancedgLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	DXCall(result = m_device->CreateInputLayout(instancedgLayout, 7, InstancedGeneric_VS, sizeof(InstancedGeneric_VS), &m_instancedGenericInputLayout));

	DXName(m_instancedGenericInputLayout, "Instanced Generic Input Layout");

	D3D11_INPUT_ELEMENT_DESC sLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DXCall(result = m_device->CreateInputLayout(sLayout, 1, Skybox_VS, sizeof(Skybox_VS), &m_skyboxInputLayout));

	DXName(m_skyboxInputLayout, "Skybox Input Layout");

	D3D11_INPUT_ELEMENT_DESC tLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DXCall(result = m_device->CreateInputLayout(tLayout, 2, TwoD_VS, sizeof(TwoD_VS), &m_2dInputLayout));

	DXName(m_2dInputLayout, "2D Input Layout");

	D3D11_INPUT_ELEMENT_DESC particleLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "SCALE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	DXCall(result = m_device->CreateInputLayout(particleLayout, 9, Particle_VS, sizeof(Particle_VS), &m_instancedParticleInputLayout));

	DXName(m_instancedParticleInputLayout, "Particle Input Layout");


	D3D11_INPUT_ELEMENT_DESC ribbonLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATIONAXIS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WIDTH", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATIONAXIS", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WIDTH", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DXCall(result = m_device->CreateInputLayout(ribbonLayout, 9, Ribbon_VS, sizeof(Ribbon_VS), &m_ribbonInputLayout));

	DXName(m_ribbonInputLayout, "Ribbon Input Layout");

	return result;
}

HRESULT D3DGraphics::MakeConstantBuffers()
{
	HRESULT result;

	SafeRelease(m_vertexCBuffer);
	SafeRelease(m_pixelCBuffer);
	SafeRelease(m_geometryCBuffer);
	SafeRelease(m_initComputeCBuffer);
	SafeRelease(m_updateComputeCBuffer);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	bufferDesc.ByteWidth = sizeof(ToVertexShader);
	DXCall(m_device->CreateBuffer(&bufferDesc, 0, &m_vertexCBuffer));

	DXName(m_vertexCBuffer, "Generic Vertex Shader Constant Buffer");


	bufferDesc.ByteWidth = sizeof(ToPixelShader);
	DXCall(result = m_device->CreateBuffer(&bufferDesc, 0, &m_pixelCBuffer));

	DXName(m_pixelCBuffer, "Pixel Shader Constant Buffer");


	bufferDesc.ByteWidth = sizeof(ToGeometryShader);
	DXCall(result = m_device->CreateBuffer(&bufferDesc, 0, &m_geometryCBuffer));

	DXName(m_geometryCBuffer, "Geometry Shader Constant Buffer");

	//bufferDesc.ByteWidth = sizeof(ToInitComputeShader);
	//DXCall(result = m_device->CreateBuffer(&bufferDesc, 0, &m_initComputeCBuffer));

	//DXName(m_initComputeCBuffer, "Init Compute Shader Constant Buffer");

	//bufferDesc.ByteWidth = sizeof(ToUpdateComputeShader);
	//DXCall(result = m_device->CreateBuffer(&bufferDesc, 0, &m_updateComputeCBuffer));

	//DXName(m_updateComputeCBuffer, "Update Compute Shader Constant Buffer");

	//Not entirely accurate because it only returns the last result
	return result;
}

void D3DGraphics::MakeViewport(D3D11_VIEWPORT &viewport)
{
	viewport.Width = (float)SCREEN_WIDTH;
	viewport.Height = (float)SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

HRESULT D3DGraphics::MakeSampleState()
{
	HRESULT result;

	SafeRelease(m_sampleStateClampY);
	SafeRelease(m_sampleStateWrap);

	D3D11_SAMPLER_DESC sampleDesc;
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MinLOD = -FLT_MAX;
	sampleDesc.MaxLOD = FLT_MAX;
	sampleDesc.MipLODBias = 0;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;


	sampleDesc.BorderColor[0] = 1.0f;
	sampleDesc.BorderColor[1] = 1.0f;
	sampleDesc.BorderColor[2] = 1.0f;
	sampleDesc.BorderColor[3] = 1.0f;

	DXCall(result = m_device->CreateSamplerState(&sampleDesc, &m_sampleStateClampY));

	DXName(m_sampleStateClampY, "Generic Sampler State");

	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	DXCall(result = m_device->CreateSamplerState(&sampleDesc, &m_sampleStateWrap));


	return result;
}

HRESULT D3DGraphics::DisableAltEnter()
{
	HRESULT result;

	IDXGIDevice * pDXGIDevice;
	DXCall(m_device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice));

	IDXGIAdapter * pDXGIAdapter;
	DXCall(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter));

	IDXGIFactory * pIDXGIFactory;
	DXCall(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory));

	DXCall(result = pIDXGIFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	SafeRelease(pDXGIDevice);
	SafeRelease(pDXGIAdapter);
	SafeRelease(pIDXGIFactory);

	return result;

}

void D3DGraphics::MakeProjMatrices(float fov)
{
	XMStoreFloat4x4(&m_projMatrix, XMMatrixPerspectiveFovLH((3.14f / 180) * fov, ((float)SCREEN_WIDTH / SCREEN_HEIGHT), SCREEN_NEAR, SCREEN_DEPTH));
	XMStoreFloat4x4(&m_orthoProjMatrix, XMMatrixOrthographicLH((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH));
}

void D3DGraphics::MakeShaders()
{
	/////////////////
	//PIXEL SHADERS//
	/////////////////

	SafeRelease(m_genericPS);
	SafeRelease(m_instancedGenericPS);
	SafeRelease(m_2dPS);
	SafeRelease(m_healthbarPS);
	SafeRelease(m_skyboxPS);
	SafeRelease(m_particlePS);

	SafeRelease(m_fadePS);
	SafeRelease(m_scrollingUVPS);
	SafeRelease(m_spherePS);
	SafeRelease(m_distortionWavePS);
	SafeRelease(m_bulletPS);
	SafeRelease(m_linePS);
	SafeRelease(m_instancedWhitePS);
	SafeRelease(m_ribbonPS);


	DXCall(m_device->CreatePixelShader(Generic_PS, sizeof(Generic_PS), nullptr, &m_genericPS));
	DXCall(m_device->CreatePixelShader(InstancedGeneric_PS, sizeof(InstancedGeneric_PS), nullptr, &m_instancedGenericPS));
	DXCall(m_device->CreatePixelShader(TwoD_PS, sizeof(TwoD_PS), nullptr, &m_2dPS));
	DXCall(m_device->CreatePixelShader(Healthbar_PS, sizeof(Healthbar_PS), nullptr, &m_healthbarPS));
	DXCall(m_device->CreatePixelShader(Skybox_PS, sizeof(Skybox_PS), nullptr, &m_skyboxPS));
	DXCall(m_device->CreatePixelShader(Particle_PS, sizeof(Particle_PS), nullptr, &m_particlePS));

	DXCall(m_device->CreatePixelShader(Sphere_PS, sizeof(Sphere_PS), nullptr, &m_spherePS));
	DXCall(m_device->CreatePixelShader(DistortionWave_PS, sizeof(DistortionWave_PS), nullptr, &m_distortionWavePS));
	DXCall(m_device->CreatePixelShader(Fade_PS, sizeof(Fade_PS), nullptr, &m_fadePS));
	DXCall(m_device->CreatePixelShader(ScrollingUV_PS, sizeof(ScrollingUV_PS), nullptr, &m_scrollingUVPS));
	DXCall(m_device->CreatePixelShader(Bullet_PS, sizeof(Bullet_PS), nullptr, &m_bulletPS));
	DXCall(m_device->CreatePixelShader(Line_PS, sizeof(Line_PS), nullptr, &m_linePS));
	DXCall(m_device->CreatePixelShader(Ribbon_PS, sizeof(Ribbon_PS), nullptr, &m_ribbonPS));
	m_device->CreatePixelShader(InstancedWhite_PS, sizeof(InstancedWhite_PS), nullptr, &m_instancedWhitePS);


	DXName(m_genericPS, "Generic Pixel Shader");
	DXName(m_instancedGenericPS, "Instanced Generic Pixel Shader");
	DXName(m_2dPS, "2D Pixel Shader");
	DXName(m_healthbarPS, "Healthbar Pixel Shader");
	DXName(m_skyboxPS, "Skybox Pixel Shader");
	DXName(m_particlePS, "Particle Pixel Shader");

	DXName(m_spherePS, "Sphere Pixel Shader");
	DXName(m_distortionWavePS, "Distortion Pixel Shader");
	DXName(m_fadePS, "Fade Pixel Shader");
	DXName(m_bulletPS, "Bullet Pixel Shader");
	DXName(m_linePS, "Line Pixel Shader");

	//////////////////
	//VERTEX SHADERS//
	//////////////////

	SafeRelease(m_genericVS);
	SafeRelease(m_instancedGenericVS);
	SafeRelease(m_instancedBillboardVS);
	SafeRelease(m_particleBillboardZVS);
	SafeRelease(m_2dVS);
	SafeRelease(m_skyboxVS);
	SafeRelease(m_particleVS);

	SafeRelease(m_scaleInstancedVS);
	SafeRelease(m_ribbonVS);
	SafeRelease(m_billboardVS);

	DXCall(m_device->CreateVertexShader(Generic_VS, sizeof(Generic_VS), nullptr, &m_genericVS));
	DXCall(m_device->CreateVertexShader(InstancedGeneric_VS, sizeof(InstancedGeneric_VS), nullptr, &m_instancedGenericVS));
	DXCall(m_device->CreateVertexShader(InstancedBillboard_VS, sizeof(InstancedBillboard_VS), nullptr, &m_instancedBillboardVS));
	DXCall(m_device->CreateVertexShader(ParticleBillboardZ_VS, sizeof(ParticleBillboardZ_VS), nullptr, &m_particleBillboardZVS));
	DXCall(m_device->CreateVertexShader(TwoD_VS, sizeof(TwoD_VS), nullptr, &m_2dVS));
	DXCall(m_device->CreateVertexShader(Skybox_VS, sizeof(Skybox_VS), nullptr, &m_skyboxVS));
	DXCall(m_device->CreateVertexShader(Particle_VS, sizeof(Particle_VS), nullptr, &m_particleVS));
	DXCall(m_device->CreateVertexShader(Ribbon_VS, sizeof(Ribbon_VS), nullptr, &m_ribbonVS));
	m_device->CreateVertexShader(InstancedScale_VS, sizeof(InstancedScale_VS), nullptr, &m_scaleInstancedVS);
	m_device->CreateVertexShader(Billboard_VS, sizeof(Billboard_VS), nullptr, &m_billboardVS);


	DXName(m_genericVS, "Generic Vertex Shader");
	DXName(m_instancedGenericVS, "Instanced Generic Vertex Shader");
	DXName(m_2dVS, "2D Vertex Shader");
	DXName(m_skyboxVS, "Skybox Vertex Shader");
	DXName(m_particleVS, "Particle Vertex Shader");


	///////////////////
	//COMPUTE SHADERS//
	///////////////////

	SafeRelease(m_initParticleCS);
	SafeRelease(m_updateParticleCS);
	SafeRelease(m_preParticleInitCS);

	DXCall(m_device->CreateComputeShader(PreParticleInit, sizeof(PreParticleInit), nullptr, &m_preParticleInitCS));
	DXCall(m_device->CreateComputeShader(InitParticle_CS, sizeof(InitParticle_CS), nullptr, &m_initParticleCS));
	DXCall(m_device->CreateComputeShader(Particle_CS, sizeof(Particle_CS), nullptr, &m_updateParticleCS));

	DXName(m_preParticleInitCS, "Particle Pre-Init Compute Shader");
	DXName(m_initParticleCS, "Particle Init Compute Shader");
	DXName(m_updateParticleCS, "Particle Update Compute Shader");

	SafeRelease(m_ribbonGS);
	DXCall(m_device->CreateGeometryShader(Ribbon_GS, sizeof(Ribbon_GS), nullptr, &m_ribbonGS));


}

void D3DGraphics::InitRenderInfo(std::string id, ID3D11PixelShader* ps, ID3D11VertexShader* vs, std::string meshId)
{
	if (meshId == "")
	{
		meshId = id;
	}

	RenderInfo* temp = new RenderInfo();

	temp->m_pixelShader = ps;
	temp->m_vertexShader = vs;
	temp->m_mesh = m_assetManager.m_meshes[meshId];

	InsertRenderInfo(temp, id);
}

void D3DGraphics::MakeViewMatrix()
{
	m_2DviewMatrix = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

void D3DGraphics::MakeSkyboxObject()
{
	m_skyboxObject = new RenderComp();
	m_skyboxObject->SetID("Skybox");

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subData;

	RenderInfo* desired = FindRenderInfo("Skybox");


	Mesh* skyboxMesh = desired->m_mesh = m_assetManager.m_meshes["Skybox"];

	XMFLOAT4 theSkybox[8];

	theSkybox[0] = XMFLOAT4(-1, 1, 1, 1);
	theSkybox[1] = XMFLOAT4(1, 1, 1, 1);
	theSkybox[2] = XMFLOAT4(-1, -1, 1, 1);
	theSkybox[3] = XMFLOAT4(1, -1, 1, 1);
	theSkybox[4] = XMFLOAT4(-1, 1, -1, 1);
	theSkybox[5] = XMFLOAT4(1, 1, -1, 1);
	theSkybox[6] = XMFLOAT4(-1, -1, -1, 1);
	theSkybox[7] = XMFLOAT4(1, -1, -1, 1);

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(XMFLOAT4) * 8;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	subData.pSysMem = theSkybox;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&bufferDesc, &subData, &skyboxMesh->m_vertexBuffer));

	DXName(skyboxMesh->m_vertexBuffer, "Skybox Vertex Buffer");

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(UINT) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	const unsigned int skyboxIndicies[36] = {
		0, 1, 2,
		2, 1, 3,
		1, 5, 3,
		3, 5, 7,
		5, 4, 7,
		7, 4, 6,
		4, 0, 6,
		6, 0, 2,
		4, 5, 0,
		0, 5, 1,
		2, 3, 6,
		6, 3, 7
	};

	subData.pSysMem = skyboxIndicies;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&bufferDesc, &subData, &skyboxMesh->m_indexBuffer));

	DXName(skyboxMesh->m_indexBuffer, "Skybox Index Buffer");

	skyboxMesh->m_numIndices = 36;

	skyboxMesh->m_stride = sizeof(XMFLOAT4);
	desired->m_pixelShader = m_skyboxPS;
	desired->m_vertexShader = m_skyboxVS;

}

void D3DGraphics::MakeInstanceBuffers()
{
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;

	m_instanceData = new InstanceData[m_instanceBufferSize];
	ZeroMemory(m_instanceData, sizeof(InstanceData) * m_instanceBufferSize);

	XMFLOAT4X4 modify;
	XMStoreFloat4x4(&modify, XMMatrixIdentity());

	m_instanceData[0].worldMatrix = modify;

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceData) * m_instanceBufferSize;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = m_instanceData;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer));
	DXName(m_instanceBuffer, "Generic Instance Buffer");


	m_particleInstanceData = new ParticleInstanceData[m_instanceBufferSize];
	ZeroMemory(m_particleInstanceData, sizeof(ParticleInstanceData) * m_instanceBufferSize);

	m_particleInstanceData[0].color = XMFLOAT4(1, 1, 1, 1);
	m_particleInstanceData[0].scale = 1;
	m_particleInstanceData[0].worldMatrix = modify;

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(ParticleInstanceData) * m_instanceBufferSize;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = m_particleInstanceData;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_particleInstanceBuffer));
	DXName(m_particleInstanceBuffer, "Particle Instance Buffer");
}


#pragma endregion Initialization Functions

void D3DGraphics::MapGamma(float input)
{

	if (input < 0)
	{
		input = 0;
	}
	else if (input > 1)
	{
		input = 1;
	}

	input = 1 - input;

	//referenced http://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another

	float input_start = 0; // The lowest number of the range input.
	float input_end = 1; // The lowest number of the range input.

	float output_start = 0.25f; // The lowest number of the range output.
	float output_end = 2.0f; // The largest number of the range ouput.

	GAMMA_VALUE = output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);

	m_pixelShaderData.gamma = GAMMA_VALUE;
}

#pragma region Update Functions

void D3DGraphics::Render(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthView)
{
	DXDrawName("Render Call");

	if (m_renderList == nullptr)
	{
		return;
	}

	if (rtv == m_rttRtv)
	{
		//Do this to make sure that the rttRtv is not bound to the pixel shader SRV
		m_context->PSSetShaderResources(0, 1, &FindRenderInfo("Skybox")->m_mesh->m_srv);
	}

	///////////////////
	//RTT FADE EFFECT//
	///////////////////

	XMFLOAT4X4 modify;

	XMStoreFloat4x4(&modify, XMMatrixIdentity());
	modify._41 = m_viewMatrix._41;
	modify._42 = m_viewMatrix._42;
	modify._43 = m_viewMatrix._43;

	m_context->OMSetRenderTargets(1, &rtv, depthView);


	if (rtv == m_rtv)
	{
		if ((m_pixelShaderData.fadeAmount < 1 || m_warpScreen) && m_rttSrv != nullptr)
		{

			m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_currentProjMatrix = m_orthoProjMatrix;

			m_context->OMSetDepthStencilState(m_2dDepthState, 1);
			m_context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

			m_context->IASetInputLayout(m_2dInputLayout);

			m_context->RSSetState(m_rastStateCullBack);


			modify._43 = m_viewMatrix._43 + 5.0f;
			m_vertexShaderData.worldMatrix = modify;

			if (m_fadeComp->m_renderInfo == nullptr)
			{
				Make2DObject(m_fadeComp);
			}

			Update2DObject(m_fadeComp);
			Draw2DObject(m_fadeComp);
			return;
		}
	}

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_context->OMSetDepthStencilState(m_3dDepthState, 1);
	m_context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	ID3D11ComputeShader* nullCS = nullptr;

	m_context->CSSetShader(nullCS, 0, 0);



	//////////
	//SKYBOX//
	//////////
	m_context->IASetInputLayout(m_skyboxInputLayout);

	m_currentProjMatrix = m_projMatrix;

	XMStoreFloat4x4(&modify, XMMatrixIdentity());
	modify._41 = m_viewMatrix._41;
	modify._42 = m_viewMatrix._42;
	modify._43 = m_viewMatrix._43;
	m_vertexShaderData.worldMatrix = modify;

	Draw3DObject(m_skyboxObject);

	m_context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	/////////////////////////////
	//INSTANCED OPAQUE GEOMETRY//
	/////////////////////////////
	XMStoreFloat4x4(&m_vertexShaderData.worldMatrix, XMMatrixIdentity());
	m_context->IASetInputLayout(m_instancedGenericInputLayout);

	DXDrawName("Instanced Opaque Geometry Render");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Opaque].size(); bucket++)
	{
		if ((*m_renderSize)[Opaque][bucket] > 0)
		{
			if (bucket == Player_Obj)
			{
				if (renderPlayer == false)
				{
					continue;
				}
			}

			if (bucket == Arrow_Lambda || bucket == Arrow_Omega || bucket == Arrow_Sigma || bucket == Pickup_Shield)
			{
				continue;
			}
			else if (bucket == Landing_Indicator)
			{
				m_pixelShaderData.cutAlpha = 2.0f;
			}
			else if (bucket == InnerStar || bucket == InnerStar2)
			{
				m_pixelShaderData.sphereScroll = 5;
			}

			if (bucket != eOBJECT_TYPE::Scenery_Obj)
			{
				for (int index = 0; index < (*m_renderSize)[Opaque][bucket]; index++)
				{
					m_instanceData[index].worldMatrix =
						(*m_renderList)[Opaque][bucket][index]->GetOwner()->GetTransform().GetWorldMatrix();


					//if (((RenderComp*)((*m_renderList)[Opaque][bucket][index]))->GetGlowID() != "")
					//{
					//	RenderComp temp;
					//	temp.SetID(((RenderComp*)((*m_renderList)[Opaque][bucket][index]))->GetGlowID());

					//	m_vertexShaderData.worldMatrix = m_instanceData[index].worldMatrix;

					//	m_context->OMSetDepthStencilState(m_transparentDepthState, 1);


					//	Draw3DObject(&temp);

					//	m_context->OMSetDepthStencilState(m_3dDepthState, 1);

					//}

				}

				D3D11_MAPPED_SUBRESOURCE msI;
				m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
				memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Opaque][bucket]);
				m_context->Unmap(m_instanceBuffer, 0);

				DrawInstanced3DObject((RenderComp*)(*m_renderList)[Opaque][bucket][0], (*m_renderSize)[Opaque][bucket], m_instanceBuffer, sizeof(InstanceData));
			}
			else
			{
				for (int index = 0; index < (*m_renderSize)[Opaque][bucket]; index++)
				{
					m_vertexShaderData.worldMatrix =
						(*m_renderList)[Opaque][bucket][index]->GetOwner()->GetTransform().GetWorldMatrix();

					Draw3DObject((RenderComp*)((*m_renderList)[Opaque][bucket][index]));
				}
			}

			if (bucket == Landing_Indicator)
			{
				m_pixelShaderData.cutAlpha = 0;
			}
			else if (bucket == InnerStar || bucket == InnerStar2)
			{
				m_pixelShaderData.sphereScroll = 0;
			}

		}

	}



	/////////////
	//LIGHTNING//
	/////////////
	m_context->IASetInputLayout(m_skyboxInputLayout);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	XMStoreFloat4x4(&m_vertexShaderData.worldMatrix, XMMatrixIdentity());

	DXDrawName("Lightning Render");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[RenderLightning].size(); bucket++)
	{

		for (int index = 0; index < (*m_renderSize)[RenderLightning][bucket]; index++)
		{

			RenderComp* temp = (RenderComp*)(*m_renderList)[RenderLightning][bucket][index];
			if (temp->m_renderInfo == nullptr)
				MakeLine(temp);

			DrawLine(temp);
		}

	}

	//////////////////////////////////
	//INSTANCED TRANSPARENT GEOMETRY//
	//////////////////////////////////
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_instancedGenericInputLayout);
	XMStoreFloat4x4(&m_vertexShaderData.worldMatrix, XMMatrixIdentity());

	//m_context->OMSetDepthStencilState(m_transparentDepthState, 1);

	DXDrawName("Instanced Transparent Geometry Render");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Transparent].size(); bucket++)
	{
		if ((*m_renderSize)[Transparent][bucket] > 0)
		{
			for (int index = 0; index < (*m_renderSize)[Transparent][bucket]; index++)
			{
				m_instanceData[index].worldMatrix =
					(*m_renderList)[Transparent][bucket][index]->GetOwner()->GetTransform().GetWorldMatrix();

			}

			D3D11_MAPPED_SUBRESOURCE msI;
			m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
			memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Transparent][bucket]);
			m_context->Unmap(m_instanceBuffer, 0);

			m_context->RSSetState(m_rastStateCullFront);
			DrawInstanced3DObject((RenderComp*)(*m_renderList)[Transparent][bucket][0], (*m_renderSize)[Transparent][bucket], m_instanceBuffer, sizeof(InstanceData));

			m_context->RSSetState(m_rastStateCullBack);
			DrawInstanced3DObject((RenderComp*)(*m_renderList)[Transparent][bucket][0], (*m_renderSize)[Transparent][bucket], m_instanceBuffer, sizeof(InstanceData));

		}
	}

	////////
	//GLOW//
	////////

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_instancedGenericInputLayout);
	XMStoreFloat4x4(&m_vertexShaderData.worldMatrix, XMMatrixIdentity());

	m_context->OMSetDepthStencilState(m_transparentDepthState, 1);

	//m_context->OMSetDepthStencilState(m_transparentDepthState, 1);

	DXDrawName("Glow Render");
	for (unsigned int bucket = 0; bucket < (*m_glowSize).size(); bucket++)
	{
		if ((*m_glowSize)[bucket] > 0)
		{
			for (int index = 0; index < (*m_glowSize)[bucket]; index++)
			{
				m_instanceData[index].worldMatrix =
					(*m_glowList)[bucket][index]->GetOwner()->GetTransform().GetWorldMatrix();

			}

			D3D11_MAPPED_SUBRESOURCE msI;
			m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
			memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_glowSize)[bucket]);
			m_context->Unmap(m_instanceBuffer, 0);

			//m_context->RSSetState(m_rastStateCullFront);
			//DrawInstanced3DObject((RenderComp*)(*m_glowList)[bucket][0], (*m_glowSize)[bucket], m_instanceBuffer, sizeof(InstanceData));

			RenderComp temp;
			temp.SetID(((GlowComponent*)(*m_glowList)[bucket][0])->GetGlowID());
			//temp.SetID("Bomber Glow");

			DrawInstanced3DObject(&temp, (*m_glowSize)[bucket], m_instanceBuffer, sizeof(InstanceData));

		}
	}

	//////////
	//ARROWS//
	//////////

	XMStoreFloat4x4(&m_vertexShaderData.worldMatrix, XMMatrixIdentity());
	m_context->IASetInputLayout(m_instancedGenericInputLayout);
	m_context->OMSetDepthStencilState(m_transparentDepthState, 1);


	if ((*m_renderSize)[Opaque][Arrow_Lambda] > 0)
	{

		for (int index = 0; index < (*m_renderSize)[Opaque][Arrow_Lambda]; index++)
		{
			m_instanceData[index].worldMatrix =
				(*m_renderList)[Opaque][Arrow_Lambda][index]->GetOwner()->GetTransform().GetWorldMatrix();
		}

		D3D11_MAPPED_SUBRESOURCE msI;
		m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
		memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Opaque][Arrow_Lambda]);
		m_context->Unmap(m_instanceBuffer, 0);

		DrawInstanced3DObject((RenderComp*)(*m_renderList)[Opaque][Arrow_Lambda][0], (*m_renderSize)[Opaque][Arrow_Lambda], m_instanceBuffer, sizeof(InstanceData));
	}
	if ((*m_renderSize)[Opaque][Arrow_Sigma] > 0)
	{

		for (int index = 0; index < (*m_renderSize)[Opaque][Arrow_Sigma]; index++)
		{
			m_instanceData[index].worldMatrix =
				(*m_renderList)[Opaque][Arrow_Sigma][index]->GetOwner()->GetTransform().GetWorldMatrix();
		}

		D3D11_MAPPED_SUBRESOURCE msI;
		m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
		memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Opaque][Arrow_Sigma]);
		m_context->Unmap(m_instanceBuffer, 0);

		DrawInstanced3DObject((RenderComp*)(*m_renderList)[Opaque][Arrow_Sigma][0], (*m_renderSize)[Opaque][Arrow_Sigma], m_instanceBuffer, sizeof(InstanceData));
	}
	if ((*m_renderSize)[Opaque][Arrow_Omega] > 0)
	{

		for (int index = 0; index < (*m_renderSize)[Opaque][Arrow_Omega]; index++)
		{
			m_instanceData[index].worldMatrix =
				(*m_renderList)[Opaque][Arrow_Omega][index]->GetOwner()->GetTransform().GetWorldMatrix();
		}

		D3D11_MAPPED_SUBRESOURCE msI;
		m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
		memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Opaque][Arrow_Omega]);
		m_context->Unmap(m_instanceBuffer, 0);

		DrawInstanced3DObject((RenderComp*)(*m_renderList)[Opaque][Arrow_Omega][0], (*m_renderSize)[Opaque][Arrow_Omega], m_instanceBuffer, sizeof(InstanceData));
	}

	//////////
	//SHIELD//
	//////////

	if (m_playerShieldShowing)
	{
		if ((*m_renderSize)[Opaque][Player_Obj] > 0)
		{

			for (int index = 0; index < (*m_renderSize)[Opaque][Player_Obj]; index++)
			{
				m_instanceData[index].worldMatrix =
					(*m_renderList)[Opaque][Player_Obj][index]->GetOwner()->GetTransform().GetWorldMatrix();

			}

			D3D11_MAPPED_SUBRESOURCE msI;
			m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
			memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Opaque][Player_Obj]);
			m_context->Unmap(m_instanceBuffer, 0);

			RenderComp temp;
			temp.SetID("Shield Effect");

			DrawInstanced3DObject(&temp, (*m_renderSize)[Opaque][Player_Obj], m_instanceBuffer, sizeof(InstanceData));
		}
	}

	if (m_bossShieldShowing)
	{
		if ((*m_renderSize)[Opaque][Enemy_Boss] > 0)
		{

			for (int index = 0; index < (*m_renderSize)[Opaque][Enemy_Boss]; index++)
			{
				m_instanceData[index].worldMatrix =
					(*m_renderList)[Opaque][Enemy_Boss][index]->GetOwner()->GetTransform().GetWorldMatrix();

				m_instanceData[index].worldMatrix._11 *= 3;
				m_instanceData[index].worldMatrix._12 *= 3;
				m_instanceData[index].worldMatrix._13 *= 3;
				m_instanceData[index].worldMatrix._21 *= 3;
				m_instanceData[index].worldMatrix._22 *= 3;
				m_instanceData[index].worldMatrix._23 *= 3;
				m_instanceData[index].worldMatrix._31 *= 4.0f;
				m_instanceData[index].worldMatrix._32 *= 4.0f;
				m_instanceData[index].worldMatrix._33 *= 4.0f;

			}

			D3D11_MAPPED_SUBRESOURCE msI;
			m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
			memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Opaque][Enemy_Boss]);
			m_context->Unmap(m_instanceBuffer, 0);

			RenderComp temp;
			temp.SetID("Boss Shield Effect");

			DrawInstanced3DObject(&temp, (*m_renderSize)[Opaque][Enemy_Boss], m_instanceBuffer, sizeof(InstanceData));
		}
	}

	if ((*m_renderSize)[Opaque][Pickup_Shield] > 0)
	{
		for (int index = 0; index < (*m_renderSize)[Opaque][Pickup_Shield]; index++)
		{
			m_instanceData[index].worldMatrix =
				(*m_renderList)[Opaque][Pickup_Shield][index]->GetOwner()->GetTransform().GetWorldMatrix();

			//if (((RenderComp*)((*m_renderList)[Opaque][Pickup_Shield][index]))->GetGlowID() != "")
			//{
			//	RenderComp temp;
			//	temp.SetID(((RenderComp*)((*m_renderList)[Opaque][Pickup_Shield][index]))->GetGlowID());

			//	m_vertexShaderData.worldMatrix = m_instanceData[index].worldMatrix;

			//	Draw3DObject(&temp);
			//}
		}

		D3D11_MAPPED_SUBRESOURCE msI;
		m_context->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
		memcpy(msI.pData, m_instanceData, sizeof(InstanceData) * (*m_renderSize)[Opaque][Pickup_Shield]);
		m_context->Unmap(m_instanceBuffer, 0);

		DrawInstanced3DObject((RenderComp*)(*m_renderList)[Opaque][Pickup_Shield][0], (*m_renderSize)[Opaque][Pickup_Shield], m_instanceBuffer, sizeof(InstanceData));
	}
	/////////////
	//PARTICLES//
	/////////////

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_instancedParticleInputLayout);
	m_context->OMSetDepthStencilState(m_transparentDepthState, 1);

	DXDrawName("Particle Render");

	if (m_emitterList)
	{
		for (auto iter : *m_emitterList)
		{
			for (int index = 0; index < iter->num_live_particles; index++)
			{

				iter->SetMolecule(&iter->m_data.mArray[index]);

				//XMFLOAT4X4 modify;
				//XMStoreFloat4x4(&modify, XMMatrixIdentity());

				//modify._41 = 80;

				m_particleInstanceData[index].worldMatrix = iter->m_Matrix.GetWorldMatrix();
				m_particleInstanceData[index].color = iter->m_data.mArray[index].m_Color;
				m_particleInstanceData[index].scale = iter->m_data.mArray[index].m_Scale;

			}

			D3D11_MAPPED_SUBRESOURCE msI;
			m_context->Map(m_particleInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msI);
			memcpy(msI.pData, m_particleInstanceData, sizeof(ParticleInstanceData) * iter->num_live_particles);
			m_context->Unmap(m_particleInstanceBuffer, 0);

			DrawInstanced3DObject(&iter->m_render, iter->num_live_particles, m_particleInstanceBuffer, sizeof(ParticleInstanceData));


		}
	}

	//////////
	//RIBBON//
	//////////
	m_context->OMSetDepthStencilState(m_3dDepthState, 1);

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_context->IASetInputLayout(m_ribbonInputLayout);
	m_currentProjMatrix = m_projMatrix;
	XMStoreFloat4x4(&m_vertexShaderData.worldMatrix, XMMatrixIdentity());

	m_context->RSSetState(m_rastStateCullNone);


	DXDrawName("Ribbon Render");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[RenderRibbon].size(); bucket++)
	{
		for (int index = 0; index < (*m_renderSize)[RenderRibbon][bucket]; index++)
		{

			RenderComp* temp = (RenderComp*)(*m_renderList)[RenderRibbon][bucket][index];
			if (temp->m_renderInfo == nullptr)
			{
				MakeRibbonObject(temp);
			}

			DrawRibbonObject(temp);
		}
	}

	m_context->GSSetShader(0, 0, 0);
	m_context->RSSetState(m_rastStateCullBack);

	/////////////////////
	//ANIMATED TEXTURES//
	/////////////////////

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_genericInputLayout);


	DXDrawName("Animated Texture Render");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[RenderAnimTex].size(); bucket++)
	{

		for (int index = 0; index < (*m_renderSize)[RenderAnimTex][bucket]; index++)
		{
			RenderComp* temp = (RenderComp*)(*m_renderList)[RenderAnimTex][bucket][index];
			if (temp->m_renderInfo == nullptr)
			{
				MakeAnimTexture(temp);
			}

			UpdateAnimTexture(temp);
			DrawAnimTexture(temp);

		}

	}




	///////////////////
	//2D HUD ELEMENTS//
	///////////////////

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_currentProjMatrix = m_orthoProjMatrix;

	m_context->OMSetDepthStencilState(m_2dDepthState, 1);
	m_context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_context->IASetInputLayout(m_2dInputLayout);

	m_context->RSSetState(m_rastStateCullBack);


	modify._43 = m_viewMatrix._43 + 5.0f;
	m_vertexShaderData.worldMatrix = modify;

	DXDrawName("2D Render Back");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Render2D_Back].size(); bucket++)
	{

		for (int index = 0; index < (*m_renderSize)[Render2D_Back][bucket]; index++)
		{
			RenderComp* temp = (RenderComp*)(*m_renderList)[Render2D_Back][bucket][index];

			if (temp->m_renderInfo == nullptr)
			{
				Make2DObject(temp);
			}

			Update2DObject(temp);
			Draw2DObject(temp);
		}


	}

	DXDrawName("2D Render Middle");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Render2D_Middle].size(); bucket++)
	{

		for (int index = 0; index < (*m_renderSize)[Render2D_Middle][bucket]; index++)
		{
			RenderComp* temp = (RenderComp*)(*m_renderList)[Render2D_Middle][bucket][index];

			if (temp->m_renderInfo == nullptr)
			{
				Make2DObject(temp);
			}

			Update2DObject(temp);
			Draw2DObject(temp);
		}
	}

	DXDrawName("2D Render Middle2");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Render2D_Middle2].size(); bucket++)
	{

		for (int index = 0; index < (*m_renderSize)[Render2D_Middle2][bucket]; index++)
		{
			RenderComp* temp = (RenderComp*)(*m_renderList)[Render2D_Middle2][bucket][index];

			if (temp->m_renderInfo == nullptr)
			{
				Make2DObject(temp);
			}

			Update2DObject(temp);
			Draw2DObject(temp);
		}
	}

	DXDrawName("2D Render Middle3");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Render2D_Middle3].size(); bucket++)
	{

		for (int index = 0; index < (*m_renderSize)[Render2D_Middle3][bucket]; index++)
		{
			RenderComp* temp = (RenderComp*)(*m_renderList)[Render2D_Middle3][bucket][index];

			if (temp->m_renderInfo == nullptr)
			{
				Make2DObject(temp);
			}

			Update2DObject(temp);
			Draw2DObject(temp);
		}
	}


	DXDrawName("2D Render Front");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Render2D_Front].size(); bucket++)
	{

		for (int index = 0; index < (*m_renderSize)[Render2D_Front][bucket]; index++)
		{
			RenderComp* temp = (RenderComp*)(*m_renderList)[Render2D_Front][bucket][index];

			if (temp->m_renderInfo == nullptr)
			{
				Make2DObject(temp);
			}

			Update2DObject(temp);
			Draw2DObject(temp);
		}

	}


	DXDrawName("2D Render Text");
	for (unsigned int bucket = 0; bucket < (*m_renderSize)[Render2D_Text].size(); bucket++)
	{
		for (int index = 0; index < (*m_renderSize)[Render2D_Text][bucket]; index++)
		{
			RenderComp* temp = (RenderComp*)(*m_renderList)[Render2D_Text][bucket][index];

			if (temp->GetID() != "")
			{
				if (temp->m_renderInfo == nullptr)
				{
					MakeFontObject(temp);

					if (temp->m_renderInfo != nullptr)
					{
						UpdateFontObject(temp);
						Draw2DObject(temp);
					}
				}
				else
				{
					UpdateFontObject(temp);
					Draw2DObject(temp);
				}
			}


		}
	}


	///////////////////
	//2D MOUSE CURSOR//
	///////////////////

	if (m_crosshairShowing)
	{

		if (m_mouseCursor->m_renderInfo == nullptr)
		{
			Make2DObject(m_mouseCursor);
		}

		Update2DObject(m_mouseCursor);
		Draw2DObject(m_mouseCursor);
	}



}

void D3DGraphics::HandleResize()
{

	if (m_chain == nullptr)
	{
		return;
	}

	std::cout << "RESIZE FUNCTION REACHED\n";

	m_context->ClearState();

	SafeRelease(m_rtv);
	SafeRelease(m_rttRtv);
	SafeRelease(m_rttTexture);
	SafeRelease(m_rttSrv);
	SafeRelease(m_depthBuffer);
	SafeRelease(m_rttDepthBuffer);
	SafeRelease(m_depthView);
	SafeRelease(m_rttDepthView);

	SafeRelease(m_blurRtv);
	SafeRelease(m_blurTexture);
	SafeRelease(m_blurSrv);

	if (!firstRun)
	{
		//OLD CODE
		//m_chain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, m_displayModeList[m_currMode].Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		//m_chain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		//NEW CODE
		m_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	}
	firstRun = false;

	DXGI_SWAP_CHAIN_DESC desc;
	m_chain->GetDesc(&desc);

	SCREEN_WIDTH = desc.BufferDesc.Width;
	SCREEN_HEIGHT = desc.BufferDesc.Height;


	static int lastMode = m_currMode;


	if ((desc.SampleDesc.Count != MSAA_LEVEL) || lastMode != m_currMode)
	{
		BOOL fullScreen;
		m_chain->GetFullscreenState(&fullScreen, nullptr);

		if (fullScreen)
		{
			m_chain->SetFullscreenState(false, nullptr);
		}

		desc.SampleDesc.Count = MSAA_LEVEL;
		desc.BufferDesc = m_displayModeList[m_currMode];

		SafeRelease(m_chain);

		IDXGIDevice * dxgiDevice = 0;
		DXCall(m_device->QueryInterface(__uuidof(IDXGIDevice), (void **)& dxgiDevice));

		IDXGIAdapter * dxgiAdapter = 0;
		DXCall(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)& dxgiAdapter));

		IDXGIFactory * dxgiFactory = 0;
		DXCall(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)& dxgiFactory));

		DXCall(dxgiFactory->CreateSwapChain(m_device, &desc, &m_chain));

		SafeRelease(dxgiDevice);
		SafeRelease(dxgiAdapter);
		SafeRelease(dxgiFactory);

		if (fullScreen)
		{
			DXCall(m_chain->SetFullscreenState(true, nullptr));
		}
	}

	lastMode = m_currMode;

	MakeRTVs();
	MakeDepthBuffers();
	MakeDepthViews();

	D3D11_VIEWPORT vp;
	MakeViewport(vp);

	MakeProjMatrices(50);

	m_context->RSSetViewports(1, &vp);

	ID3D11SamplerState* bufferPointers[2];

	bufferPointers[0] = m_sampleStateClampY;
	bufferPointers[1] = m_sampleStateWrap;


	m_context->PSSetSamplers(0, 2, bufferPointers);
	m_context->RSSetState(m_rastStateCullBack);
	m_context->VSSetConstantBuffers(0, 1, &m_vertexCBuffer);
	m_context->PSSetConstantBuffers(0, 1, &m_pixelCBuffer);
	m_context->OMSetBlendState(m_alphaBlendState, 0, 0xffffffff);

}

void D3DGraphics::UpdateConstantBuffers()
{
	D3D11_MAPPED_SUBRESOURCE msV;
	m_context->Map(m_vertexCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msV);
	memcpy(msV.pData, &m_vertexShaderData, sizeof(ToVertexShader));
	m_context->Unmap(m_vertexCBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE msP;
	m_context->Map(m_pixelCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msP);
	memcpy(msP.pData, &m_pixelShaderData, sizeof(ToPixelShader));
	m_context->Unmap(m_pixelCBuffer, 0);
}

void D3DGraphics::UpdateCursor()
{
	POINT mousePos;
	GetCursorPos(&mousePos);

	RECT screenRect;
	GetClientRect(m_hWnd, &screenRect);

	float width = (float)(screenRect.right - screenRect.left);
	float height = (float)(screenRect.bottom - screenRect.top);

	RECT offsetRect;
	offsetRect.bottom = 0;
	offsetRect.top = 0;
	offsetRect.left = 0;
	offsetRect.right = 0;

	static bool cursorShowing = true;

	//If the game is not currently running in windowed-borderless mode, compensate for the windows position
	if (m_bordersActive)
	{
		AdjustWindowRect(&offsetRect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, false);

		RECT windowRect;
		GetWindowRect(m_hWnd, &windowRect);

		windowRect.bottom -= offsetRect.bottom;
		windowRect.top -= offsetRect.top;
		windowRect.left -= offsetRect.left;
		windowRect.right -= offsetRect.right;


		if (((mousePos.x > windowRect.left) && (mousePos.x < windowRect.right)) && ((mousePos.y > windowRect.top) && (mousePos.y < windowRect.bottom)))
		{
			if (cursorShowing)
			{
				ShowCursor(false);
				cursorShowing = false;
			}
		}
		else
		{
			if (!cursorShowing)
			{
				ShowCursor(true);
				cursorShowing = true;
			}

		}
	}
	//Otherwise there is no need to compensate for the window offset
	else
	{
		if (cursorShowing)
		{
			ShowCursor(false);
			cursorShowing = false;
		}
	}

	if (FULL_SCREEN)
	{
		if (cursorShowing)
		{
			ShowCursor(false);
			cursorShowing = false;
		}
	}


	ScreenToClient(m_hWnd, &mousePos);

	float posX = (float)mousePos.x / width;
	float posY = (float)mousePos.y / height;

	MOUSE_X = posX;
	MOUSE_Y = posY;

	//Slightly adjust the render position so that the crosshair is centered around the true mouse position
	posX -= m_cursorHalfWidth;
	posY -= m_cursorHalfHeight;

	m_mouseCursor->m_posX = posX;
	m_mouseCursor->m_posY = posY;

}


#pragma endregion Update Functions

RenderInfo*	D3DGraphics::FindRenderInfo(std::string id)
{

	int index = HashString(id);
	return m_renderInfos[index];


}

int D3DGraphics::HashString(std::string s)
{

	int h = 0;
	char c[64];
	strcpy_s(c, s.c_str());
	int len = s.length();

	if (c[0] == 'B')
	{
		if (s == "Bomber" || s == "Bomber Glow")
		{
			h = 3;
		}
	}
	else if (c[0] == 'D')
	{
		if (s == "Distortion D Sphere" || s == "Dodger")
		{
			h = 3;
		}
	}


	for (int i = 0; i < len; ++i)
	{
		h = h * 31 + c[i];
	}


	h = h * 31 + c[(len >> 1)];

	return abs(h) % HashSize;
}



#pragma region Draw Functions

void D3DGraphics::Draw3DObject(RenderComp* object)
{
	///////////////////////////
	//UPDATE CONSTANT BUFFERS//
	///////////////////////////

	XMStoreFloat4x4(&m_vertexShaderData.viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewMatrix)));
	m_vertexShaderData.projMatrix = m_currentProjMatrix;

	m_pixelShaderData.color = object->color;

	UpdateConstantBuffers();

	//////////////////////////
	//SET VARIABLES AND DRAW//
	//////////////////////////

	UINT offset = 0;

	RenderInfo* desiredInfo = FindRenderInfo(object->GetID());

	if (m_currentVS != desiredInfo->m_vertexShader)
	{
		m_context->VSSetShader(desiredInfo->m_vertexShader, 0, 0);
		m_currentVS = desiredInfo->m_vertexShader;
	}

	if (m_currentPS != desiredInfo->m_pixelShader)
	{
		m_context->PSSetShader(desiredInfo->m_pixelShader, 0, 0);
		m_currentPS = desiredInfo->m_pixelShader;
	}

	m_context->PSSetShaderResources(0, 1, &desiredInfo->m_mesh->m_srv);
	m_context->IASetVertexBuffers(0, 1, &desiredInfo->m_mesh->m_vertexBuffer, &desiredInfo->m_mesh->m_stride, &offset);
	m_context->IASetIndexBuffer(desiredInfo->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_context->DrawIndexed(desiredInfo->m_mesh->m_numIndices, 0, 0);

}

void D3DGraphics::DrawInstanced3DObject(RenderComp* object, int numObjects, ID3D11Buffer* instanceBuffer, UINT instanceStride)
{
	///////////////////////////
	//UPDATE CONSTANT BUFFERS//
	///////////////////////////

	RenderInfo* desiredInfo = FindRenderInfo(object->GetID());


	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	strides[0] = desiredInfo->m_mesh->m_stride;
	strides[1] = instanceStride;

	offsets[0] = 0;
	offsets[1] = 0;

	bufferPointers[0] = desiredInfo->m_mesh->m_vertexBuffer;
	bufferPointers[1] = instanceBuffer;

	XMStoreFloat4x4(&m_vertexShaderData.viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewMatrix)));
	m_vertexShaderData.projMatrix = m_currentProjMatrix;

	m_pixelShaderData.color = object->color;



	UpdateConstantBuffers();

	//////////////////////////
	//SET VARIABLES AND DRAW//
	//////////////////////////

	if (m_currentVS != desiredInfo->m_vertexShader)
	{
		m_context->VSSetShader(desiredInfo->m_vertexShader, 0, 0);
		m_currentVS = desiredInfo->m_vertexShader;
	}

	if (m_currentPS != desiredInfo->m_pixelShader)
	{
		m_context->PSSetShader(desiredInfo->m_pixelShader, 0, 0);
		m_currentPS = desiredInfo->m_pixelShader;
	}
	//m_context->VSSetShader(m_renderInfos[object->GetID()]->m_vertexShader, 0, 0);
	//m_context->PSSetShader(m_renderInfos[object->GetID()]->m_pixelShader, 0, 0);
	m_context->PSSetShaderResources(0, 1, &desiredInfo->m_mesh->m_srv);
	m_context->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	m_context->IASetIndexBuffer(desiredInfo->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_context->DrawIndexedInstanced(desiredInfo->m_mesh->m_numIndices, numObjects, 0, 0, 0);


}

void D3DGraphics::Draw2DObject(RenderComp* object)
{
	///////////////////////////
	//UPDATE CONSTANT BUFFERS//
	///////////////////////////

	XMStoreFloat4x4(&m_vertexShaderData.viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_2DviewMatrix)));
	m_vertexShaderData.projMatrix = m_currentProjMatrix;

	m_pixelShaderData.color = object->color;


	UpdateConstantBuffers();

	//////////////////////////
	//SET VARIABLES AND DRAW//
	//////////////////////////

	if (object->GetID() == "Fade")
	{
		m_context->PSSetShaderResources(0, 1, &m_rttSrv);
	}
	else
	{
		m_context->PSSetShaderResources(0, 1, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_srv);
	}


	if (object->GetID() == "PylonEnergyFill1" || object->GetID() == "PylonEnergyFill2" || object->GetID() == "PylonEnergyFill3")
	{

		RenderInfo* desired = FindRenderInfo(object->GetID());

		if (m_currentPS != desired->m_pixelShader)
		{
			m_context->PSSetShader(desired->m_pixelShader, 0, 0);
			m_currentPS = desired->m_pixelShader;
		}
	}
	else
	{
		//m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);

		if (m_currentPS != ((RenderInfo*)object->m_renderInfo)->m_pixelShader)
		{
			m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);
			m_currentPS = ((RenderInfo*)object->m_renderInfo)->m_pixelShader;
		}

	}

	UINT offset = 0;

	if (m_currentVS != ((RenderInfo*)object->m_renderInfo)->m_vertexShader)
	{
		m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
		m_currentVS = ((RenderInfo*)object->m_renderInfo)->m_vertexShader;
	}

	//m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
	m_context->IASetVertexBuffers(0, 1, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_stride, &offset);
	m_context->IASetIndexBuffer(((RenderInfo*)object->m_renderInfo)->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_context->DrawIndexed(((RenderInfo*)object->m_renderInfo)->m_mesh->m_numIndices, 0, 0);

}

void D3DGraphics::DrawLine(RenderComp* object)
{
	UpdateLine(object);

	///////////////////////////
	//UPDATE CONSTANT BUFFERS//
	///////////////////////////

	XMStoreFloat4x4(&m_vertexShaderData.viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewMatrix)));
	m_vertexShaderData.projMatrix = m_currentProjMatrix;

	m_pixelShaderData.color = ((ElectricityEffect*)object->GetOwner())->GetColorAsXMFloat4();

	UpdateConstantBuffers();

	//////////////////////////
	//SET VARIABLES AND DRAW//
	//////////////////////////

	UINT offset = 0;

	if (m_currentVS != ((RenderInfo*)object->m_renderInfo)->m_vertexShader)
	{
		m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
		m_currentVS = ((RenderInfo*)object->m_renderInfo)->m_vertexShader;
	}

	if (m_currentPS != ((RenderInfo*)object->m_renderInfo)->m_pixelShader)
	{
		m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);
		m_currentPS = ((RenderInfo*)object->m_renderInfo)->m_pixelShader;
	}

	//m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
	//m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);
	m_context->IASetVertexBuffers(0, 1, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_stride, &offset);

	m_context->Draw(((RenderInfo*)object->m_renderInfo)->m_mesh->m_numVertices, 0);

}

void D3DGraphics::DrawAnimTexture(RenderComp* object)
{
	///////////////////////////
	//UPDATE CONSTANT BUFFERS//
	///////////////////////////

	XMStoreFloat4x4(&m_vertexShaderData.viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewMatrix)));
	m_vertexShaderData.projMatrix = m_currentProjMatrix;
	m_vertexShaderData.worldMatrix = object->GetOwner()->GetTransform().GetWorldMatrix();

	UpdateConstantBuffers();

	//////////////////////////
	//SET VARIABLES AND DRAW//
	//////////////////////////

	UINT offset = 0;

	if (m_currentVS != ((RenderInfo*)object->m_renderInfo)->m_vertexShader)
	{
		m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
		m_currentVS = ((RenderInfo*)object->m_renderInfo)->m_vertexShader;
	}

	if (m_currentPS != ((RenderInfo*)object->m_renderInfo)->m_pixelShader)
	{
		m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);
		m_currentPS = ((RenderInfo*)object->m_renderInfo)->m_pixelShader;
	}

	//m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
	//m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);
	m_context->PSSetShaderResources(0, 1, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_srv);
	m_context->IASetVertexBuffers(0, 1, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_stride, &offset);
	m_context->IASetIndexBuffer(((RenderInfo*)object->m_renderInfo)->m_mesh->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_context->DrawIndexed(((RenderInfo*)object->m_renderInfo)->m_mesh->m_numIndices, 0, 0);
}

void D3DGraphics::DrawRibbonObject(RenderComp* object)
{
	UpdateRibbonObject(object);

	///////////////////////////
	//UPDATE CONSTANT BUFFERS//
	///////////////////////////

	RibbonEffect* effect = (RibbonEffect*)object->GetOwner();

	XMStoreFloat4x4(&m_geometryShaderData.viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewMatrix)));
	m_geometryShaderData.projMatrix = m_currentProjMatrix;
	m_geometryShaderData.rotAxis = effect->GetRotAxis();
	m_geometryShaderData.width = effect->GetCurrentHalfWidth();
	m_geometryShaderData.color = effect->GrabTrailColor();

	UpdateConstantBuffers();

	m_context->GSSetConstantBuffers(0, 1, &m_geometryCBuffer);

	D3D11_MAPPED_SUBRESOURCE msG;
	m_context->Map(m_geometryCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msG);
	memcpy(msG.pData, &m_geometryShaderData, sizeof(ToGeometryShader));
	m_context->Unmap(m_geometryCBuffer, 0);

	//////////////////////////
	//SET VARIABLES AND DRAW//
	//////////////////////////

	UINT offset = 0;
	m_context->PSSetShaderResources(0, 1, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_srv);

	if (m_currentVS != ((RenderInfo*)object->m_renderInfo)->m_vertexShader)
	{
		m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
		m_currentVS = ((RenderInfo*)object->m_renderInfo)->m_vertexShader;
	}

	if (m_currentPS != ((RenderInfo*)object->m_renderInfo)->m_pixelShader)
	{
		m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);
		m_currentPS = ((RenderInfo*)object->m_renderInfo)->m_pixelShader;
	}

	//m_context->VSSetShader(((RenderInfo*)object->m_renderInfo)->m_vertexShader, 0, 0);
	//m_context->PSSetShader(((RenderInfo*)object->m_renderInfo)->m_pixelShader, 0, 0);
	m_context->GSSetShader(m_ribbonGS, 0, 0);
	m_context->IASetVertexBuffers(0, 1, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, &((RenderInfo*)object->m_renderInfo)->m_mesh->m_stride, &offset);

	m_context->Draw(effect->GetNumPairsInUse(), 0);

}

#pragma endregion Draw Functions

#pragma region Dynamic Object Functions

void D3DGraphics::MakeFontObject(RenderComp* object)
{
	if (object->GetID() == " ")
	{
		object->SetID("BUGGED");
	}

	if (m_bitmapFont == nullptr)
	{
		return;
	}

	int letter;
	int index = 0;
	int indexIndex = 0;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	RenderInfo* temp = new RenderInfo();
	temp->m_mesh = new Mesh();

	int numLetters = object->GetID().length();

	int numValidLetters = 0;

	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)object->GetID().c_str()[i]) - 32;

		if (letter != 0)
		{
			numValidLetters++;
		}
	}

	POS3UV_VERTEX* vertices = new POS3UV_VERTEX[numValidLetters * 4];
	UINT* indexes = new UINT[numValidLetters * 6];

	float posX = object->m_posX - (1 - object->m_posX);
	float posY = 1 - (object->m_posY * 2);


	float left = (SCREEN_WIDTH / 2) * posX;
	float top = (SCREEN_HEIGHT / 2) * posY;

	float xOffset = 0;

	float spacing = 1.5f;


	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)object->GetID().c_str()[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			xOffset += object->m_width * 60;
		}
		else
		{
			indexes[indexIndex] = index;
			indexIndex++;

			indexes[indexIndex] = index + 1;
			indexIndex++;

			indexes[indexIndex] = index + 2;
			indexIndex++;

			indexes[indexIndex] = index + 2;
			indexIndex++;

			indexes[indexIndex] = index + 1;
			indexIndex++;

			indexes[indexIndex] = index + 3;
			indexIndex++;

			/////////////////////
			//CALCULATE THE UVS//
			/////////////////////

			float startU = 0;
			float startV = 0;
			float endU = 0;
			float endV = 0;

			float letterWidth = 0;
			float letterHeight = 0;
			float yOffset = 0;


			startU = m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->x / 4096.0f;
			startV = m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->y / 4096.0f;
			endU = startU + (m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->width / 4096.0f);
			endV = startV + (m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->height / 4096.0f);

			letterWidth = (float)m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->width;
			letterHeight = (float)m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->height;
			yOffset = (float)m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->Yoffest;



			float trueWidth = SCREEN_WIDTH * object->m_width * 0.001f;
			float trueHeight = SCREEN_HEIGHT * object->m_height * 0.001f;



			vertices[index].position = XMFLOAT3(left + xOffset, top, 0.0f);  // Top left.
			vertices[index].texture = XMFLOAT2(startU, startV);
			index++;

			vertices[index].position = XMFLOAT3(left + xOffset + (letterWidth * trueWidth), top, 0.0f);  // Top right.
			vertices[index].texture = XMFLOAT2(endU, startV);
			index++;

			vertices[index].position = XMFLOAT3(left + xOffset, top - (letterHeight * trueHeight), 0.0f);  // Bottom left.
			vertices[index].texture = XMFLOAT2(startU, endV);
			index++;

			vertices[index].position = XMFLOAT3(left + xOffset + (letterWidth * trueWidth), top - (letterHeight * trueHeight), 0.0f);  // Bottom right.
			vertices[index].texture = XMFLOAT2(endU, endV);
			index++;

			xOffset += (letterWidth * trueWidth) + (spacing * trueWidth);

		}
	}

	temp->m_pixelShader = m_2dPS;
	temp->m_vertexShader = m_2dVS;

	temp->oldX = object->m_posX;
	temp->oldY = object->m_posY;
	temp->oldTexWidth = object->m_width;
	temp->oldTexHeight = object->m_height;
	temp->oldScreenHeight = SCREEN_HEIGHT;
	temp->oldScreenWidth = SCREEN_WIDTH;



	temp->m_mesh->m_stride = sizeof(POS3UV_VERTEX);


	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(POS3UV_VERTEX) * numValidLetters * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &temp->m_mesh->m_vertexBuffer);

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * numValidLetters * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indexes;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	m_device->CreateBuffer(&indexBufferDesc, &indexData, &temp->m_mesh->m_indexBuffer);

	temp->m_mesh->m_numIndices = numValidLetters * 6;
	temp->m_mesh->m_numVertices = numValidLetters * 4;

	temp->m_mesh->m_shortVertexArray = vertices;

	temp->m_mesh->m_srv = m_assetManager.m_meshes["Font"]->m_srv;

	object->m_renderInfo = temp;
	m_trashInfos.push_back(temp);

	delete[] indexes;

}

void D3DGraphics::UpdateFontObject(RenderComp* object)
{

	if (m_bitmapFont == nullptr)
	{
		return;
	}

	//This checks if the object needs to be updated or not
	if ((((RenderInfo*)object->m_renderInfo)->oldTexHeight == object->m_height)
		&& (((RenderInfo*)object->m_renderInfo)->oldTexWidth == object->m_width)
		&& (((RenderInfo*)object->m_renderInfo)->oldX == object->m_posX)
		&& (((RenderInfo*)object->m_renderInfo)->oldY == object->m_posY)
		&& (((RenderInfo*)object->m_renderInfo)->oldScreenHeight == SCREEN_HEIGHT)
		&& (((RenderInfo*)object->m_renderInfo)->oldScreenWidth == SCREEN_WIDTH))
	{
		return;
	}

	((RenderInfo*)object->m_renderInfo)->oldX = object->m_posX;
	((RenderInfo*)object->m_renderInfo)->oldY = object->m_posY;
	((RenderInfo*)object->m_renderInfo)->oldTexWidth = object->m_width;
	((RenderInfo*)object->m_renderInfo)->oldTexHeight = object->m_height;
	((RenderInfo*)object->m_renderInfo)->oldScreenWidth = SCREEN_WIDTH;
	((RenderInfo*)object->m_renderInfo)->oldScreenHeight = SCREEN_HEIGHT;

	float posX = object->m_posX - (1 - object->m_posX);
	float posY = 1 - (object->m_posY * 2);


	float left = (SCREEN_WIDTH / 2) * posX;
	float top = (SCREEN_HEIGHT / 2) * posY;
	float xOffset = 0;

	float spacing = 1.5f;
	int numLetters = object->GetID().length();

	int index = 0;

	int letter;

	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)object->GetID().c_str()[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			xOffset += object->m_width * 60;
		}
		else
		{
			/////////////////////
			//CALCULATE THE UVS//
			/////////////////////

			float startU = 0;
			float startV = 0;
			float endU = 0;
			float endV = 0;

			float letterWidth = 0;
			float letterHeight = 0;

			startU = m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->x / 4096.0f;
			startV = m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->y / 4096.0f;
			endU = startU + (m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->width / 4096.0f);
			endV = startV + (m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->height / 4096.0f);

			letterWidth = (float)m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->width;
			letterHeight = (float)m_bitmapFont->m_vLoadedLetters[(int)object->GetID().c_str()[i]]->height;


			float trueWidth = SCREEN_WIDTH * object->m_width * 0.001f;
			float trueHeight = SCREEN_HEIGHT * object->m_height * 0.001f;

			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].position = XMFLOAT3(left + xOffset, top, 0.0f);  // Top left.
			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].texture = XMFLOAT2(startU, startV);
			index++;

			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].position = XMFLOAT3(left + xOffset + (letterWidth * trueWidth), top, 0.0f);  // Top right.
			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].texture = XMFLOAT2(endU, startV);
			index++;

			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].position = XMFLOAT3(left + xOffset, top - (letterHeight * trueHeight), 0.0f);  // Bottom left.
			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].texture = XMFLOAT2(startU, endV);
			index++;

			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].position = XMFLOAT3(left + xOffset + (letterWidth * trueWidth), top - (letterHeight * trueHeight), 0.0f);  // Bottom right.
			((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray[index].texture = XMFLOAT2(endU, endV);
			index++;

			xOffset += (letterWidth * trueWidth) + (spacing * trueWidth);

		}
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_context->Map(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, ((RenderInfo*)object->m_renderInfo)->m_mesh->m_shortVertexArray, sizeof(POS3UV_VERTEX) * ((RenderInfo*)object->m_renderInfo)->m_mesh->m_numVertices);
	m_context->Unmap(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0);

}


void D3DGraphics::Make2DObject(RenderComp* object)
{

	RenderInfo* temp = new RenderInfo();
	temp->m_mesh = new Mesh();

	RenderInfo* desired = FindRenderInfo(object->GetID());

	if (object->GetID() != "Fade")
	{
		temp->m_mesh->m_srv = desired->m_mesh->m_srv;

	}
	//else
	//{
	//	//THIS DOESNT WORK AND I DONT KNOW WHY
	//	temp->m_mesh->m_srv = m_rttSrv;

	//}
	temp->m_pixelShader = desired->m_pixelShader;
	temp->m_vertexShader = desired->m_vertexShader;


	float posX = object->m_posX - (1 - object->m_posX);
	float posY = 1 - (object->m_posY * 2);

	float left, top, right, bottom;

	left = (SCREEN_WIDTH / 2) * posX;

	right = left + (SCREEN_WIDTH * object->m_width);

	top = (SCREEN_HEIGHT / 2) * posY;

	bottom = top - (SCREEN_HEIGHT * object->m_height);


	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	POS3UV_VERTEX vertices[4];

	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[1].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	const unsigned int indexes[6] = {
		0, 1, 2,
		2, 1, 3
	};

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(POS3UV_VERTEX) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &temp->m_mesh->m_vertexBuffer));

	DXName(temp->m_mesh->m_vertexBuffer, (object->GetID() + " Vertex Buffer"));


	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indexes;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&indexBufferDesc, &indexData, &temp->m_mesh->m_indexBuffer));

	DXName(temp->m_mesh->m_indexBuffer, (object->GetID() + " Index Buffer"));

	temp->m_mesh->m_numIndices = 6;

	temp->oldX = object->m_posX;
	temp->oldY = object->m_posY;
	temp->oldTexWidth = object->m_width;
	temp->oldTexHeight = object->m_height;
	temp->oldScreenWidth = SCREEN_WIDTH;
	temp->oldScreenHeight = SCREEN_HEIGHT;
	temp->m_mesh->m_stride = sizeof(POS3UV_VERTEX);

	object->m_renderInfo = temp;
	m_trashInfos.push_back(temp);

}

void D3DGraphics::Update2DObject(RenderComp* object)
{
	if (object->GetID() != "Fade")
	{
		((RenderInfo*)object->m_renderInfo)->m_mesh->m_srv = FindRenderInfo(object->GetID())->m_mesh->m_srv;
	}

	//This checks if the object needs to be updated or not
	if ((((RenderInfo*)object->m_renderInfo)->oldTexHeight == object->m_height)
		&& (((RenderInfo*)object->m_renderInfo)->oldTexWidth == object->m_width)
		&& (((RenderInfo*)object->m_renderInfo)->oldX == object->m_posX)
		&& (((RenderInfo*)object->m_renderInfo)->oldY == object->m_posY)
		&& (((RenderInfo*)object->m_renderInfo)->oldScreenHeight == SCREEN_HEIGHT)
		&& (((RenderInfo*)object->m_renderInfo)->oldScreenWidth == SCREEN_WIDTH))
	{
		return;
	}

	((RenderInfo*)object->m_renderInfo)->oldX = object->m_posX;
	((RenderInfo*)object->m_renderInfo)->oldY = object->m_posY;
	((RenderInfo*)object->m_renderInfo)->oldTexWidth = object->m_width;
	((RenderInfo*)object->m_renderInfo)->oldTexHeight = object->m_height;
	((RenderInfo*)object->m_renderInfo)->oldScreenWidth = SCREEN_WIDTH;
	((RenderInfo*)object->m_renderInfo)->oldScreenHeight = SCREEN_HEIGHT;

	float posX = object->m_posX - (1 - object->m_posX);
	float posY = 1 - (object->m_posY * 2);

	float left, top, right, bottom;

	left = (SCREEN_WIDTH / 2) * posX;

	right = left + (SCREEN_WIDTH * object->m_width);

	top = (SCREEN_HEIGHT / 2) * posY;

	bottom = top - (SCREEN_HEIGHT * object->m_height);


	POS3UV_VERTEX vertices[4];

	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[1].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	D3D11_MAPPED_SUBRESOURCE ms;
	m_context->Map(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, &vertices, sizeof(POS3UV_VERTEX) * 4);
	m_context->Unmap(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0);
}


void D3DGraphics::MakeRibbonObject(RenderComp* object)
{

	RenderInfo* desired = FindRenderInfo(object->GetID());

	RenderInfo* temp = new RenderInfo();
	temp->m_mesh = new Mesh();
	temp->m_mesh->m_srv = desired->m_mesh->m_srv;

	temp->m_pixelShader = desired->m_pixelShader;
	temp->m_vertexShader = desired->m_vertexShader;


	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;


	RibbonEffect * effect = ((RibbonEffect*)object->GetOwner());

	RibbonPair* vertices = nullptr;
	effect->GetPairs(vertices);

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(RibbonPair) * effect->GetNumPairsTotal();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &temp->m_mesh->m_vertexBuffer));
	DXName(temp->m_mesh->m_vertexBuffer, (object->GetID() + " Vertex Buffer"));

	temp->m_mesh->m_numIndices = 0;
	temp->m_mesh->m_numVertices = effect->GetNumPairsTotal();
	temp->m_mesh->m_stride = sizeof(RibbonPair);

	object->m_renderInfo = temp;
	m_trashInfos.push_back(temp);

}

void D3DGraphics::UpdateRibbonObject(RenderComp* object)
{
	RibbonEffect * effect = ((RibbonEffect*)object->GetOwner());

	RibbonPair* vertices;
	effect->GetPairs(vertices);

	D3D11_MAPPED_SUBRESOURCE ms;
	m_context->Map(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, vertices, ((RenderInfo*)object->m_renderInfo)->m_mesh->m_stride * effect->GetNumPairsInUse());
	m_context->Unmap(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0);
}


void D3DGraphics::MakeLine(RenderComp* object)
{
	RenderInfo* desired = FindRenderInfo(object->GetID());


	RenderInfo* temp = new RenderInfo();
	temp->m_mesh = new Mesh();

	temp->m_pixelShader = desired->m_pixelShader;
	temp->m_vertexShader = desired->m_vertexShader;

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subData;

	UINT count = ((ElectricityEffect*)object->GetOwner())->GetNumSegments() + 1;

	XMFLOAT4* data = new XMFLOAT4[count];
	Position* rawData = ((ElectricityEffect*)object->GetOwner())->GetPositions();

	for (UINT i = 0; i < count; i++)
	{
		data[i] = XMFLOAT4(rawData[i].x, rawData[i].y, rawData[i].z, 1);
	}

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(XMFLOAT4) * count;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	subData.pSysMem = data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&bufferDesc, &subData, &temp->m_mesh->m_vertexBuffer));

	DXName(temp->m_mesh->m_vertexBuffer, object->GetID() + " Vertex Buffer");

	temp->m_mesh->m_stride = sizeof(XMFLOAT4);
	temp->m_mesh->m_indexBuffer = nullptr;

	temp->m_mesh->m_posArray = data;
	temp->m_mesh->m_numVertices = count;

	object->m_renderInfo = temp;
	m_trashInfos.push_back(temp);
}

void D3DGraphics::UpdateLine(RenderComp* object)
{
	Position* rawData = ((ElectricityEffect*)object->GetOwner())->GetPositions();

	for (UINT i = 0; i < ((RenderInfo*)object->m_renderInfo)->m_mesh->m_numVertices; i++)
	{
		((RenderInfo*)object->m_renderInfo)->m_mesh->m_posArray[i] = XMFLOAT4(rawData[i].x, rawData[i].y, rawData[i].z, 1);

	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_context->Map(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, ((RenderInfo*)object->m_renderInfo)->m_mesh->m_posArray, ((RenderInfo*)object->m_renderInfo)->m_mesh->m_stride * ((RenderInfo*)object->m_renderInfo)->m_mesh->m_numVertices);
	m_context->Unmap(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0);
}


void D3DGraphics::MakeAnimTexture(RenderComp* object)
{

	RenderInfo* desired = FindRenderInfo(object->GetID());


	RenderInfo* temp = new RenderInfo();
	temp->m_mesh = new Mesh();

	temp->m_pixelShader = desired->m_pixelShader;
	temp->m_vertexShader = desired->m_vertexShader;
	temp->m_mesh->m_srv = desired->m_mesh->m_srv;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	POSUVNRM_VERTEX* vertices = new POSUVNRM_VERTEX[4];
	UINT* indexes = new UINT[6];
	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 2;
	indexes[4] = 1;
	indexes[5] = 3;

	float scale = ((AnimTexture*)object->GetOwner())->GrabScale();

	vertices[0].position = XMFLOAT4(-scale, scale, 0.0f, 1.0f);  // Top left.
	vertices[0].uv = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT4(scale, scale, 0.0f, 1.0f);   // Top right.
	vertices[1].uv = XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = XMFLOAT4(-scale, -scale, 0.0f, 1.0f);   // Bottom left.
	vertices[2].uv = XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = XMFLOAT4(scale, -scale, 0.0f, 1.0f);   // Bottom right.
	vertices[3].uv = XMFLOAT2(1.0f, 1.0f);

	temp->m_mesh->m_vertexArray = vertices;
	temp->m_mesh->m_numVertices = 4;

	temp->m_mesh->m_indexArray = indexes;
	temp->m_mesh->m_numIndices = 6;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(POSUVNRM_VERTEX) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &temp->m_mesh->m_vertexBuffer));

	DXName(temp->m_mesh->m_vertexBuffer, object->GetID() + " Vertex Buffer");

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indexes;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	DXCall(m_device->CreateBuffer(&indexBufferDesc, &indexData, &temp->m_mesh->m_indexBuffer));

	DXName(temp->m_mesh->m_indexBuffer, object->GetID() + " Index Buffer");

	temp->m_mesh->m_stride = sizeof(POSUVNRM_VERTEX);

	object->m_renderInfo = temp;
	m_trashInfos.push_back(temp);

}

void D3DGraphics::UpdateAnimTexture(RenderComp* object)
{
	XMFLOAT2* texcoords = ((AnimTexture*)object->GetOwner())->GrabUVs();

	for (UINT i = 0; i < ((RenderInfo*)object->m_renderInfo)->m_mesh->m_numVertices; i++)
	{
		((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexArray[i].uv = texcoords[i];
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_context->Map(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, ((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexArray, ((RenderInfo*)object->m_renderInfo)->m_mesh->m_stride * ((RenderInfo*)object->m_renderInfo)->m_mesh->m_numVertices);
	m_context->Unmap(((RenderInfo*)object->m_renderInfo)->m_mesh->m_vertexBuffer, 0);
}

#pragma endregion Dynamic Object Functions

#pragma region Public Functions

void D3DGraphics::SetCamera(GameObject* camera)
{
	m_camera = camera;
}

void D3DGraphics::SetRenderList(std::vector<std::vector<std::vector<BaseComponent*>>>* list, std::vector<std::vector<BaseComponent*>>* glowList)
{
	m_renderList = list;
	m_glowList = glowList;
}

void D3DGraphics::SetRenderSizes(std::vector<std::vector<int>> *sizes, std::vector<int> * glowSizes)
{
	m_renderSize = sizes;
	m_glowSize = glowSizes;
}

void D3DGraphics::SetRenderEmitters(std::list<MoleculeEmitter*> *list)
{
	m_emitterList = list;
}

void D3DGraphics::SetCrosshairShowing(bool showing)
{
	m_crosshairShowing = showing;
}

void D3DGraphics::SetPlayerShieldShowing(bool showing)
{
	m_playerShieldShowing = showing;
}

void D3DGraphics::SetBossShieldShowing(bool showing)
{
	m_bossShieldShowing = showing;
}

void D3DGraphics::SetScreenWarp(bool warp)
{
	m_warpScreen = warp;
}

void D3DGraphics::SetBitmapFont(BitmapFont* font)
{
	m_bitmapFont = font;
}

void D3DGraphics::SetPlayerWhite(bool white)
{

	if (white)
	{
		FindRenderInfo("Player")->m_pixelShader = m_instancedWhitePS;
	}
	else
	{
		FindRenderInfo("Player")->m_pixelShader = m_instancedGenericPS;

	}
}

void D3DGraphics::SetPylonWhite(UINT pylon, bool white)
{

	if (pylon == 1)
	{
		if (white)
		{
			FindRenderInfo("PylonEnergyFill1")->m_pixelShader = m_instancedWhitePS;
		}
		else
		{
			FindRenderInfo("PylonEnergyFill1")->m_pixelShader = m_2dPS;
		}
	}
	else if (pylon == 2)
	{
		if (white)
		{
			FindRenderInfo("PylonEnergyFill2")->m_pixelShader = m_instancedWhitePS;
		}
		else
		{
			FindRenderInfo("PylonEnergyFill2")->m_pixelShader = m_2dPS;
		}
	}
	else
	{
		if (white)
		{
			FindRenderInfo("PylonEnergyFill3")->m_pixelShader = m_instancedWhitePS;
		}
		else
		{
			FindRenderInfo("PylonEnergyFill3")->m_pixelShader = m_2dPS;
		}
	}

}


const HWND D3DGraphics::GetWindow()
{
	return m_hWnd;
}

UINT D3DGraphics::GetScreenHeight()
{
	return SCREEN_HEIGHT;
}

UINT D3DGraphics::GetScreenWidth()
{
	return SCREEN_WIDTH;
}

UINT D3DGraphics::GetMSAALevel()
{
	return MSAA_LEVEL;
}

Mesh* D3DGraphics::GetDistortionMesh(int& _outIndex, std::string levelTopography)
{
	Mesh** currArr;

	if (levelTopography == "Sphere")
		currArr = m_sphereDistortionMeshes;
	else if (levelTopography == "RoundedCube")
		currArr = m_roundedCubeDistortionMeshes;
	else if (levelTopography == "Torus")
		currArr = m_torusDistortionMeshes;

	for (int i = 0; i < 4; i++)
	{
		if (m_distortionUsed[i] == false)
		{
			m_distortionUsed[i] = true;
			_outIndex = i;
			return currArr[i];
		}
	}

	_outIndex = -1;
	return nullptr;
}

bool D3DGraphics::GetCrosshairShowing()
{
	return m_crosshairShowing;
}


void D3DGraphics::ReturnDistortionMesh(Mesh* mesh, std::string levelTopography)
{
	Mesh** currArr;

	if (levelTopography == "Sphere")
		currArr = m_sphereDistortionMeshes;
	else if (levelTopography == "RoundedCube")
		currArr = m_roundedCubeDistortionMeshes;
	else if (levelTopography == "Torus")
		currArr = m_torusDistortionMeshes;

	for (int i = 0; i < 4; i++)
	{
		if (currArr[i] == mesh)
		{
			m_distortionUsed[i] = false;
		}
	}
}

void D3DGraphics::SetResolution(int resolutionIndex)
{

	if (FULL_SCREEN)
	{
		m_chain->SetFullscreenState(false, NULL);
	}

	if (resolutionIndex < 0)
	{
		resolutionIndex = 0;
	}
	else if (((UINT)resolutionIndex) >= m_resolutionStates.size())
	{
		resolutionIndex = m_resolutionStates.size() - 1;
	}

	m_currMode = m_resolutionStates[resolutionIndex].modeIndex;


	UINT oldHeight = SCREEN_HEIGHT;
	UINT oldWidth = SCREEN_WIDTH;

	SCREEN_WIDTH = m_displayModeList[m_currMode].Width;
	SCREEN_HEIGHT = m_displayModeList[m_currMode].Height;

	std::cout << "\nNew Width: " << SCREEN_WIDTH << "\n";
	std::cout << "New Height: " << SCREEN_HEIGHT << "\n\n";

	m_chain->ResizeTarget(&m_displayModeList[m_currMode]);

	//If the window didnt change resolutions then the application will not recieve a WM_SIZE message
	if ((oldHeight == SCREEN_HEIGHT) && (oldWidth == SCREEN_WIDTH))
	{
		HandleResize();
	}

	if (m_bordersActive == false)
	{
		SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
		ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	}

	if (FULL_SCREEN)
	{
		m_chain->SetFullscreenState(true, NULL);
	}

}

void D3DGraphics::ToggleFullscreen()
{
	FULL_SCREEN = !FULL_SCREEN;

	SetFullscreen(FULL_SCREEN);
}

void D3DGraphics::SetFullscreen(bool fullScreen)
{

	FULL_SCREEN = fullScreen;

	BOOL isFullscreen;
	m_chain->GetFullscreenState(&isFullscreen, nullptr);

	if (isFullscreen == ((BOOL)FULL_SCREEN))
	{
		return;
	}


	if (!FULL_SCREEN)
	{

		m_chain->ResizeTarget(&m_displayModeList[m_currMode]);
		m_chain->SetFullscreenState(FULL_SCREEN, NULL);
		m_chain->SetFullscreenState(!FULL_SCREEN, NULL);
		m_chain->SetFullscreenState(FULL_SCREEN, NULL);

		if (m_bordersActive)
		{
			//WINDOWED
			SetWindowLong(m_hWnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
			SetWindowPos(m_hWnd, m_hWnd, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
			ShowWindow(m_hWnd, SW_SHOWNORMAL);

		}
		else
		{
			//BORDERLESS
			SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
			ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);

		}

	}
	else
	{

		m_chain->ResizeTarget(&m_displayModeList[m_currMode]);
		m_chain->SetFullscreenState(FULL_SCREEN, NULL);

		//m_output->SetGammaControl(&m_gamma);
	}


}

void D3DGraphics::ToggleBorders()
{

	m_bordersActive = !m_bordersActive;

	if (m_bordersActive)
	{
		//WINDOWED
		SetWindowLong(m_hWnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
		SetWindowPos(m_hWnd, m_hWnd, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		ShowWindow(m_hWnd, SW_SHOWNORMAL);

	}
	else
	{
		//BORDERLESS
		SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
		ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);

	}

}

void D3DGraphics::FadeOut()
{

	m_fadeIn = false;

	while (m_pixelShaderData.fadeAmount > 0)
	{
		m_Xtime->Signal();
		static DWORD prevCount = GetTickCount();
		float deltaTime = (GetTickCount() - prevCount) / 1000.0f;
		prevCount = GetTickCount();

		m_pixelShaderData.fadeAmount -= (float)(m_fadeFactor * m_Xtime->Delta());
		Frame((float)m_Xtime->Delta());
	}

	m_fadeIn = true;

}

const Mesh* D3DGraphics::GetMesh(std::string meshName)
{
	auto iter = m_assetManager.m_meshes.find(meshName);

	if (iter == m_assetManager.m_meshes.end())
	{
		return nullptr;
	}
	else
	{
		return m_assetManager.m_meshes[meshName];
	}
}

XMVECTOR D3DGraphics::Unproject(XMVECTOR vec)
{
	XMMATRIX view, proj, viewProj;

	view = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewMatrix));
	proj = XMLoadFloat4x4(&m_projMatrix);

	viewProj = XMMatrixInverse(nullptr, XMMatrixMultiply(view, proj));

	XMVECTOR temp;

	temp = XMVector3Transform(vec, viewProj);

	temp /= temp.m128_f32[3];

	return temp;
}

#pragma endregion Public Functions








