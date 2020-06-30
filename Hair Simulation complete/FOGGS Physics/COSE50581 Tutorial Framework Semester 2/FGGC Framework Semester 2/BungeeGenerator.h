#pragma once
#include "ParticleForceGenerator.h"

class BungeeGenerator : public ParticleForceGenerator
{
private:
	Particle* _other;
	float _springConstant;
	float _restLength;

public:

	BungeeGenerator(Particle* other, float springConstant, float restLength);
	~BungeeGenerator();

	void UpdateForce(Particle* particle, float deltaTime);
};