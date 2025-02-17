#include "FPCamera.h"
#include "KeyboardController.h"

//Include GLFW
#include <GLFW/glfw3.h>

FPCamera::FPCamera() : isDirty(false)
{
	this->position = glm::vec3(0, 0, 0);
	this->target = glm::vec3(0, 0, 0);
	this->up = glm::vec3(0, 1, 0);
}

FPCamera::~FPCamera()
{
}

void FPCamera::Init(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	this->position = position;
	this->target = target;
	this->up = up;
	this->isDirty = true;

	Refresh();
}

void FPCamera::Reset()
{
}

void FPCamera::Update(double dt)
{
	static const float ROTATE_SPEED = 60.0f;
	static const float ZOOM_SPEED = 10.0f;
	const float TREE_WIDTH = 1.5f;  // Half-width for bounding box
	const float TREE_DEPTH = 1.5f;  // Half-depth for bounding box

	view = glm::normalize(target - position);
	right = glm::normalize(glm::cross(view, up)); // Update the right vector
	glm::vec3 newPosition = position;

	// Graves positions (x, z coordinates)
	float tree[][2] = {
		{-60,-60}, {-58,-45}, {-55,-30}, {-57,-12}, {-60,5}, {-58,22}, {-55,35}, {-60,48}, {-58,60},
		{-45,-58}, {-42,-42}, {-40,-25}, {-41,-14}, {-45,8}, {-43,20}, {-40,38}, {-42,52}, {-45,60},
		{-30,-55}, {-28,-41}, {-25,-29}, {-27,-17}, {-30,12}, {-28,26}, {-25,40}, {-27,54}, {-30,58},
		{-12,-57}, {-14,-44}, {-10,-35}, {-11,-22}, {-12,14}, {-14,28}, {-10,46}, {-11,55}, {-12,60},
		{5,-60}, {8,-45}, {6,-30}, {17,-12}, {5,22}, {8,36}, {6,50}, {7,60},
		{22,-58}, {20,-42}, {24,-28}, {21,-15}, {22,8}, {20,25}, {24,40}, {21,52}, {22,60},
		{35,-55}, {32,-38}, {30,-26}, {33,-12}, {35,5}, {32,18}, {30,33}, {33,48}, {35,60},
		{48,-60}, {45,-44}, {42,-30}, {41,-18}, {48,8}, {45,22}, {42,36}, {41,50}, {48,60},
		{60,-58}, {58,-42}, {55,-28}, {57,-12}, {60,5}, {58,24}, {55,38}, {57,52}, {60,60},
		{ 57,-13 }, {14,-48}, {-37,-26}, {-33,46}, {36,24}, {25,-12}, {59,-37}, {-18,30}, {50,-35}, {38,34},
		{-11,30}, {27,-42}, {40,15}, {-39,-23}, {-28,47}, {22,-30}, {60,21}, {42,-49}, {31,-33}, {24,35}
	};

	// Utility function to check collision
	auto CheckCollision = [&](glm::vec3 pos) {
		for (int i = 0; i < sizeof(tree) / sizeof(tree[0]); i++) {
			float gx = tree[i][0];
			float gz = tree[i][1];

			// Simple AABB collision check on the x-z plane
			if (pos.x > gx - TREE_WIDTH && pos.x < gx + TREE_WIDTH &&
				pos.z > gz - TREE_DEPTH && pos.z < gz + TREE_DEPTH) {
				return true; // Collision detected
			}
		}
		return false; // No collision
		};
	if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W)) {
		glm::vec3 tempPosition = newPosition + view * ZOOM_SPEED * static_cast<float>(dt);
		if (CheckCollision(tempPosition) == false) {
			newPosition = tempPosition;
		}
	}
	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S)) {
		glm::vec3 tempPosition = newPosition - view * ZOOM_SPEED * static_cast<float>(dt);
		if (CheckCollision(tempPosition) == false) {
			newPosition = tempPosition;
		}
	}

	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D)) {
		glm::vec3 tempPosition = newPosition + right * ZOOM_SPEED * static_cast<float>(dt);
		if (CheckCollision(tempPosition) == false) {
			newPosition = tempPosition;
		}
	}

	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A)) {
		glm::vec3 tempPosition = newPosition - right * ZOOM_SPEED * static_cast<float>(dt);
		if (CheckCollision(tempPosition) == false) {
			newPosition = tempPosition;
		}
	}

	if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_LEFT)) {
		// Implement the camera rotations
		float angle = ROTATE_SPEED * static_cast<float>(dt);
		glm::mat4 yaw = glm::rotate(
			glm::mat4(1.f), // Default identity
			glm::radians(angle), // Convert degree angle to radians
			glm::vec3(up.x, up.y, up.z) // Use camera Up vector to rotate
		);
		// Calculate the rotated view vector
		glm::vec3 rotatedView = yaw * glm::vec4(view, 0.f);
		target = position + rotatedView;
		//isDirty = true;
	}

	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_RIGHT)) {
		// Implement the camera rotations
		float angle = ROTATE_SPEED * static_cast<float>(dt);
		glm::mat4 yaw = glm::rotate(
			glm::mat4(1.f), // Default identity
			glm::radians(-angle), // Convert degree angle to radians
			glm::vec3(up.x, up.y, up.z) // Use camera Up vector to rotate
		);
		// Calculate the rotated view vector
		glm::vec3 rotatedView = yaw * glm::vec4(view, 0.f);
		target = position + rotatedView;
		//isDirty = true;
	}

	newPosition.x = glm::clamp(newPosition.x, -40.0f, 40.0f);
	newPosition.z = glm::clamp(newPosition.z, -40.0f, 40.0f);

	if (newPosition != position) {
		glm::vec3 movement = newPosition - position;
		position = newPosition;
		target += movement; // Adjust target accordingly
		isDirty = true;
	}

	//this->Refresh();
}

void FPCamera::Refresh()
{
	if (!this->isDirty) return;
	glm::vec3 view = glm::normalize(target - position);
	glm::vec3 right = glm::normalize(glm::cross(up, view));
	// Recalculate the up vector
	//this->up = glm::normalize(glm::cross(view, right));
	this->isDirty = false;
}
