#include "Camera.h"


void Camera::processKeyboard(float deltaTime)
{
	processController(deltaTime);

	float cameraSpeed = movementSpeed * deltaTime;
	const bool* keyState = SDL_GetKeyboardState(NULL);

	if (keyState[SDL_SCANCODE_W])
		//std::cout << "this is hell" << std::endl;
		cameraPos += cameraSpeed * cameraFront;
	else if (keyState[SDL_SCANCODE_S])
		cameraPos -= cameraSpeed * cameraFront;
	//Theoretically front and up might not have to be normalized
	if (keyState[SDL_SCANCODE_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	else if (keyState[SDL_SCANCODE_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyState[SDL_SCANCODE_SPACE] || keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT])
		cameraPos += cameraSpeed * normalize(cameraUp);
	if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL])
		cameraPos -= cameraSpeed * normalize(cameraUp);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		std::cout << "Quit" << std::endl;
		exit(0);
		//Doesnt work
		//void SDL_Quit(void);
	}
}

void Camera::processInput(SDL_Event ev, float deltaTime)
{	
	//Mouse zoom in
	if (ev.type == SDL_EVENT_MOUSE_WHEEL)
	{
		if (fov >= 1.0f && fov <= 45.f)
			fov -= ev.wheel.y * 10;
		if (fov <= 1.0f)
			fov = 1.0f;
		if (fov >= 45.f)
			fov = 45.f;
	}

	if (ev.type == SDL_EVENT_MOUSE_MOTION)
	{
		float xpos = ev.motion.x;
		float ypos = ev.motion.y;

		//Initializes lasX and lasY if its first mouse motion event
		static bool firstMouse = true;
		if (firstMouse)
		{
			firstMouse = false;
			lastX = xpos;
			lastY = ypos;
		}


		//Fixed problem when mouse tries to leave the window there is no change in position
		float xoffset = xpos + ev.motion.xrel - lastX;
		float yoffset = lastY - ypos - ev.motion.yrel;
		lastX = xpos;
		lastY = ypos;

		//float xoffset = xpos - ev.motion.xrel;
		//float yoffset = lastY - ypos;

		//float sensitivity = 150.f*deltaTime;
		float sensitivity = 0.05f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		//Prevents strange camera
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		//Not necessary but if I need to use the yaw for some calculation in the future it is good to have it clamped
		if (yaw > 180.0f) yaw = -180.0f;
		if (yaw < -180.0f) yaw = 180.0f;

		//Debug
		//std::cout << "ev.motion.yrel" << std::endl;
		//std::cout << ev.motion.yrel << std::endl;


		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraFront = glm::normalize(front);
	}
}

glm::mat4 Camera::getViewMatrix()
{
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}

//───────────────────────────────────────────────────────────Controllers───────────────────────────────────────────────────────────
void Camera::openController()
{
	//Get the first controller
	int count = 0;
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
	for (int i = 0; i < count; i++)
	{
		if (SDL_IsGamepad(joysticks[i]))
		{
			gamepad = SDL_OpenGamepad(joysticks[i]);
			if (gamepad)
			{
				std::cout << "Controller connected: "
					<< SDL_GetGamepadName(gamepad) << std::endl;
				break;
			}
		}
	}
	SDL_free(joysticks);
}

void Camera::processController(float deltaTime)
{
	if (!gotController)
	{
		openController();
		gotController = true;
	}

	if (!gamepad) return;

	float cameraSpeed = movementSpeed * deltaTime;

	//ContextPr prevents them from being updated every frame
	//Need this to normalize
	constexpr float AXIS_MAX = 32767.0f;
	//Deadzone to ignore thumstick drift and minor input
	constexpr float DEAD_ZONE = 0.15f;

	//This specific part is copied from a source I can't find anymore it uses Sint16 which suposidly is the same value as the gamepad input, clamps if above max value, and if input value within deadzone ignores it
	auto normalizeAxis = [&](Sint16 raw) -> float {
		float val = raw / AXIS_MAX;
		return (glm::abs(val) < DEAD_ZONE) ? 0.0f : val;
	};


	//Movement input
	float leftX = normalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX));
	float leftY = normalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY));

	//Movement in axis map
	cameraPos -= cameraFront * leftY * cameraSpeed;
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * leftX * cameraSpeed;

	//Camera movement
	float rightX = normalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX));
	float rightY = normalizeAxis(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY));

	//Prevents strange camera
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	//Not necessary but if I need to use the yaw for some calculation in the future it is good to have it clamped
	if (yaw > 180.0f) yaw = -180.0f;
	if (yaw < -180.0f) yaw = 180.0f;

	//Up and down with triggers right Up, Left down
	if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)) cameraPos += glm::normalize(cameraUp) * cameraSpeed;
	else if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)) cameraPos -= glm::normalize(cameraUp) * cameraSpeed;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}