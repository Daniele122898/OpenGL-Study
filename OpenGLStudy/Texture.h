#pragma once

#include <cstring>

#include <GL/glew.h>

#include "stb_image.h"

class Texture
{
public:
	Texture();
	Texture(const char* filePath);
	Texture(const char* filePath, GLint imageFormat);

	bool LoadTexture();

	void UseTexture();
	void ClearTexture();

	~Texture();

private:
	GLuint textureId;
	GLuint imageFormat;
	int width, height, bitDepth;

	const char* filePath;
};

