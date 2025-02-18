#include "PhysicsObject.h"

PhysicsObject::PhysicsObject()
	: pos{}, vel{}, accel{}, mass(1.f), m_totalForces{},
	  angularVel{}, angleDeg{}
{
}

void PhysicsObject::AddForce(const Vector3& force)
{
	m_totalForces += force;
}

void PhysicsObject::AddImpulse(const Vector3& impulse)
{
	vel += impulse * (1/mass);
}

void PhysicsObject::UpdatePhysics(float dt)
{
	//if mass is 0, treat object as static/immovable
	if (mass == 0)
		return;

	//F = MA
	invMass = 1.f / mass;
	Vector3 finalAccel = accel + m_totalForces * invMass;

	//euler integration
	vel += finalAccel * dt;
	pos += vel * dt;

	//clear forces
	m_totalForces.SetZero();

	//do angular movement
	angleDeg += angularVel * dt;
}
