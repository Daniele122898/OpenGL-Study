#include "Light.h"

Light::Light()
{
	color = glm::vec3(1.f, 1.f, 1.f);
	ambientIntensity = 1.f;

	diffuseIntensity = 0.f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity, 
				 GLfloat diffIntensity)
{
	color = glm::vec3(red, green, blue);
	ambientIntensity = ambIntensity;

	diffuseIntensity = diffIntensity;
}

Light::~Light()
{
}
