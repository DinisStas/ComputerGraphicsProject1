#pragma once
#include <iostream>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera {
public:
	//Responsible for movement keyboard contains processController to update controler input inside
	void processKeyboard(float deltaTime);

	//Responsible for camera movement with mouse
	void processInput(SDL_Event ev, float deltaTime);

	//Responisble for controller camera and movement
	void processController(float deltaTime);

	
	glm::mat4 getViewMatrix();
	
	glm::vec3 getPosition() { return cameraPos; }

	float getFOV() { return fov; };

private:
	//───────────────────────────────────────────────────────────General setup───────────────────────────────────────────────────────────
	float fov = 45.f;

	float yaw = -90.f;
	float pitch = 0.f;

	float movementSpeed = 25.f;

	float lastX = 800, lastY = 600;

	//───────────────────────────────────────────────────────────Camera setup───────────────────────────────────────────────────────────
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	//Current camera target  is origin
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	//───────────────────────────────────────────────────────────Controller setup───────────────────────────────────────────────────────────
	//Called once to get controller
	void openController();

	SDL_Gamepad* gamepad = nullptr;
	bool gotController = false;;
};

