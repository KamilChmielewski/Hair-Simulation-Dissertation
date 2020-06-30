#include "RigidHairStrandGenerator.h"
#include "RigidBody.h"
#include <DirectXMath.h>

RigidHairStrandGenerator::RigidHairStrandGenerator(float springConstant, float restLength, RigidBody* parent) : _parent(parent), _springConstant(springConstant), _restLength(restLength)
{

}

RigidHairStrandGenerator::~RigidHairStrandGenerator()
{

}

void RigidHairStrandGenerator::UpdateForce(RigidBody* particle, float delta)
{
	//Vector3D Anchor;
	//Vector3D force = Vector3D(0.0f, 0.0f, 0.0f);
	//Vector3D offsetWithScale = Vector3D(0.0f, 0.0f, 0.0f);

	//if (_parent == NULL)
	//	return; //Return if Parent is null(HEAD)
	//else
	//	Anchor = _parent->GetPosition();

	//force = particle->GetPosition();
	//force -= Anchor;

	//float magnitude = force.Magnitude();
	//magnitude = (_restLength - magnitude) * _springConstant;

	////magnitude /= 2.0f;

	//float damp = 0.7;
	//force.Normalise();
	//force *= magnitude * damp;

	//if (force.x > 25.0f)
	//	force.x = 25.0f;
	//if (force.x < -25.0f)
	//	force.x = -25.0f;

	//Vector3D force = Vector3D(0.0f, 0.0f, 0.0f);
	//Vector3D Anchor;

	//if (_parent == NULL)
	//	return;
	//else
	//	Anchor = _parent->GetPosition();

	////Rest length and spring constant
	//float RestLength = 1;
	//float k = 2.0f;

	//force = particle->GetPosition() - Anchor;
 //	float distance = force.Magnitude();

	////Stretch difference between current distance and restlength
	//float strectch = distance - RestLength;

	////Calculate force based on hooke's law
	//force.Normalise();
	//force *= ( k * strectch);

	//particle->AddForce(force);


//	Vector3D force = Vector3D(0.0f, 0.0f, 0.0f);
//	Vector3D Anchor;
//
//	if (_parent == NULL)
//		return;
//	else
//		Anchor =  _parent->GetPosition();
//
//	force = -k * (particle->GetPosition() - _parent->GetPosition());
//
//	//Vector3D dampingForce = damping * particle->GetVelocity();
//
//	//force -= dampingForce;
//
//	particle->AddForce(force);
////	particle->AddForceAtPoint(force, Vector3D(0.0f,  0.0f, -0.1f));
////	particle->AddForceAtPoint(force, Vector3D(0.1, 0.0f, 0.1f));





	Vector3D force = Vector3D(0.0f, 0.0f, 0.0f);
	Vector3D Anchor;

	if (_parent == NULL)
		return;
	else
		Anchor = _parent->GetPosition();

	force = -k * (particle->GetPosition() - _parent->GetPosition());

	//Vector3D dampingForce = damping * particle->GetVelocity();

	//force -= dampingForce;

	particle->AddForce(force);

	float angle, angleZ;

	Vector3D prevPos = _parent->GetPosition() - particle->GetPosition();
	prevPos.Normalise();

	angle = atan2f(prevPos.y, prevPos.x);
	angleZ = atan2f(prevPos.z, prevPos.x);

	//angle = angle * (DirectX::XM_PI / 180);

//	angleZ = angleZ * (DirectX::XM_PI / 180);
	
	particle->AddTorque(Vector3D(cos(angle), sin(angle), sin(angleZ)));
}