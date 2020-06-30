#pragma once
#include "GameObject.h"
#include "Vector3D.h"

class CollisionResolution
{
private:
	static CollisionResolution* _instance;

	void RigidCalculateCoefficientResolution(GameObject* object1, GameObject* object2);
	void CalculateCoefficientResolution(GameObject* object1, GameObject* object2);

public:
	static CollisionResolution* Instance() { static CollisionResolution* _instance = new CollisionResolution(); return _instance; }

	void SphereAndPlaneCollisionResolution(GameObject* sphere, GameObject* plane);

	void SphereAndSphereCollisionResolution(GameObject* sphere, GameObject* sphere2);

	void RigidSphereAndSphereCollisionResolution(GameObject* sphere, GameObject* sphere2);

	void RigidSphereAndPlaneCollisionResolution(GameObject* sphere, GameObject* plane);
};