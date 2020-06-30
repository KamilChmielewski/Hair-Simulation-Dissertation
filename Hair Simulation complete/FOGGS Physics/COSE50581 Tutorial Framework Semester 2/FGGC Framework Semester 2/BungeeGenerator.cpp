#include "BungeeGenerator.h"
#include "Particle.h"

BungeeGenerator::BungeeGenerator(Particle* other, float SpringConstant, float RestLength) :
	_other(other), _springConstant(SpringConstant), _restLength(RestLength)
{

}

BungeeGenerator::~BungeeGenerator()
{

}

void BungeeGenerator::UpdateForce(Particle* particle, float deltaTime)
{
	//Vector3D Anchor = _other->GetPosition();//Vector3D(0.0f, 20.0f, 0.0f);
	
	Vector3D force = particle->GetPosition();

	force = particle->GetPosition();
	force -= _other->GetPosition();

	float magintude = force.Magnitude();
	magintude = _springConstant *(_restLength - magintude) ;

	force.Normalise();
	force *= magintude;
	particle->AddForce(force);
}