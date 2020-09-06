#include "Texture.h"

Texture::Texture()
{
	textureId = 0;
	width = height = bitDepth = 0;
	imageFormat = GL_RGBA;
	filePath = "";
}

Texture::Texture(const char* filePath)
{
	textureId = 0;
	width = height = bitDepth = 0;
	imageFormat = GL_RGBA;
	this->filePath = filePath;
}

Texture::Texture(const char* filePath, GLint imageFormat)
{
	textureId = 0;
	width = height = bitDepth = 0;
	this->filePath = filePath;
	this->imageFormat = imageFormat;
}

bool Texture::LoadTexture()
{
	unsigned char* texData = stbi_load(filePath, &width, &height, &bitDepth, 0);
	if (!texData)
	{
		printf("Failed to find texture: %s\n", filePath);
		return false;
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData); // Free data up again since its in GPU now
	return true;
}

void Texture::UseTexture()
{
	// TODO parameterize the texture unit
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::ClearTexture()
{
	if (textureId != 0)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	width = height = bitDepth = 0;
	filePath = (char*)"";
}

Texture::~Texture()
{
	ClearTexture();
}
