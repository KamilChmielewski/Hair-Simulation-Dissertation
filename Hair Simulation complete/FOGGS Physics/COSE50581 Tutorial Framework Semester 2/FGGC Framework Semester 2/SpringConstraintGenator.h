#pragma once
#include "ParticleForceGenerator.h"

class SpringConstraintGenator : public ParticleForceGenerator
{
private:
	Particle* _other;
	float _springConstant;
	float _restLength;

public:

	SpringConstraintGenator(Particle* other, float springConstant, float restLength);
	~SpringConstraintGenator();

	void UpdateForce(Particle* particle, float deltaTime);
};