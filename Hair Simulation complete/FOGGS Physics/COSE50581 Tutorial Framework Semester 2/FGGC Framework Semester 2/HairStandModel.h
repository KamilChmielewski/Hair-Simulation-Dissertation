#pragma once
#include "ConstantBuffers.h"
#include "Vector3D.h"
#include "Transform.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include <d3dcompiler.h>
#include <d3d11_1.h>


using namespace DirectX;

class HairStrandModel
{
private:
	ID3D11Buffer* ComputeBufferA;
	ID3D11Buffer* ComputeBufferB;
	
	ID3D11Buffer* ComputeDrawBuffer;
	ID3D11Buffer* _constantBuffer;
	ID3D11Device* _pd3dDevice;
	ID3D11DeviceContext* _pImmediateContext;
	ID3D11Buffer* _positionBuffer;
	ID3D11PixelShader* _pixelShader;
	ID3D11VertexShader* _vertexShader;
	std::vector<Vector3D> _positions;
	std::vector<Particle*> _particles;
	std::vector<Transform*> _transforms;
	ParticleSystem* system;
	Vector3D rootNode;
	Vector3D rootNodeFinalPos;
	Vector3D rootOffset;
	Vector3D hairDistance = Vector3D(0.0, 0.2f, 0.0f);

	Transform* _transform;
	Transform* _rootTransform;

	std::vector<GameObject*> objects;


	//physics
	Vector3D _velocity;
	Vector3D _position;
	Vector3D _acceleration;
	Vector3D _forceAccum;
	float _dampingForce;
	float _mass = 0.7f;

	int bufferCounter = 0;
	ID3D11ComputeShader* computeShader;
	ID3D11UnorderedAccessView* computeUnorderedAccessViewA;
	ID3D11UnorderedAccessView* computeUnorderedAccessViewB;

	ID3D11UnorderedAccessView* Current;// = computeUnorderedAccessViewA;
	ID3D11UnorderedAccessView* Prev; //= computeUnorderedAccessViewB;

	ID3D11Buffer* ComputeBufferCurrent; //= ComputeBufferA;
	ID3D11Buffer* ComputeBufferPrev; //= ComputeBufferB;

	std::vector<ParticleStruct> _computeParticles;

	HRESULT CompileComputeShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ ID3D11Device* device, _Outptr_ ID3DBlob** blob);
	void InitComputeShader();

	
	
public:
	GameObject* sphere;
	HairStrandModel(ID3D11Device* _pd3dDevice, ID3D11DeviceContext* _pImmediateContext, Transform* rootNode, Vector3D offset, float hairLength);
	~HairStrandModel();

	void InitBuffer(float size);

	void SetSphereObject(GameObject* _sphere) { sphere = _sphere; };
	void SetComputeShader(ID3D11ComputeShader* computeShader);
	void SetPixelShader(ID3D11PixelShader* pixelShader) { _pixelShader = pixelShader; }
	void SetVertexShader(ID3D11VertexShader* vertexShader) { _vertexShader = vertexShader; }
	void SetParticleSystem(ParticleSystem* sys) { system = sys; }

	void AddGenerator(ParticleForceGenerator* fg)
	{
		for (unsigned int i = 0; i < _particles.size(); i++)
		{
			_particles[i]->AddGenerator(fg);
		}
	}

	void Update(float dt);

	void UpdateCompute(float dt);

	void Render(ConstantBuffer cb, ID3D11Buffer* constantBuffer);
	void RenderCompute(ConstantBuffer cb, ID3D11Buffer* constantBuffer);
};