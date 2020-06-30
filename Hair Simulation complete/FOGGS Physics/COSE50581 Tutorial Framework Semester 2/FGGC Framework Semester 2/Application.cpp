#include "Application.h"
#include <iostream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool Application::HandleKeyboard(MSG msg)
{
	XMFLOAT3 cameraPosition = _camera->GetPosition();

	switch (msg.wParam)
	{
	case VK_UP:
		_cameraOrbitRadius = max(_cameraOrbitRadiusMin, _cameraOrbitRadius - (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_DOWN:
		_cameraOrbitRadius = min(_cameraOrbitRadiusMax, _cameraOrbitRadius + (_cameraSpeed * 0.2f));
		return true;
		break;

	case VK_RIGHT:
		_cameraOrbitAngleXZ -= _cameraSpeed;
		return true;
		break;

	case VK_LEFT:
		_cameraOrbitAngleXZ += _cameraSpeed;
		return true;
		break;
	}

	return false;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;

	DSLessEqual = nullptr;
	RSCullNone = nullptr;

	_hairLineModels.clear();
	
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\stone.dds", nullptr, &_pTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\floor.dds", nullptr, &_pGroundTextureRV);

    // Setup Camera
	XMFLOAT3 eye = XMFLOAT3(0.0f, 10.0f, -40.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_camera = new Camera(eye, at, up, (float)_renderWidth, (float)_renderHeight, 0.01f, 400.0f);
		
	// Setup the scene's light
	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	Geometry cubeGeometry;
	cubeGeometry.indexBuffer = _pIndexBuffer;
	cubeGeometry.vertexBuffer = _pVertexBuffer;
	cubeGeometry.numberOfIndices = 36;
	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Geometry planeGeometry;
	planeGeometry.indexBuffer = _pPlaneIndexBuffer;
	planeGeometry.vertexBuffer = _pPlaneVertexBuffer;
	planeGeometry.numberOfIndices = 6;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Material shinyMaterial;
	shinyMaterial.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial.specularPower = 10.0f;

	Material noSpecMaterial;
	noSpecMaterial.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial.specularPower = 0.0f;

	_cubeAppearance = new Appearance(cubeGeometry, shinyMaterial, _pImmediateContext);
	_planeAppearance = new Appearance(planeGeometry, noSpecMaterial, _pImmediateContext);
	
	GameObject* gameObject = new GameObject("Floor", _planeAppearance);
	gameObject->SetPlane(Vector3D(0.0f, 1.0f, 0.0f));
	auto transform = gameObject->GetTransform();
	transform->SetPosition(0.0f, 0.0f, 0.0f);
	transform->SetScale(15.0f, 15.0f, 15.0f);
	transform->SetRotation(XMConvertToRadians(90.0f), 0.0f, 0.0f);

	gameObject->GetApperance()->SetTextureRV(_pGroundTextureRV);

	_gameObjects.push_back(gameObject);

	//Head
	Transform* HeadTransform = new Transform();
	HeadTransform->SetScale(0.5, 1.0f, 0.5);
	HeadTransform->SetPosition(2.0f, 20.0f, 0.0f);

	GameObject* Head = new GameObject(_cubeAppearance, HeadTransform);
	Head->SetType("Head");
	Head->GetApperance()->SetTextureRV(_pTextureRV);
	//Head->GetParticle()->SetDampingForce(0.4f);
	Head->SetSphere(0.5f);
	Head->GetRigidBody()->SetMass(20.0f);
	Head->SetCoefficient(1.0f);

	_hairObjects.push_back(Head);


	/*HairMesh Loading*/
	_hairModel = ModelLoader::Load("Resources\\HairStrand.obj", _pd3dDevice);
	Geometry test;
	test.indexBuffer = _hairModel->_pIndexBuffer;
	test.vertexBuffer = _hairModel->_pVertexBuffer;
	test.numberOfIndices = _hairModel->Indices.size();
	test.vertexBufferOffset = 0;
	test.vertexBufferStride = sizeof(Vertex);

	_hairCylinder = new Appearance(test, shinyMaterial, _pImmediateContext);

	_hairCylinder->SetMesh(_hairModel);


	for (unsigned int i = 0; i < 10; i++)
	{
		Transform* transform = new Transform();
		transform->SetScale(0.0, 4.0, 1.0);

		GameObject* Strand = new GameObject(_hairCylinder, transform);
		Strand->SetType("Hair Strand");
		Strand->GetApperance()->SetTextureRV(_pTextureRV);
		//Strand->GetParticle()->SetDampingForce(0.98);
		Strand->SetSphere(0.5f);
		Strand->GetRigidBody()->SetMass(0.7f);
		Strand->SetCoefficient(1.0f);

		if (i == 0)
		{
			Vector3D HeadPos = _hairObjects[0]->GetTransform()->GetPosition();
			HeadPos.y -= -2.0f;
			transform->SetPosition(HeadPos);
			//Strand->GetTransform()->SetPosition(HeadPos);
			Strand->GetRigidBody()->SetPosition(HeadPos);

			_hairObjects.push_back(Strand);
			RigidHairStrandGenerator* HairGenerator = new RigidHairStrandGenerator(40.0f, 0.0f, _hairObjects[0]->GetRigidBody());
			_hairObjects[1]->GetRigidBody()->AddGenerator(HairGenerator);
			//_hairStrandGenerators.push_back(HairGenerator);
		}
		else
		{
			Vector3D ParentPos = _hairObjects[i]->GetRigidBody()->GetPosition();
			ParentPos.y -= -2.0f;
			Strand->GetRigidBody()->SetPosition(ParentPos);

			_hairObjects.push_back(Strand);
			RigidHairStrandGenerator* HairGenerator = new RigidHairStrandGenerator(40.0f, 0.0f, _hairObjects[i]->GetRigidBody());
			_hairObjects[i + 1]->GetRigidBody()->AddGenerator(HairGenerator);
			//_hairStrandGenerators.push_back(HairGenerator);
		}
	}



	//Head
	Transform* HeadTransform2 = new Transform();
	HeadTransform2->SetScale(0.5, 0.5f, 0.5);
	HeadTransform2->SetPosition(2.0f, 20.0f, 0.0f);

	GameObject* Head2 = new GameObject("Head", _cubeAppearance, HeadTransform2);
	Head2->SetType("Head");
	Head2->GetApperance()->SetTextureRV(_pTextureRV);
	//Head->GetParticle()->SetDampingForce(0.4f);
	Head2->SetSphere(0.5f);
	Head2->GetParticle()->SetMass(1.0f);
	Head2->SetCoefficient(1.0f);

	_hairObjectsNonRigid.push_back(Head2);

	for (unsigned int i = 0; i < 10; i++)
	{
		Transform* transform = new Transform();
		transform->SetScale(0.5, 0.35, 1.0);

		GameObject* Strand = new GameObject("Hair Strand", _hairCylinder, transform);
		Strand->SetType("Hair Strand");
		Strand->GetApperance()->SetTextureRV(_pTextureRV);
		//Strand->GetParticle()->SetDampingForce(0.98);
		Strand->SetSphere(0.5f);
		Strand->GetParticle()->SetMass(0.7f);
		Strand->SetCoefficient(1.0f);

		if (i == 0)
		{
			Vector3D HeadPos = _hairObjectsNonRigid[0]->GetTransform()->GetPosition();
			HeadPos.y -= -2.0f;
			transform->SetPosition(HeadPos);
			//Strand->GetTransform()->SetPosition(HeadPos);
			Strand->GetTransform()->SetPosition(HeadPos);

			_hairObjectsNonRigid.push_back(Strand);
			HairStrandGenerator* HairGenerator = new HairStrandGenerator(40.0f, 0.0f, _hairObjectsNonRigid[0]->GetParticle());
			_hairObjectsNonRigid[1]->GetParticle()->AddGenerator(HairGenerator);
			//_hairStrandGenerators.push_back(HairGenerator);
		}
		else
		{
			Vector3D ParentPos = _hairObjectsNonRigid[i]->GetParticle()->GetPosition();
			ParentPos.y -= -1.0f;
			Strand->GetTransform()->SetPosition(ParentPos);

			_hairObjectsNonRigid.push_back(Strand);
			HairStrandGenerator* HairGenerator = new HairStrandGenerator(40.0f, 0.0f, _hairObjectsNonRigid[i]->GetParticle());
			_hairObjectsNonRigid[i + 1]->GetParticle()->AddGenerator(HairGenerator);
			//_hairStrandGenerators.push_back(HairGenerator);
		}
	}


	for (auto i = 0; i < 5; i++)
	{
		Transform* transform = new Transform();
		transform->SetScale(0.25f, 0.25f, 0.25f);
		transform->SetPosition(-4.0f + (i * 2.0f), 7.0f, 30.0f);
		
		gameObject = new GameObject("Cube " + i, _cubeAppearance, transform);
		
		gameObject->GetApperance()->SetTextureRV(_pTextureRV);
		gameObject->GetParticle()->SetDampingForce(0.98);
		gameObject->SetSphere(0.5f);
		gameObject->GetParticle()->SetMass(20.0f);
		gameObject->SetCoefficient(1.0f);

		//gameObject->GetParticle()->SetMass(2);
		_gameObjects.push_back(gameObject);
	}

	Transform* trans = new Transform();
	trans->SetScale(1.0f, 10.0f, 1.0f);
	//trans->SetPosition(-8.0f, 20.0f, 10.0f);
	trans->SetPosition(-8.0f, 0.5f, 10.0f);
	
	testObj = new GameObject(_cubeAppearance, trans);

	testObj->GetApperance()->SetTextureRV(_pTextureRV);
	testObj->SetSphere(1.0f);
	

	_gravityGenerator = new GravityGenerator();
	_laminarGenerator = new LaminarGenerator();
	_turbulentGenerator = new TurbulentGenerator();
	_particleSystem = new ParticleSystem(_gameObjects);

	_testParticleSystem = new ParticleSystem(_cubeAppearance, Vector3D(0.0f, 0.5, 10.0f), 0.4f, 200, 0.8);
	_testParticleSystem->AddGenerator(_gravityGenerator);

	//Scalp will be _hairObject[1]
	/*_springGeneratorA = new SpringConstraintGenator(_hairObjects[1]->GetParticle(), 400.0f, 1.0f);
	_springGeneratorB = new SpringConstraintGenator(_hairObjects[1]->GetParticle(), 1.0f, 2.0f);
	_BungeeGenerator = new BungeeGenerator(_hairObjects[1]->GetParticle(), 100.0f, 0.0142f);*/

	_hairSystem = new ParticleSystem(_hairObjectsNonRigid);
	_hairSystem->AddGenerator(_gravityGenerator);

	//Rigid bodies Hair system
	_rigidGravityGenerator = new RigidGravityGenerator();
	_rigidBodySystem = new RigidParticleSystem(_hairObjects);
	_rigidBodySystem->AddGenerator(_rigidGravityGenerator);
	
	for (int i = 0; i < _hairStrandGenerators.size(); i++)
	{
	//	_hairSystem->AddGenerator(_hairStrandGenerators[i]);
	}
	//_hairSystem->AddGenerator(_BungeeGenerator);
	//_hairSystem->AddGenerator(_springGeneratorA);
	//_hairSystem->AddGenerator(_springGeneratorB);

	_particleSystem->AddGenerator(_turbulentGenerator);
	_particleSystem->AddGenerator(_gravityGenerator);


	//_hairLineModel = new HairStrandModel(_pd3dDevice, _pImmediateContext, _hairObjectsNonRigid[0]->GetTransform(), Vector3D(0.0f, 0.0f, 0.0f), 20);
	//_hairLineModel->SetPixelShader(_pHairPixelShader);
	//_hairLineModel->SetVertexShader(_pHairVertexShader);
	//_hairLineModel->SetParticleSystem(_hairSystem);
	//_hairLineModel->AddGenerator(_gravityGenerator);

	//for (int i = 0; i < 15; i++)
	//{
	//	HairStrandModel* temp = new HairStrandModel(_pd3dDevice, _pImmediateContext, _hairObjectsNonRigid[0]->GetTransform(), Vector3D(i * 0.1f , 0.0f, 0.0f), 20);
	//	temp->SetPixelShader(_pHairPixelShader);
	//	temp->SetVertexShader(_pHairVertexShader);
	//	temp->AddGenerator(_gravityGenerator);
	//	_hairLineModels.push_back(temp);

	//}

	CreateSphere(1.0f); //Creates Sphere index and vertexBuffers;

	Geometry sphereGeometry;
	sphereGeometry.indexBuffer = _sphereIndexBuffer;
	sphereGeometry.vertexBuffer = _sphereVertexBuffer;
	sphereGeometry.numberOfIndices = sphereIndices.size();
	sphereGeometry.vertexBufferOffset = 0;
	sphereGeometry.vertexBufferStride = sizeof(SimpleVertex);

	Material sphereMaterial;
	shinyMaterial.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial.specularPower = 10.0f;

	_sphereAppearance = new Appearance(sphereGeometry, shinyMaterial, _pImmediateContext);
	Transform* sphereTrans = new Transform();
	sphereTrans->SetScale(1.0f, 1.0f, 1.0f);
	sphereTrans->SetPosition(-12.0f, 0.5f, 0.0f);

	sphereObject = new GameObject("Sphere", _sphereAppearance, sphereTrans);
	
	sphereObject->SetSphere(1.0f);
	sphereObject->GetApperance()->SetTextureRV(_pTextureRV);
	sphereObject->GetParticle()->SetMass(5.0f);

	_hairLineModels.clear();

	HairStrandModel* temp = new HairStrandModel(_pd3dDevice, _pImmediateContext, _hairObjectsNonRigid[0]->GetTransform(), Vector3D(0.2f, 0.0f, 0.2f), 20);
	temp->SetPixelShader(_pHairPixelShader);
	temp->SetVertexShader(_pHairVertexShader);
	temp->AddGenerator(_gravityGenerator);
	temp->SetComputeShader(computeShader);
	temp->SetSphereObject(sphereObject);
	_hairLineModels.push_back(temp);


	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	ID3DBlob* pVSHairBlob;

	 hr = CompileShaderFromFile(L"HairShader.fx", "VS", "vs_4_0", &pVSHairBlob);

	 hr = _pd3dDevice->CreateVertexShader(pVSHairBlob->GetBufferPointer(), pVSHairBlob->GetBufferSize(), nullptr, &_pHairVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;


	ID3DBlob* pPSHairBlob = nullptr;

	hr = CompileShaderFromFile(L"HairShader.fx", "PS", "ps_4_0", &pPSHairBlob);


	if (FAILED(hr))
	{
		std::cout << "failed to load HairShader.fx" << std::endl;
		return hr;
	}
	hr = _pd3dDevice->CreatePixelShader(pPSHairBlob->GetBufferPointer(), pPSHairBlob->GetBufferSize(), nullptr, &_pHairPixelShader);
	
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);

	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);


	ID3DBlob* csBlob = nullptr;
	hr = CompileComputeShader(L"ComputerHair.hlsl", "CSMain", _pd3dDevice, &csBlob);
	if (FAILED(hr))
	{
		_pd3dDevice->Release();
		printf("Failed compiling shader %08X\n", hr);
		return hr;
	}

	// Create shader

	hr = _pd3dDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &computeShader);


	csBlob->Release();


	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Application::CreateSphere(float radius)
{
	sphereVector.clear();
	sphereIndices.clear();
	HRESULT hr;
	_radius = radius;
	int stackCount = 80;
	int sliceCount = 80;
	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;

	SimpleVertex root;

	root.PosL.x = 0;
	root.PosL.y = radius;
	root.PosL.z = 0;

	root.NormL.x = 0;
	root.NormL.y = 1;
	root.NormL.z = 0;

	root.Tex.x = 0;
	root.Tex.y = 0;

	sphereVector.push_back(root);

	for (unsigned int i = 0; i <= stackCount-1; i++)
	{
		float phi = i * phiStep;
		for (unsigned int j = 0; j <= sliceCount; j++) 
		{
			SimpleVertex temp;
			float theta = j * thetaStep;
			temp.PosL.x = _radius * std::sin(phi) * std::cos(theta);
			temp.PosL.y = _radius * std::cos(phi);
			temp.PosL.z = _radius * std::sin(phi) * std::sin(theta);

			temp.NormL.x = 0;
			temp.NormL.y = 1;
			temp.NormL.z = 0;

			temp.Tex.x = theta / (XM_PI * 2);
			temp.Tex.y = phi / XM_PI;

			sphereVector.push_back(temp);
		}
	}
	SimpleVertex finalVertex;
	finalVertex.PosL.x = 0;
	finalVertex.PosL.y = -_radius;
	finalVertex.PosL.z = 0;

	finalVertex.NormL.x = 0;
	finalVertex.NormL.y = -1; //will change to positive probs
	finalVertex.NormL.z = 0;

	finalVertex.Tex.x = 0;
	finalVertex.Tex.y = 1;

	sphereVector.push_back(finalVertex);

	for (unsigned int i = 0; i <= sliceCount; i++)
	{
		sphereIndices.push_back(0);
		sphereIndices.push_back(i + 1);
		sphereIndices.push_back(i);
	}

	int baseIndex = 1;
	int ringVertexCount = sliceCount + 1;

	for (unsigned int i = 0; i < stackCount-2; i++)
	{
		for (unsigned int j = 0; j < sliceCount; j++)
		{
			sphereIndices.push_back(baseIndex + i * ringVertexCount + j);
			sphereIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			sphereIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			sphereIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			sphereIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			sphereIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	int southPoleIndex = sphereVector.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;
	for (unsigned int i = 0; i < sliceCount; i++)
	{
		sphereIndices.push_back(southPoleIndex);
		sphereIndices.push_back(baseIndex + i);
		sphereIndices.push_back(baseIndex + i + 1);
	}

	D3D11_BUFFER_DESC bdVertex;
	ZeroMemory(&bdVertex, sizeof(bdVertex));
	bdVertex.Usage = D3D11_USAGE_DEFAULT;
	bdVertex.ByteWidth = sizeof(SimpleVertex) * sphereVector.size();
	bdVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdVertex.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitVertexData;
	ZeroMemory(&InitVertexData, sizeof(InitVertexData));
	InitVertexData.pSysMem = &sphereVector[0];
	

	hr = _pd3dDevice->CreateBuffer(&bdVertex, &InitVertexData, &_sphereVertexBuffer);

	if (FAILED(hr))
	{
		std::cout << "Sphere VertexBuffer failed" << std::endl;
	}


	D3D11_BUFFER_DESC bd1;
	ZeroMemory(&bd1, sizeof(bd1));

	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(WORD) * sphereIndices.size();
	bd1.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd1.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitSphereData;
	ZeroMemory(&InitSphereData, sizeof(InitSphereData));
	InitSphereData.pSysMem = &sphereIndices[0];
	hr = _pd3dDevice->CreateBuffer(&bd1, &InitSphereData, &_sphereIndexBuffer);

	if (FAILED(hr))
		std::cout << "Sphere IndexBuffer failed" << std::endl;

}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileComputeShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ ID3D11Device* device, _Outptr_ ID3DBlob** blob)
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
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,entryPoint, profile,flags, 0, &shaderBlob, &errorBlob);
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

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
      //  D3D_FEATURE_LEVEL_10_1,
      //  D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 1;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
	UINT maxQuality;
	/*hr = _pd3dDevice->CheckMultisampleQualityLevels(sd.BufferDesc.Format, sd.SampleDesc.Count, &maxQuality);
	if (hr == S_OK)
	{
		std::cout << "ok" << std::endl;
	}*/
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _renderWidth;
    sd.BufferDesc.Height = _renderHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 144;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;



    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_renderWidth;
    vp.Height = (FLOAT)_renderHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _renderWidth;
	depthStencilDesc.Height = _renderHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	if (_pTextureRV) _pTextureRV->Release();

	if (_pGroundTextureRV) _pGroundTextureRV->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();

    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();

    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (DSLessEqual) DSLessEqual->Release();
	if (RSCullNone) RSCullNone->Release();

	if (CCWcullMode) CCWcullMode->Release();
	if (CWcullMode) CWcullMode->Release();

	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	for (auto gameObject : _gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}

	for (auto hairObject : _hairLineModels)
	{
		delete hairObject;
		hairObject = nullptr;
	}
}

