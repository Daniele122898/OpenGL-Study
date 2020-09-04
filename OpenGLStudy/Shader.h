#pragma once

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>


#include "Constants.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class Shader
{
public:
	Shader();

	void CreateFromString(const char* vertexCode, const char* fragCode);
	void CreateFromFiles(const char* vertexPath, const char* fragPath);

	std::string ReadFile(const char* path) const;

	GLuint GetProjectionLocation() { return uniformProjection; }
	GLuint GetModelLocation() const { return uniformModel; }
	GLuint GetViewLocation() const { return uniformView; }
	
	GLuint GetAmbientIntensityLocation() const { return uniformDirectionalLight.uniformAmbientIntensity; }
	GLuint GetAmbientColorLocation() const 	{ return uniformDirectionalLight.uniformAmbientColor; }

	GLuint GetDiffuseDirectionLocation() const { return uniformDirectionalLight.uniformDiffuseDirection; }
	GLuint GetDiffuseIntensityLocation() const { return uniformDirectionalLight.uniformDiffuseIntensity; }

	GLuint GetSpecularIntensityLocation() const { return uniformSpecularIntensity; }
	GLuint GetShininessLocation() const { return uniformShininess; }
	GLuint GetEyeLocation() const { return uniformEyePosition; }

	void SetDirectionalLight(DirectionalLight* directionalLight);
	void SetPointLight(PointLight* pLight, int lightCount);

	void UseShader() const;
	void ClearShader();
	
	~Shader();

	static void UnbindShader();

private:
	GLuint shaderId, uniformProjection, uniformModel, uniformView,
		uniformEyePosition, uniformSpecularIntensity, uniformShininess;

	struct
	{
		GLuint uniformAmbientColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDiffuseDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightCount;
	struct
	{
		GLuint uniformAmbientColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];


	void compileShader(const char* vertexCode, const char* fragCode);
	static void addShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType);
};

