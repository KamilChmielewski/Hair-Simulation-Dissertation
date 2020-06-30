#include "RigidParticleSystem.h"

RigidParticleSystem::RigidParticleSystem(std::vector<GameObject*> objects) : _rigidBodies(objects)
{}

RigidParticleSystem::~RigidParticleSystem()
{
	for (GameObject* ob : _rigidBodies)	
		delete ob;
}

void RigidParticleSystem::AddGenerator(RigidForceGenerator* generator)
{
	for (GameObject* g : _rigidBodies)
	{
		if (g->GetType() != "Head")
		g->GetRigidBody()->AddGenerator(generator);
	}
}

void RigidParticleSystem::Update(float deltaTime)
{
	for (GameObject* g : _rigidBodies)
	{
		g->UpdateRigidBody(deltaTime);
	}
}

void RigidParticleSystem::Draw(ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* _pConstantBuffer, ConstantBuffer cb)
{
	for (auto gameObject : _rigidBodies)
	{
		// Get render material
		Material material = gameObject->GetApperance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetTransform()->GetRigidWorld());

		// Set texture
		if (gameObject->GetApperance()->HasTexture())
		{
			ID3D11ShaderResourceView* textureRV = gameObject->GetApperance()->GetTextureRV();
			pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->Draw(pImmediateContext);
		//gameObject->DrawLines();
		//gameObject->DrawObj();
	}
}

