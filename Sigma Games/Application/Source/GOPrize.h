#pragma once
#include "Windows.h"
#include <iostream>
#include "string.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm/gtc/constants.hpp>

class GOPrize
{
public:
	std::string name;
	std::string description;
	int cost;

	glm::vec3 position;
	glm::vec3 dimensions;

	GOPrize();
	GOPrize(int newcost, std::string prizename, std::string desc, float w, float d, float h, float x, float z, float y);
	~GOPrize();

	bool bought;
};

