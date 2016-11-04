#pragma once
#include "../../stdafx.h"
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <DirectXMath.h>

struct POSUVNRM_VERTEX
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
};

struct POS3UV_VERTEX
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
};

struct Mesh
{
	UINT m_numIndices;
	UINT m_numVertices;

	UINT* m_indexArray = nullptr;
	POSUVNRM_VERTEX * m_vertexArray = nullptr;
	POS3UV_VERTEX* m_shortVertexArray = nullptr;

	DirectX::XMFLOAT4* m_posArray = nullptr;

	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	UINT m_stride;
};



class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	void LoadAssets();
	void Initialize(ID3D11Device* m_device);

	void Shutdown();

	std::unordered_map<std::string, Mesh*>	m_meshes;

private:

	ID3D11Device*					m_device;

	void LoadModel(Mesh *object, const wchar_t* textureFileName, const char* modelFileName);
	void LoadDynamicModel(Mesh *object, const wchar_t* textureFileName, const char* modelFileName);
	void LoadTexture(Mesh *object, const wchar_t* textureFileName);
	void LoadBillboard(Mesh* object, const wchar_t* textureFileName, float size = 3);
	void LoadDynamicBillboard(Mesh* object, const wchar_t* textureFileName);

};

