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
	glm::vec3 view = glm::normalize(target - position); // calculate the new view vector
	glm::vec3 right = glm::normalize(glm::cross(view, up));
	//foward or backward
	if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W)) {
		//forward
		//position += view * ZOOM_SPEED * static_cast<float>(dt);
		//target += view * ZOOM_SPEED * static_cast<float>(dt);
		glm::vec3 horizontalView = glm::normalize(glm::vec3(view.x, 0.0f, view.z));
		position += horizontalView * ZOOM_SPEED * static_cast<float>(dt);
		target += horizontalView * ZOOM_SPEED * static_cast<float>(dt);

		isDirty = true;
	}
	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S)) {
		//backward
		//position -= view * ZOOM_SPEED * static_cast<float>(dt);
		//target -= view * ZOOM_SPEED * static_cast<float>(dt);
		glm::vec3 horizontalView = glm::normalize(glm::vec3(view.x, 0.0f, view.z));
		position -= horizontalView * ZOOM_SPEED * static_cast<float>(dt);
		target -= horizontalView * ZOOM_SPEED * static_cast<float>(dt);
		isDirty = true;
	}

	//strafe
	if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D)) {
		//right
		position += right * ZOOM_SPEED * static_cast<float>(dt);
		target += right * ZOOM_SPEED * static_cast<float>(dt);

		isDirty = true;
	}
	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A)) {
		//left
		position -= right * ZOOM_SPEED * static_cast<float>(dt);
		target -= right * ZOOM_SPEED * static_cast<float>(dt);

		isDirty = true;
	}

	//Yaw rotation
	if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_LEFT)) {
		// Calculate amount of angle to rotate
		float angle = ROTATE_SPEED * static_cast<float>(dt);

		glm::mat4 yaw = glm::rotate(
			glm::mat4(1.f), // Default identity
			glm::radians(angle), // Convert degree angle to radians
			glm::vec3(up.x, up.y, up.z) // Use camera Up vector to rotate
		);

		// Calculate the rotated view vector
		glm::vec3 rotatedView = yaw * glm::vec4(view, 0.f);
		target = position + rotatedView;

		isDirty = true;
	}
	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_RIGHT)) {
		// Calculate amount of angle to rotate
		float angle = -ROTATE_SPEED * static_cast<float>(dt);

		glm::mat4 yaw = glm::rotate(
			glm::mat4(1.f), // Default identity
			glm::radians(angle), // Convert degree angle to radians
			glm::vec3(up.x, up.y, up.z) // Use camera Up vector to rotate
		);

		// Calculate the rotated view vector
		glm::vec3 rotatedView = yaw * glm::vec4(view, 0.f);
		target = position + rotatedView;

		isDirty = true;
	}

	//pitch rotation
	if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_UP)) {
		right.y = 0; // Right vector should not point any y direction

		// Calculate amount of angle to rotate
		float angle = ROTATE_SPEED * static_cast<float>(dt);

		glm::mat4 pitch = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(right.x, right.y, right.z)); // Rotate on right vector

		// Calculate the rotated view vector
		glm::vec3 rotatedView = pitch * glm::vec4(view, 0.f);
		target = position + rotatedView;

		isDirty = true;
	}
	else if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_DOWN)) {
		right.y = 0; // Right vector should not point any y direction

		// Calculate amount of angle to rotate
		float angle = -ROTATE_SPEED * static_cast<float>(dt);

		glm::mat4 pitch = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(right.x, right.y, right.z)); // Rotate on right vector

		// Calculate the rotated view vector
		glm::vec3 rotatedView = pitch * glm::vec4(view, 0.f);
		target = position + rotatedView;

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
