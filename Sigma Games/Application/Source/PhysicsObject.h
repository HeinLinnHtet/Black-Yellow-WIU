#pragma once

#include <glm\glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/constants.hpp>

class PhysicsObject
{
public:
	PhysicsObject();
	virtual ~PhysicsObject() = default;

	//for linear movement
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec3 accel;
	float mass;
	float bounciness;

	//for angular movement
	float angularVel; //degrees per second
	float angleDeg;   //degrees

	float invMass;

	void AddForce(const glm::vec3& force); //add a pushing force through the center of mass
	void AddImpulse(const glm::vec3& impulse);
	void UpdatePhysics(float dt);

protected:
	glm::vec3 m_totalForces;
};
