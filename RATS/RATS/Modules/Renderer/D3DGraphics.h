#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

#include "../Asset Manager/AssetManager.h"
#include "RenderComp.h"



#include "../../Wwise files/EventManager.h"
#include "../../Wwise files/Transform.h"
#include "../../Dependencies/Paramesium/Include/steam_api.h"
#include "../Input/InputManager.h"
#include "../Input/MessageSystem.h"


// Grants Testing Includes
#include "../Object Manager/ObjectFactory.h"

#include "../BitmapFont/BitmapFont.h"
#include "../VFX/RibbonEffect/RibbonEffect.h"
#include "../Molecules/MoleculeEmitter.h"


//DONT CHANGE THIS D:
#define HashSize 1750


struct ToVertexShader
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
	float pixelSize;
	float pad[3];
};

struct ToGeometryShader
{
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
	XMFLOAT3 rotAxis;
	float width;
	XMFLOAT4 color;
};

struct ToPixelShader
{

	float fadeAmount;
	float playerHealth = 1;
	float currTime = 0;
	float warpAmount;
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1);

	float screenWarp;
	float cutAlpha;
	float gamma;
	float sphereScroll;

};

struct InstanceData
{
	DirectX::XMFLOAT4X4 worldMatrix;
};

struct ParticleInstanceData
{
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1,1,1,1);
	float scale = 1;
	DirectX::XMFLOAT4X4 worldMatrix = DirectX::XMFLOAT4X4(1,0,0,0,
														  0,1,0,0,
														  0,0,1,0,
														  0,0,0,1);

};



struct RenderInfo
{
	ID3D11PixelShader*				m_pixelShader;
	ID3D11VertexShader*				m_vertexShader;

	Mesh*							m_mesh;
	//For 2D ONLY
	float oldX;
	float oldY;
	float oldTexWidth;
	float oldTexHeight;
	int oldScreenWidth;
	int oldScreenHeight;

};

struct RenderObject
{
	RenderInfo*						m_info;
	RenderComp*						m_component;

};

struct VFXInfo : public RenderInfo
{
	ID3D11ComputeShader*			m_computeShaders[3];
	ID3D11Buffer*					m_particleBuffer;
	UINT							m_threadAxis;
	ID3D11UnorderedAccessView*		m_accessView;
};

struct MSAA_State
{
	string m_displayString;	
	int m_msaaValue;
};

struct Screen_State
{
	string m_displayString;
	bool bordersActive;
	bool fullScreen;
};

struct Resolution_State
{
	string m_displayString;
	UINT width;
	UINT height;

	int modeIndex;

};

struct Vsync_State
{
	string m_displayString;
	bool vsync;
};





class D3DGraphics
{
public:

	bool FULL_SCREEN = true;
	bool VSYNC_ENABLED = false;
	float SCREEN_DEPTH = 1000;
	float SCREEN_NEAR = 0.1f;
	int SCREEN_WIDTH = 1920;
	int SCREEN_HEIGHT = 1080;
	float GAMMA_VALUE = 1.125f;

	float MOUSE_X = 0;
	float MOUSE_Y = 0;

	int FRAME_RATE = 60;

	bool justBeatTheGame = false;


	bool m_bordersActive = true;

	float m_cursorHalfWidth = 0.024f;
	float m_cursorHalfHeight = 0.025f;


	vector<XMFLOAT3> m_fireworkPositions;

	RenderComp* m_loseText = nullptr;

	bool renderPlayer = true;



	//MUST be 1, 2, 4, or 8
	int MSAA_LEVEL = 8;
	const int m_instanceBufferSize = 500;
	InstanceData* m_instanceData;
	ParticleInstanceData* m_particleInstanceData;


	AssetManager m_assetManager;

	D3DGraphics();
	D3DGraphics(const D3DGraphics &copy);
	~D3DGraphics();

	bool Initialize(HINSTANCE hInstance);
	void Shutdown();

