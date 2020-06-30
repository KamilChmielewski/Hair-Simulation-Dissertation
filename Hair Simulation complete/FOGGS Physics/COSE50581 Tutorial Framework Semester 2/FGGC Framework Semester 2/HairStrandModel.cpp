#include "HairStandModel.h"

float LengthOfHair = 0.0f;
int counter = 0;
int rootCounter = 0;

HairStrandModel::HairStrandModel(ID3D11Device* Device, ID3D11DeviceContext* ImmediateContext, Transform* rootPos, Vector3D offset, float hairLength) :
	_pd3dDevice(Device), _pImmediateContext(ImmediateContext), _rootTransform(rootPos), rootOffset(offset)
{
	sphere = nullptr;
	computeShader = nullptr;
	LengthOfHair = hairLength;


	for (unsigned int i = 0; i < hairLength; i++)
	{
		if (i == 0)
		{
			rootNodeFinalPos = (_rootTransform->GetPosition() + offset);
			_positions.push_back(rootNodeFinalPos);
			_particles.push_back(new Particle());
		}
		else
		{
			Vector3D parentPos = _positions[i - 1];
			parentPos.y -=  0.1f;
		
			Particle* particle = new Particle();
			particle->SetPosition(parentPos);
			_particles.push_back(particle);


			HairStrandGenerator* generator = new HairStrandGenerator(40.0f, 0.0f, _particles[i -1]);

			_particles[i]->AddGenerator(generator);


			_positions.push_back(parentPos);
		}
	}


	ParticleStruct temp;
	_computeParticles.clear();

	_computeParticles.clear();
	for (int x = 0; x < 200; x++)
	{
		for (int z = 0; z < 200; z++) //316 is normal amount for real hair
		{
			for (int hair = 0; hair < hairLength; hair++)
			{
				if (hair == 0)
				{
					temp.pos.x = 0.0f;
					temp.pos.y = 0.0f;
					temp.pos.z = 0.0f;
					temp.pos.w = 0.0f;
					temp.parentPos = XMFLOAT4(0, 0, 0, 0);
					temp.acceleration = XMFLOAT4(0, 0, 0, 0);
					temp.velocity = XMFLOAT4(0, 0, 0, 0);
					temp.force = XMFLOAT4(0, 0, 0, 0);
					temp.isRoot = 1;
					temp.mass = 1.0f;
					temp.offset.x = x * 0.01;
					temp.offset.y = 0;
					temp.offset.z = z * 0.01;
					_computeParticles.push_back(temp);
					counter++;
					rootCounter;
				}
				else
				{
					temp.pos.x = 0;
					temp.pos.y = 0;
					temp.pos.z = 0;
					temp.pos.w = 0.0f;
					temp.acceleration = XMFLOAT4(0, 0, 0, 0);
					temp.velocity = XMFLOAT4(0, 0, 0, 0);
					temp.force = XMFLOAT4(0, 0, 0, 0);
					temp.parentPos.x = 0.0f;
					temp.parentPos.y = 0.0f;
					temp.parentPos.z = 0.0f;
					temp.isRoot = 0;
					temp.mass = 1.0f;
					temp.radius = 0.01;
					_computeParticles.push_back(temp);
					counter++;
				}
			}
		}
	}

	InitComputeShader();
}

HairStrandModel::~HairStrandModel()
{
	_positions.clear();
}

void HairStrandModel::InitBuffer(float size)
{
	CD3D11_BUFFER_DESC vertexBufferDesc;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //Needs to be dynamic as positions will be updated.
	vertexBufferDesc.ByteWidth = sizeof(Vector3D) * size;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //allows CPU to write into this buffer
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &_positions;

	HRESULT hr = _pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &_positionBuffer); 
}

void HairStrandModel::Update(float dt)
{


	UpdateCompute(dt);

}

void HairStrandModel::Render(ConstantBuffer cb, ID3D11Buffer* constantBuffer)
{

	RenderCompute(cb, constantBuffer);
}

