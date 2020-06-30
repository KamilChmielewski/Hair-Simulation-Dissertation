#pragma once
#include "RigidForceGenerator.h"

class RigidGravityGenerator : public RigidForceGenerator
{
private:
	Vector3D _gravity = Vector3D(0.0f, -9.8f, 0.0f);

public:
	bool gravityIsOn = true;
	RigidGravityGenerator();
	~RigidGravityGenerator();

	void UpdateForce(RigidBody* particle, float deltaTime);
};