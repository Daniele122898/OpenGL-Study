#pragma once

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public:
	Shader();

	void CreateFromString(const char* vertexCode, const char* fragCode);
	void CreateFromFiles(const char* vertexPath, const char* fragPath);

	std::string ReadFile(const char* path) const;

	GLuint GetProjectionLocation() const;
	GLuint GetModelLocation() const;
	GLuint GetViewLocation() const;

	void UseShader() const;
	void ClearShader();
	
	~Shader();

	static void UnbindShader();

private:
	GLuint shaderId, uniformProjection, uniformModel, uniformView;

	void compileShader(const char* vertexCode, const char* fragCode);
	static void addShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType);
};