HRESULT HairStrandModel::CompileComputeShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ ID3D11Device* device, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !device || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR profile = (device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, nullptr, nullptr,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

void HairStrandModel::InitComputeShader()
{
	HRESULT hr;

	D3D11_SUBRESOURCE_DATA instanceData;

	D3D11_BUFFER_DESC sbDesc;
	sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	sbDesc.CPUAccessFlags = 0;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.StructureByteStride = sizeof(ParticleStruct);
	sbDesc.ByteWidth = sizeof(ParticleStruct) * counter;
	sbDesc.Usage = D3D11_USAGE_DEFAULT;

	ZeroMemory(&instanceData, sizeof(instanceData));
	instanceData.pSysMem = &_computeParticles[0];
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;


	hr = _pd3dDevice->CreateBuffer(&sbDesc, &instanceData, &ComputeBufferA);
	_pd3dDevice->CreateBuffer(&sbDesc, &instanceData, &ComputeBufferB);

	ComputeBufferCurrent = ComputeBufferA;
	ComputeBufferPrev = ComputeBufferB;


	D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
	sbUAVDesc.Buffer.FirstElement = 0;
	sbUAVDesc.Buffer.Flags = 0;
	sbUAVDesc.Buffer.NumElements = counter;
	sbUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	hr = _pd3dDevice->CreateUnorderedAccessView(ComputeBufferA, &sbUAVDesc, &computeUnorderedAccessViewA);

	hr = _pd3dDevice->CreateUnorderedAccessView(ComputeBufferB, &sbUAVDesc, &computeUnorderedAccessViewB);

	Current = computeUnorderedAccessViewA;
	Prev = computeUnorderedAccessViewB;


	UINT initCounts = 0;
	_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &computeUnorderedAccessViewA, &initCounts);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(HeadConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_constantBuffer);

	if (FAILED(hr))
	{
		std::cout << "HR Failed" << std::endl;
	}

	D3D11_BUFFER_DESC bd1;
	ZeroMemory(&bd1, sizeof(bd1));
	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(ParticleStruct) * counter;
	bd1.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd1.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &_computeParticles[0];

	hr = _pd3dDevice->CreateBuffer(&bd1, nullptr, &ComputeDrawBuffer);
}

void HairStrandModel::UpdateCompute(float dt)
{
	std::swap(ComputeBufferCurrent, ComputeBufferPrev);
	std::swap(Current, Prev);
	
	HeadConstantBuffer cb;
	cb.pos.x = _rootTransform->GetPosition().x; //+ rootOffset.x;
	cb.pos.y = _rootTransform->GetPosition().y; //+ rootOffset.y;
	cb.pos.z = _rootTransform->GetPosition().z; //+ rootOffset.z;
	cb.MaxParticles = counter;
	cb.SphereObject.x = sphere->GetTransform()->GetPosition().x;
	cb.SphereObject.y = sphere->GetTransform()->GetPosition().y;
	cb.SphereObject.z = sphere->GetTransform()->GetPosition().z;
	cb.SphereVelocity.x = sphere->GetParticle()->GetVelocity().x;
	cb.SphereVelocity.y = sphere->GetParticle()->GetVelocity().y;
	cb.SphereVelocity.z = sphere->GetParticle()->GetVelocity().z;
	cb.Sphereraduis = sphere->GetSphere()._radius;
	cb.deltaTime = dt;
	cb.hairLength = LengthOfHair;

	_pImmediateContext->CSSetConstantBuffers(0, 1, &_constantBuffer);
	_pImmediateContext->UpdateSubresource(_constantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->CSSetShader(computeShader, nullptr, 0); //enable computeShader
	_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &Current, 0);
	_pImmediateContext->CSSetUnorderedAccessViews(1, 1, &Prev, 0);
	_pImmediateContext->Dispatch(counter /64, 1, 1);
	_pImmediateContext->CSSetShader(nullptr, nullptr, 0); //disable the ComputeShader

	_pImmediateContext->CopyResource(ComputeDrawBuffer, ComputeBufferCurrent);
	
}

void HairStrandModel::RenderCompute(ConstantBuffer cb, ID3D11Buffer* constantBuffer)
{
	UINT stride = sizeof(ParticleStruct);
	UINT offset = 0;


	_pImmediateContext->VSSetShader(_vertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pixelShader, nullptr, 0);


	_pImmediateContext->IASetVertexBuffers(0, 1, &ComputeDrawBuffer, &stride, &offset);
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	cb.World = XMMatrixTranspose(XMMatrixIdentity());
	_pImmediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

	for (unsigned i = 0; i < counter; i += LengthOfHair)
	{
		_pImmediateContext->Draw(LengthOfHair, i);
	}

}

void HairStrandModel::SetComputeShader(ID3D11ComputeShader* computeShader)
{ 
	this->computeShader = computeShader;
}