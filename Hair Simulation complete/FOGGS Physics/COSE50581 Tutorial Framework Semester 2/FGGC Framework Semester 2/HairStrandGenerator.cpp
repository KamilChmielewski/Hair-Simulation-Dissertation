#include "HairStrandGenerator.h"
#include "Particle.h"

HairStrandGenerator::HairStrandGenerator(float springConstant, float restLength, Particle* parent) : _parent(parent), _springConstant(springConstant), _restLength(restLength)
{

}

HairStrandGenerator::~HairStrandGenerator()
{

}

void HairStrandGenerator::UpdateForce(Particle* particle, float delta)
{
	//Vector3D Anchor;
	//Vector3D force = Vector3D(0.0f, 0.0f, 0.0f);
	//Vector3D offsetWithScale = Vector3D(0.0f, 0.0f, 0.0f);
	//
	//if (_parent == NULL)
	//	return; //Return if Parent is null(HEAD)
	//else
	//	Anchor = _parent->GetPosition();

	//force = particle->GetPosition() + offsetWithScale;
	//force -= Anchor;

	//float magnitude = force.Magnitude();
	//magnitude = (_restLength - magnitude) * _springConstant;

	//force.Normalise();
	//force *= magnitude;

	//if (force.x > 25.0f)
	//	force.x = 25.0f;
	//if (force.x < -25.0f)
	//	force.x = -25.0f;

	//particle->AddForce(force);


	Vector3D force = Vector3D(0.0f, 0.0f, 0.0f);
	Vector3D Anchor;

	if (_parent == NULL)
		return;
	else
		Anchor = _parent->GetPosition();

	force = -k * (particle->GetPosition() - _parent->GetPosition());

	float k = 2 ^ 2;


	particle->AddForce(force);


	/*float angle, angleZ;

	Vector3D prevPos = _parent->GetPosition() - particle->GetPosition();

	angle = atan2f(prevPos.y, prevPos.x);
	angleZ = atan2f(prevPos.z, prevPos.x);

	particle->SetRotation(cos(angle), sin(angle), sin(angleZ));

	Vector3D newPos = _parent->GetPosition();
	newPos.y -= -1;
	particle->GetTransform()->SetPosition(newPos);*/
}