	void Frame(float deltaTime);

	void HandleResize();

	void SetResolution(int resolutionIndex);

	void ToggleFullscreen();

	void SetFullscreen(bool fullScreen);
	void ToggleBorders();

	void			MapGamma(float input);


	void FadeOut();
	//void FadeIn();

	const HWND GetWindow();

	UINT GetScreenHeight();
	UINT GetScreenWidth();

	void SetScreenWarp(bool screenWarp);

	UINT GetMSAALevel();

	void FlushOldMemory();


	//void IncreaseMSAALevel();
	//void DecreaseMSAALevel();

	Mesh*			GetDistortionMesh(int& _outIndex, std::string levelTopography);
	void			ReturnDistortionMesh(Mesh* mesh, std::string levelTopography);

	void SetPlayerWhite(bool white);

	DirectX::XMVECTOR D3DGraphics::Unproject(DirectX::XMVECTOR vec);

	const Mesh* GetMesh(std::string meshName);

	//void SetRenderList(std::vector<std::vector<std::list<BaseComponent*>>>* list);
	
	void SetRenderList(std::vector<std::vector<std::vector<BaseComponent*>>>* list, std::vector<std::vector<BaseComponent*>>* glowList);
	void SetRenderSizes(std::vector<std::vector<int>> *sizes, std::vector<int> * glowSizes);
	void SetRenderEmitters(std::list<MoleculeEmitter*> *list);
	void SetCamera(GameObject* camera);
	void SetCrosshairShowing(bool showing);
	bool GetCrosshairShowing();

	void SetPlayerShieldShowing(bool showing);
	void SetBossShieldShowing(bool showing);


	void SetBitmapFont(BitmapFont* font);

	void SetPSPlayerHealth(float newval) { m_pixelShaderData.playerHealth = newval; }

	void			SetPylonWhite(UINT pylon, bool white);

	ID3D11DeviceContext* GetContext(){ return m_context; }

	// HashString is now public
	int				HashString(std::string id);


	MSAA_State			GetMSAAState(int index);
	Screen_State		GetScreenState(int index);
	Vsync_State			GetVsyncState(int index);
	Resolution_State	GetResolutionState(int index);
	UINT				GetNumResolutions();

	void ApplyGraphicsSettings(int msaaIndex, int screenIndex, int resolutionIndex, int vsyncIndex, float gamma = 0.5f);

private:	

	bool firstRun = true;

	IDXGIOutput* m_output;

	vector<MSAA_State> m_msaaLevels;
	vector<Screen_State> m_screenStates;
	vector<Resolution_State> m_resolutionStates;
	vector<Vsync_State> m_vsyncStates;


	Mesh* m_sphereDistortionMeshes[4];
	Mesh* m_roundedCubeDistortionMeshes[4];
	Mesh* m_torusDistortionMeshes[4];
	bool m_distortionUsed[4];


	bool m_warpScreen;

	bool m_playerShieldShowing;
	bool m_bossShieldShowing;

	bool m_crosshairShowing;
	BitmapFont*							m_bitmapFont;



	bool m_fadeIn;

	float m_fadeFactor = 2.0f;


	ID3D11PixelShader*					m_currentPS;
	ID3D11VertexShader*					m_currentVS;


	

	/////////////////
	//UNIVERSAL USE//
	/////////////////

	HWND								m_hWnd;
	HINSTANCE							m_hInst;
	WNDPROC								m_appWndProc;

	ID3D11Device*						m_device;
	IDXGISwapChain*						m_chain;
	ID3D11DeviceContext*				m_context;

	int									m_videoCardMemory;
	DXGI_MODE_DESC*						m_displayModeList;
	int									m_numDisplayModes;
	int									m_currMode;

	ID3D11Buffer*						m_vertexCBuffer;
	ID3D11Buffer*						m_pixelCBuffer;
	ID3D11Buffer*						m_geometryCBuffer;
	ID3D11Buffer*						m_initComputeCBuffer;
	ID3D11Buffer*						m_updateComputeCBuffer;

