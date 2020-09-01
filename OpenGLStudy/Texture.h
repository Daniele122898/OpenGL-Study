#pragma once

#include <cstring>

#include <GL/glew.h>

#include "stb_image.h"

class Texture
{
public:
	Texture();
	Texture(char* filePath);
	Texture(char* filePath, GLint imageFormat);

	void LoadTexture();
	void UseTexture();
	void ClearTexture();

	~Texture();

private:
	GLuint textureId;
	GLuint imageFormat;
	int width, height, bitDepth;

	char* filePath;
};

