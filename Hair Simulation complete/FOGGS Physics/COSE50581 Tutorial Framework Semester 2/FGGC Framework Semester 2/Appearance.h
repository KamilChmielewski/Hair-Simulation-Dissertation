#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include "ConstantBuffers.h"

using namespace DirectX;

struct Geometry
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numberOfIndices;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;
};

struct Material
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	float specularPower;
};

class Appearance
{
private:
	Geometry _geometry;
	Material _material;
	ID3D11ShaderResourceView* _textureRV;
	ID3D11DeviceContext* _pImmediateContext;
	Mesh* _model;

public:
	Appearance(Geometry geometry, Material material, ID3D11DeviceContext* pImmediateContext);
	~Appearance();
	void SetTextureRV(ID3D11ShaderResourceView* textureRV) { _textureRV = textureRV; }
	bool HasTexture() const { return _textureRV ? true : false; }
	
	Geometry GetGeometryData() const { return _geometry; }
	Material GetMaterial() const { return _material; }
	
	ID3D11ShaderResourceView* GetTextureRV() const { return _textureRV; }
	void SetMesh(Mesh* model) { _model = model; }

	void Draw();

	void DrawLines();

	void DrawObj();

	void DrawHairStrand();
};