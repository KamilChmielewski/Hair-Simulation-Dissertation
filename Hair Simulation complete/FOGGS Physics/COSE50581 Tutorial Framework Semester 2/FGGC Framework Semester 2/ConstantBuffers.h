#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include "Vector3D.h"

using namespace DirectX;

struct SurfaceInfo
{
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 SpecularMtrl;
};

struct Light
{
	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;

	float SpecularPower;
	XMFLOAT3 LightVecW;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	SurfaceInfo surface;

	Light light;

	XMFLOAT3 EyePosW;
	float HasTexture;
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

struct PosVertex
{
	XMFLOAT3 pos;
};

struct Textures
{
	float u, v;
};

struct Mesh
{
	std::vector<Vertex> elements;
	std::vector<unsigned short> Indices;
	unsigned short vertexIndex[3], uvIndex[3], normalIndex[3];
	std::vector<unsigned short> vertexIndices, uvIndices, normalIndices;
	std::vector<XMFLOAT3> indexed_vertices;
	std::vector<XMFLOAT2> indexed_uvs;
	std::vector<XMFLOAT3> indexed_normals;

	ID3D11Buffer* _pVertexBuffer;
	ID3D11Buffer* _pIndexBuffer;
};

struct PackedVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
	//Overloading < function
	bool operator <(const PackedVertex that) const { return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0; } //So if that is greater than this then return true
};

struct ParticleStruct
{
	XMFLOAT4 pos;
	XMFLOAT4 velocity;
	XMFLOAT4 acceleration;
	XMFLOAT4 force;
	XMFLOAT4 parentPos;
	XMFLOAT4 distance = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 offset = XMFLOAT4(0, 0, 0, 0);
	float isRoot;
	float mass;
	float radius = 0.04f;
	float isIN= 0.0f;
};

struct HeadConstantBuffer
{
	XMFLOAT4 pos = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 SphereObject = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 SphereVelocity = XMFLOAT4(0, 0, 0, 0);
	float Sphereraduis = 0.0f;
	float MaxParticles = 0.0f;
	float hairLength = 0.0f;
	float deltaTime = 0.0f;
};