void Application::moveForward(int objectNumber)
{
	auto transform = _gameObjects[objectNumber]->GetTransform();

	Vector3D position = transform->GetPosition();
	position.z -= 0.1f;
	transform->SetPosition(position);
}

void Application::Update(float const deltaTime)
{

    // Update our time
    static float timeSinceStart = 0.0f;
    static DWORD dwTimeStart = 0;

    DWORD dwTimeCur = GetTickCount();

    if (dwTimeStart == 0)
        dwTimeStart = dwTimeCur;

	timeSinceStart = (dwTimeCur - dwTimeStart) / 1000.0f;

	static float elapsedTime = 0.0166667;


	// Move gameobject
	if (GetAsyncKeyState('1'))
	{
		moveForward(1);
	}

	// Update camera
	float angleAroundZ = XMConvertToRadians(_cameraOrbitAngleXZ);

	float x = _cameraOrbitRadius * cos(angleAroundZ);
	float z = _cameraOrbitRadius * sin(angleAroundZ);

	XMFLOAT3 cameraPos = _camera->GetPosition();
	cameraPos.x = x;
	cameraPos.z = z;

	_camera->SetPosition(cameraPos);
	_camera->Update(deltaTime);

	if (GetAsyncKeyState(VK_UP))
	{
		_gameObjects[_currentObject]->GetParticle()->AddForce(Vector3D(0.0f, 0.0f, -400.0f));
		_hairObjectsNonRigid[0]->GetParticle()->AddForce(Vector3D(0.0f, 0.0f, -60.0f));

	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		_gameObjects[_currentObject]->GetParticle()->AddForce(Vector3D(0.0f, 0.0f, 400.0f));
		_hairObjectsNonRigid[0]->GetParticle()->AddForce(Vector3D(0.0f, 0.0f, 60.0f));

	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		_gameObjects[_currentObject]->GetParticle()->AddForce(Vector3D(400.0f, 0.0f, 0.0f));
		_hairObjectsNonRigid[0]->GetParticle()->AddForce(Vector3D(200.0f, 0.0f, 0.0f));
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		_gameObjects[_currentObject]->GetParticle()->AddForce(Vector3D(-400.0f, 0.0f, 0.0f));

		_hairObjectsNonRigid[0]->GetParticle()->AddForce(Vector3D(-200.0f, 0.0f, 0.0f));
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		_gameObjects[_currentObject]->GetParticle()->AddForce(Vector3D(0.0f, 400.0f, 0.0f));

		_hairObjectsNonRigid[0]->GetParticle()->AddForce(Vector3D(0.0f, 100.0f, 0.0f));
	}
	if (GetAsyncKeyState(VK_CONTROL))
	{
		_hairObjectsNonRigid[0]->GetParticle()->AddForce(Vector3D(0.0f, -200.0f, 0.0f));
	}
	if (GetAsyncKeyState('1'))
	{
		_currentObject = 1;
	}
	if (GetAsyncKeyState('2'))
	{
		_currentObject = 2;
	}
	if (GetAsyncKeyState('3'))
	{
		_currentObject = 3;
	}
	if (GetAsyncKeyState('4'))
	{
		_currentObject = 4;
	}


	if (GetAsyncKeyState('I'))
	{
		sphereObject->GetParticle()->AddForce(Vector3D(0.0f, 0.0f, -200.0f));

	}
	if (GetAsyncKeyState('J'))
	{
		sphereObject->GetParticle()->AddForce(Vector3D(-200.0f, 0.0f, 0.0f));

	}
	if (GetAsyncKeyState('K'))
	{
		sphereObject->GetParticle()->AddForce(Vector3D(0.0f, 0.0f, 200.0f));

	}
	if (GetAsyncKeyState('L'))
	{
		sphereObject->GetParticle()->AddForce(Vector3D(200.0f, 0.0f, 0.0f));

	}
	if (GetAsyncKeyState('O'))
	{
		sphereObject->GetParticle()->AddForce(Vector3D(00.0f, 40.0f, 0.0f));
	}
	if (GetAsyncKeyState('U'))
	{
		sphereObject->GetParticle()->AddForce(Vector3D(00.0f, -40.0f, 0.0f));
	}

	if (GetAsyncKeyState('M'))
	{
		testObj->GetRigidBody()->SetPosition(-8.0f, 20.0f, 10.0f);
	}

	_particleSystem->Update(elapsedTime);
	_hairSystem->Update(deltaTime);
	_rigidBodySystem->Update(elapsedTime);
	_testParticleSystem->UpdateTest(elapsedTime);

	sphereObject->Update(deltaTime);
	

	//hair gravity
	for (GameObject* gameObject1 : _hairObjects)
	{
		for (GameObject* gameObject2 : _hairObjects)
		{
			if (gameObject1 != gameObject2)
			{
				if (CollisionDetection::Instance()->CheckRigidSphereAndPlaneCollisionDetection(gameObject1, _gameObjects[0]))
				{
					CollisionResolution::Instance()->RigidSphereAndPlaneCollisionResolution(gameObject1, _gameObjects[0]);
					gameObject1->GetRigidBody()->SetGravity(false);
				}
				else
				{
					gameObject1->GetRigidBody()->SetGravity(true);
				}
			}
		}
	}


	for (int i = 0; i < _hairLineModels.size(); i++)
	{
		_hairLineModels[i]->Update(deltaTime);
	}

}