	ID3D11Buffer*						m_instanceBuffer;
	ID3D11Buffer*						m_particleInstanceBuffer;


	ToVertexShader						m_vertexShaderData;
	ToPixelShader						m_pixelShaderData;
	ToGeometryShader					m_geometryShaderData;
//	ToInitComputeShader					m_initComputeShaderData;
//	ToUpdateComputeShader				m_updateComputeShaderData;

	GameObject*												m_camera;
	std::vector<std::vector<std::vector<BaseComponent*>>>*	m_renderList;
	std::vector<std::vector<int>>*							m_renderSize;
	std::list<MoleculeEmitter*>*							m_emitterList;

	std::vector<std::vector<BaseComponent*>>*	m_glowList;
	std::vector<int>*							m_glowSize;

	std::vector<RenderInfo*>								m_renderInfos;

	//std::unordered_map<std::string, RenderInfo*>			m_renderInfos;
	std::list<RenderInfo*>									m_trashInfos;
//	std::list<ParticleBufferInfo*>							m_trashParticleBuffers;

	//////////////
	//MAIN SCENE//
	//////////////

	ID3D11RenderTargetView*				m_rtv;
	ID3D11Texture2D*					m_depthBuffer;
	ID3D11DepthStencilView*				m_depthView;

	/////////////////////
	//RENDER TO TEXTURE//
	/////////////////////

	ID3D11Texture2D*					m_rttTexture;
	ID3D11RenderTargetView*				m_rttRtv;
	ID3D11ShaderResourceView*			m_rttSrv;
	ID3D11Texture2D*					m_rttDepthBuffer;
	ID3D11DepthStencilView*				m_rttDepthView;

	ID3D11Texture2D*					m_blurTexture;
	ID3D11RenderTargetView*				m_blurRtv;
	ID3D11ShaderResourceView*			m_blurSrv;


	ID3D11SamplerState*					m_sampleStateClampY;
	ID3D11SamplerState*					m_sampleStateWrap;
	ID3D11RasterizerState*				m_rastStateCullBack;
	ID3D11RasterizerState*				m_rastStateCullFront;
	ID3D11RasterizerState*				m_rastStateCullNone;
	ID3D11BlendState*					m_alphaBlendState;
	//ID3D11BlendState*					m_additiveBlendState;


	ID3D11DepthStencilState*			m_3dDepthState;
	ID3D11DepthStencilState*			m_2dDepthState;
	ID3D11DepthStencilState*			m_transparentDepthState;

	DirectX::XMFLOAT4X4					m_currentProjMatrix;

	DirectX::XMFLOAT4X4					m_projMatrix;
	DirectX::XMFLOAT4X4					m_orthoProjMatrix;

	DirectX::XMFLOAT4X4					m_viewMatrix;
	DirectX::XMFLOAT4X4					m_2DviewMatrix;


	/////////////////
	//PIXEL SHADERS//
	//COUNT: 14    //
	/////////////////

	ID3D11PixelShader*					m_skyboxPS;
	ID3D11PixelShader*					m_genericPS;
	ID3D11PixelShader*					m_instancedGenericPS;
	ID3D11PixelShader*					m_2dPS;
	ID3D11PixelShader*					m_healthbarPS;
	ID3D11PixelShader*					m_spherePS;
	ID3D11PixelShader*					m_distortionWavePS;
	ID3D11PixelShader*					m_fadePS;
	ID3D11PixelShader*					m_scrollingUVPS;
	ID3D11PixelShader*					m_bulletPS;
	ID3D11PixelShader*					m_linePS;
	ID3D11PixelShader*					m_particlePS;
	ID3D11PixelShader*					m_instancedWhitePS;
	ID3D11PixelShader*					m_ribbonPS;

	//////////////////
	//VERTEX SHADERS//
	//COUNT: 9      //
	//////////////////

