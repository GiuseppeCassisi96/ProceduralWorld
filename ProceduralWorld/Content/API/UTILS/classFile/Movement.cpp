#include "UTILS/include/Movement.h"

Movement::Movement(glm::vec3 initialPos, glm::mat4& viewMatrix) : position{ initialPos }
{
	/*I obtain the camera forward direction by subtract the position of camera and
	 *the position of center of scene. I have a reverse order because I want a positive
	 *Z-axis*/
	
	forwardAxe = glm::normalize(position - centerOfScene);
	/*I obtain the rightAxe by doing the cross product between the UPWorldAxe and forwardAxe*/
	rightAxe = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forwardAxe));
	upAxe = glm::cross(forwardAxe, rightAxe);
	viewMatrix = glm::lookAt(position, centerOfScene, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Movement::Move(GLFWwindow* window, glm::mat4& viewMatrix)
{
	currentTime = static_cast<float>(glfwGetTime());
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	if(glfwGetKey(window, GLFW_KEY_W))
	{
		position -=  forwardAxe * (PLAYER_MOVEMENT * deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_S))
	{
		position += forwardAxe * (PLAYER_MOVEMENT * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		position += rightAxe * (PLAYER_MOVEMENT * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		position -= rightAxe * (PLAYER_MOVEMENT * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		position += upAxe * (PLAYER_MOVEMENT * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		position -= upAxe * (PLAYER_MOVEMENT * deltaTime);
	}
	rightAxe = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forwardAxe));
	viewMatrix = glm::lookAt(position, position + (-forwardAxe), glm::vec3(0.0f, 1.0f, 0.0f));
	
}

void Movement::Rotate(double xpos, double ypos)
{
	float fXpos = static_cast<float>(xpos);
	float fYpos = static_cast<float>(ypos);
	if(firstMouse)
	{
		lastX = fXpos;
		lastY = fYpos;
	}
	float xOffset = fXpos - lastX;
	float yOffset = lastY - fYpos;
	lastX = fXpos;
	lastY = fYpos;
	xOffset *= rotationSensitivity;
	yOffset *= rotationSensitivity;
	yaw += xOffset;
	pitch -= yOffset;
	if(pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	else if(pitch < -89.0f)
	{
		pitch = -89.0f;
	}
	if (!firstMouse)
	{
		cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection.y = sin(glm::radians(pitch));
		cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forwardAxe = glm::normalize(cameraDirection);
		firstMouse = false;
	}
	
}




