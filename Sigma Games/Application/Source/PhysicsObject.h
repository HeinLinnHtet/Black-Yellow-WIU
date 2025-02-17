#pragma once

#include "Vector3.h"

class PhysicsObject
{
public:
	PhysicsObject();
	virtual ~PhysicsObject() = default;

	//for linear movement
	Vector3 pos;
	Vector3 vel;
	Vector3 accel;
	float mass;
	float bounciness;

	//for angular movement
	float angularVel; //degrees per second
	float angleDeg;   //degrees

	void AddForce(const Vector3& force); //add a pushing force through the center of mass
	void AddImpulse(const Vector3& impulse);
	void UpdatePhysics(float dt);

protected:
	Vector3 m_totalForces;
};