	ID3D11VertexShader*					m_skyboxVS;
	ID3D11VertexShader*					m_genericVS;
	ID3D11VertexShader*					m_instancedGenericVS;
	ID3D11VertexShader*					m_instancedBillboardVS;
	ID3D11VertexShader*					m_particleBillboardZVS;
	ID3D11VertexShader*					m_ribbonVS;
	ID3D11VertexShader*					m_2dVS;
	ID3D11VertexShader*					m_particleVS;
	ID3D11VertexShader*					m_scaleInstancedVS;
	ID3D11VertexShader*					m_billboardVS;

	/////////////////
	//CS/GS SHADERS//
	//COUNT: 4     //
	/////////////////

	ID3D11ComputeShader*				m_preParticleInitCS;
	ID3D11ComputeShader*				m_initParticleCS;
	ID3D11ComputeShader*				m_updateParticleCS;

	ID3D11GeometryShader*				m_ribbonGS;

	//////////////////
	//INPUT LAYOUTS //
	//COUNT: 6      //
	//////////////////

	ID3D11InputLayout*					m_skyboxInputLayout;
	ID3D11InputLayout*					m_genericInputLayout;
	ID3D11InputLayout*					m_instancedGenericInputLayout;
	ID3D11InputLayout*					m_2dInputLayout;
	ID3D11InputLayout*					m_instancedParticleInputLayout;
	ID3D11InputLayout*					m_ribbonInputLayout;     








	RenderComp*							m_skyboxObject;
	RenderComp*							m_fadeComp;
	RenderComp*							m_mouseCursor;

	/////////////
	//FUNCTIONS//
	/////////////

	void			Draw3DObject(RenderComp* object);
	void			Draw2DObject(RenderComp* object);
	void			DrawInstanced3DObject(RenderComp* object, int numObjects, ID3D11Buffer* instanceBuffer, UINT instanceStride);
	void			DrawLine(RenderComp* object);
	void			DrawAnimTexture(RenderComp* object);
	void			DrawRibbonObject(RenderComp* object);


	void			MakeWindow(HINSTANCE hInstance, WNDPROC proc);
	HRESULT			MakeDeviceAndChain();
	HRESULT			MakeRTVs();
	HRESULT			MakeDepthBuffers();
	HRESULT			MakeDepthStates();
	HRESULT			MakeDepthViews();
	HRESULT			MakeRastState();
	HRESULT			MakeInputLayouts();
	HRESULT			MakeConstantBuffers();
	HRESULT			MakeSampleState();
	HRESULT			MakeBlendState();
	void			MakeViewport(D3D11_VIEWPORT &viewport);
	void			MakeProjMatrices(float fov);
	void			MakeViewMatrix();
	void			MakeInstanceBuffers();
	void			MakeShaders();
	void			MakeSkyboxObject();
	void			MakeRenderInfos();
	void			Make2DObject(RenderComp* object);
	void			MakeRibbonObject(RenderComp* object);
	void			MakeFontObject(RenderComp* object);
	void			MakeLine(RenderComp* object);
	void			UpdateLine(RenderComp* object);
	void			MakeAnimTexture(RenderComp* object);
	void			UpdateAnimTexture(RenderComp* object);
	void			UpdateFontObject(RenderComp* object);
	void			UpdateRibbonObject(RenderComp* object);



	RenderInfo*		FindRenderInfo(std::string id);
//	int				HashString(std::string id);
	void			InsertRenderInfo(RenderInfo* info, std::string id);


	void			InitRenderInfo(std::string id, ID3D11PixelShader* ps, ID3D11VertexShader* vs, std::string meshId = "");




	//void			MakeParticleBuffers(RenderComp* object);
	//void			UpdateParticleBuffers(RenderComp* object);
	void			UpdateConstantBuffers();

	void			Render(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthView);


	void			UpdateCursor();

	HRESULT			DisableAltEnter();
	void			Update2DObject(RenderComp* object);
};

