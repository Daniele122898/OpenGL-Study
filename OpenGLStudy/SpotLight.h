#pragma once
#include "PointLight.h"


class SpotLight :
	public PointLight
{

public:
	SpotLight();

	SpotLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity,
		GLfloat diffIntensity, GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat edg);

	void UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation,
		GLint diffuseIntensityLocation, GLint positionLocation,
		GLint constantLocation, GLint linearLocation, GLint exponentLocation,
		GLint directionLocation, GLint edgeLocation);

	void SetFlash(glm::vec3 pos, glm::vec3 dir);
	
	~SpotLight();
	
private:
	glm::vec3 direction;

	GLfloat edge, procEdge; // angle
};

