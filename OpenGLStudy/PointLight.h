#pragma once


#include "Light.h"


class PointLight :
	public Light
{
public:
	PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity,
		GLfloat diffIntensity, GLfloat xPos, GLfloat yPos, GLfloat zPos, 
		GLfloat con, GLfloat lin, GLfloat exp);

	void UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation,
		GLint diffuseIntensityLocation, GLint positionLocation,
		GLint constantLocation, GLint linearLocation, GLint exponentLocation);
	
	~PointLight();

protected:
	glm::vec3 position;

	GLfloat constant, linear, exponent; // 3 values for the attenuation (drop-off over distance) in quadratic function
};

