#include "Material.h"

Material::Material()
{
	specularIntensity = 0.f;
	shininess = 0.f;
}

Material::Material(GLfloat spIntensity, GLfloat shine)
{
	specularIntensity = spIntensity;
	shininess = shine;
}

void Material::UseMaterial(GLint specularIntensityLocation, GLint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}

Material::~Material()
{
}
