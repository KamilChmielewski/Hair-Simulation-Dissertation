#include "SpringConstraintGenator.h"
#include "Particle.h"

SpringConstraintGenator::SpringConstraintGenator(Particle* other, float SpringConstant, float RestLength) : _other(other), _springConstant(SpringConstant), _restLength(RestLength)
{

}

SpringConstraintGenator::~SpringConstraintGenator()
{

}

void SpringConstraintGenator::UpdateForce(Particle* particle, float deltaTime)
{
	Vector3D Anchor = _other->GetPosition();//Vector3D(0.0f, 20.0f, 0.0f);
	Vector3D force;

	//particle->GetPosition();
	//force -= _other->GetPosition();
	//float magintude = force.Magnitude();
	//magintude = abs(magintude - _restLength);
	//magintude *= _springConstant;

	////calculate the final force and apply it
	//force.Normalise();
	//force *= -magintude;
	//particle->AddForce(force);

	//force.Zero();

	force = particle->GetPosition();
	force -= Anchor;

	float magintude = force.Magnitude();
	magintude = (_restLength - magintude) * _springConstant;

	force.Normalise();
	force *= magintude;
	particle->AddForce(force);
}