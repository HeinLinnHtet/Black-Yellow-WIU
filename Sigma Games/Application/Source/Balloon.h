#pragma once
#include "Windows.h"
#include <iostream>
#include "string.h"
#include "PhysicsObject.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm/gtc/constants.hpp>

class Balloon : public PhysicsObject
{
public:
	glm::vec3 dimensions{ 1.9, 2.2, 1.9 };
	bool popped = false;

	Balloon();
	Balloon(float x, float y, float z, int pts);
	~Balloon();

	int points = 100;
};

