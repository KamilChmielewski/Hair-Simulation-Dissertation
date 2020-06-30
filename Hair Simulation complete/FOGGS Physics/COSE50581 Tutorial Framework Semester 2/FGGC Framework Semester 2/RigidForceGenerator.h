#pragma once
class RigidBody;
#include "Vector3D.h"

class RigidForceGenerator
{
private:

public:

	virtual void UpdateForce(RigidBody* particle, float deltaTime) = 0;
};