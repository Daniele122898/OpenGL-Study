#pragma once

#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 initialPosition, glm::vec3 initialUp, GLfloat initialYaw, GLfloat initialPitch, GLfloat moveSpeed, GLfloat turnSpeed);

	void KeyControl(bool* keys, GLfloat deltaTime);
	void MouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 GetCameraPosition() { return position; }

	glm::mat4 CalculateViewMatrix();
	
	~Camera();
	

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;	// Left-right
	GLfloat pitch;	// Up-down

	GLfloat movementSpeed;
	GLfloat turnSpeed;

	void update();
};

