#include "RigidGravityGenerator.h"
#include "RigidBody.h"

RigidGravityGenerator::RigidGravityGenerator()
{
}

RigidGravityGenerator::~RigidGravityGenerator()
{
}

void RigidGravityGenerator::UpdateForce(RigidBody* particle, float deltaTime)
{
	if (particle->GetGravityState())
	{
		particle->AddForce(_gravity * particle->GetMass());
	}

}