void Application::Draw()
{
    //
    // Clear buffers
    //

	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //
    // Setup buffers and render scene
    //


	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = _camera->GetView();
	XMFLOAT4X4 projectionAsFloats = _camera->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);
	
	cb.light = basicLight;
	cb.EyePosW = _camera->GetPosition();

	Material material = testObj->GetApperance()->GetMaterial();

	// Copy material to shader
	cb.surface.AmbientMtrl = material.ambient;
	cb.surface.DiffuseMtrl = material.diffuse;
	cb.surface.SpecularMtrl = material.specular;


	cb.World = XMMatrixTranspose(sphereObject->GetTransform()->GetWorldMatrix());//XMMatrixTranspose(testObj->GetTransform()->worldMatrix());
	if (sphereObject->GetApperance()->HasTexture())
	{
		ID3D11ShaderResourceView* textureRV = sphereObject->GetApperance()->GetTextureRV();
		_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
		cb.HasTexture = 1.0f;
	}

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	sphereObject->Draw(_pImmediateContext);



	Material material1 = _hairObjectsNonRigid[0]->GetApperance()->GetMaterial();

	// Copy material to shader
	cb.surface.AmbientMtrl = material1.ambient;
	cb.surface.DiffuseMtrl = material1.diffuse;
	cb.surface.SpecularMtrl = material1.specular;

	// Set world matrix
	cb.World = XMMatrixTranspose(_hairObjectsNonRigid[0]->GetTransform()->GetWorldMatrix());

	// Set texture
	if (_hairObjectsNonRigid[0]->GetApperance()->HasTexture())
	{
		ID3D11ShaderResourceView* textureRV = _hairObjectsNonRigid[0]->GetApperance()->GetTextureRV();
		_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
		cb.HasTexture = 1.0f;
	}
	else
	{
		cb.HasTexture = 0.0f;
	}

	// Update constant buffer
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_hairObjectsNonRigid[0]->DrawLines();
	

	for (int i = 0; i < _hairLineModels.size(); i++)
	{
		_hairLineModels[i]->Render(cb, _pConstantBuffer);
	}
    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}