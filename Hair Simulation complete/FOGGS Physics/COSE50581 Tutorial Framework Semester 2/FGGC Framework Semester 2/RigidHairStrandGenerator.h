#pragma once
#include "RigidForceGenerator.h"

class RigidHairStrandGenerator : public RigidForceGenerator
{
private:
	RigidBody* _parent;
	float _springConstant;
	float _restLength;

	float k = 8; //stiffness of spring
	//float damping = 5; // without damping spring will always bounce

public:

	RigidHairStrandGenerator(float springConstant, float restLength, RigidBody* parent = NULL);
	~RigidHairStrandGenerator();

	void UpdateForce(RigidBody* particle, float deltaTime);
	void SetParent(RigidBody* parent) { _parent = parent; }
};