#include "GameObject.h"

GameObject::GameObject(string type, Appearance* appearance): _apperance(appearance), _type(type)
{
	_sphere._type = "Sphere";
	_plane._type = "Plane";
	

	_transform = new Transform();
	_particle = new Particle(_transform);
}

GameObject::GameObject(string type, Appearance* appearance, Transform* transform)
{
	_sphere._type = "Sphere";
	_plane._type = "Plane";

	_apperance = appearance;
	_transform = transform;
	_type = type;
	_particle = new Particle(_transform);
}

GameObject::GameObject(Appearance* appearance, Transform* transform) : _apperance(appearance), _transform(transform)
{
	
	_sphere._type = "Sphere";
	_plane._type = "Plane";
	
	_rigidBody = new RigidBody(_transform);
	_rigidBody->SetAngularDamping(0.2);
	_rigidBody->SetLinearDamping(0.98);
	_rigidBody->SetMass(20.0f);

}

GameObject::~GameObject()
{
}

void GameObject::SetCollisionPrimitive(CollisionPrimitive collision)
{
//	_collision = collision;
}

void GameObject::Update(float t)
{
	_particle->Update(t);
	_transform->UpdateWorldMatrix();
}

void GameObject::UpdateRigidBody(float t)
{
	//std::cout << "X: " << _rigidBody->GetPosition().x << " Y: " << _rigidBody->GetPosition().y << std::endl;
	_rigidBody->Update(t);	
	//_transform->UpdateWorldMatrix();
}

void GameObject::Draw(ID3D11DeviceContext* pImmediateContext)
{
	_apperance->Draw();
}

void GameObject::DrawLines()
{
	_apperance->DrawLines();
}

void GameObject::DrawObj()
{
	_apperance->DrawObj();
}
