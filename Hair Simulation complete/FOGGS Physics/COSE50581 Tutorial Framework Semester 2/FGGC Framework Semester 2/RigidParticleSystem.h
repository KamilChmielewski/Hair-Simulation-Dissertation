#pragma once
#include "GameObject.h"
#include "RigidForceGenerator.h"
#include "RigidGravityGenerator.h"
#include "RigidHairStrandGenerator.h"
#include "ConstantBuffers.h"
#include <vector>

class RigidParticleSystem
{
private:
	std::vector<GameObject*> _rigidBodies;
public:
	RigidParticleSystem(std::vector<GameObject*> objects);
	~RigidParticleSystem();
	
	void AddGenerator(RigidForceGenerator* generator);
	void Update(float deltaTime);
	void Draw(ID3D11DeviceContext* pImmediateContext, ID3D11Buffer* _pConstantBuffer, ConstantBuffer cb);
};