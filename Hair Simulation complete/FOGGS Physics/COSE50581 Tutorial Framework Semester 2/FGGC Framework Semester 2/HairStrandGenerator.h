#pragma once
#include "ParticleForceGenerator.h"

class HairStrandGenerator : public ParticleForceGenerator
{
private:
	Particle* _parent;
	float _springConstant;
	float _restLength;

	float k = 50;

public:

	HairStrandGenerator(float springConstant, float restLength, Particle* parent = NULL);
	~HairStrandGenerator();

	void UpdateForce(Particle* particle, float deltaTime);
	void SetParent(Particle* parent) { _parent = parent; }
};