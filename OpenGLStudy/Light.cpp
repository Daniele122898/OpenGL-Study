#include "Light.h"

Light::Light()
{
	color = glm::vec3(1.f, 1.f, 1.f);
	ambientIntensity = 1.f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity)
{
	color = glm::vec3(red, green, blue);
	ambientIntensity = ambIntensity;
}

void Light::UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation)
{
	glUniform3f(ambientColorLocation, color.x, color.y, color.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);
}

Light::~Light()
{
}
