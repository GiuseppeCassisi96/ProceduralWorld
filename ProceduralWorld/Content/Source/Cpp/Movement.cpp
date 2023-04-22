#include "Movement.h"

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
	//I compute the delta time in order to do a moment based on frame time
	currentTime = static_cast<float>(glfwGetTime());
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		position -= forwardAxe * (PLAYER_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		position += forwardAxe * (PLAYER_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		position -= rightAxe * (PLAYER_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		position += rightAxe * (PLAYER_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		position += upAxe * (PLAYER_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		position -= upAxe * (PLAYER_SPEED * deltaTime);
	}
	//Recompute the rightAxe, upAxe and view matrix
	rightAxe = glm::normalize(glm::cross(forwardAxe, glm::vec3(0.0f, 1.0f, 0.0f)));
	viewMatrix = glm::lookAt(position, position + (-forwardAxe), glm::vec3(0.0f, 1.0f, 0.0f));

}

void Movement::Rotate(double xpos, double ypos)
{
	float fXPos = static_cast<float>(xpos);
	float fYPos = static_cast<float>(ypos);
	if (firstMouse)
	{
		lastX = fXPos;
		lastY = fYPos;
		firstMouse = false;
	}

	//I compute the new xPos and yPos offset
	float xoffset = fXPos - lastX;
	float yoffset = lastY - fYPos;
	lastX = fXPos;
	lastY = fYPos;

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch -= yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	//Recompute the fowardAxe
	forwardAxe = glm::normalize(direction);
}