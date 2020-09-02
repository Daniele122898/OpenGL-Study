#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity,
			GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffIntensity);

	void UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation,
					GLint diffuseIntensityLocation, GLint directionLocation);
	
	~Light();

private:
	glm::vec3 color;
	GLfloat ambientIntensity;

	glm::vec3 direction;
	GLfloat diffuseIntensity;
};
