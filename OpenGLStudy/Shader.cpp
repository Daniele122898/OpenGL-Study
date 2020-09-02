#include "Shader.h"

Shader::Shader()
{
	shaderId = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragCode)
{
	compileShader(vertexCode, fragCode);
}

void Shader::CreateFromFiles(const char* vertexPath, const char* fragPath)
{
	std::string vertexString = ReadFile(vertexPath);
	std::string fragString = ReadFile(fragPath);
	const char* vertexCode = vertexString.c_str();
	const char* fragCode = fragString.c_str();

	compileShader(vertexCode, fragCode);
}

std::string Shader::ReadFile(const char* path) const
{
	std::string content;
	std::ifstream fileStream(path, std::ios::in);

	if (!fileStream.is_open())
	{
		printf("Failed to read %s! File doesn't exist", path);
		return "";
	}

	std::string line;
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void Shader::UseShader() const
{
	glUseProgram(shaderId);
}

void Shader::ClearShader()
{
	if (shaderId != 0)
	{
		glDeleteProgram(shaderId);
		shaderId = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

Shader::~Shader()
{
	ClearShader();
}

void Shader::UnbindShader()
{
	glUseProgram(0);
}

void Shader::compileShader(const char* vertexCode, const char* fragCode)
{
	shaderId = glCreateProgram(); // Create shader program and return ID
	if (!shaderId)
	{
		printf("Error creating shader program! \n");
		return;
	}

	addShader(shaderId, vertexCode, GL_VERTEX_SHADER);
	addShader(shaderId, fragCode, GL_FRAGMENT_SHADER);

	// Get error codes from shader creation
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderId); // create the shader
	glGetProgramiv(shaderId, GL_LINK_STATUS, &result); // Check if linking worked
	if (!result)
	{
		glGetProgramInfoLog(shaderId, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shaderId); // Check if shader is valid in current OpenGL context
	glGetProgramiv(shaderId, GL_VALIDATE_STATUS, &result); // Check if validation worked
	if (!result)
	{
		glGetProgramInfoLog(shaderId, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	// Get uniform Id
	uniformModel = glGetUniformLocation(shaderId, "model");
	uniformProjection = glGetUniformLocation(shaderId, "projection");
	uniformView = glGetUniformLocation(shaderId, "view");
	
	uniformAmbientColor = glGetUniformLocation(shaderId, "directionalLight.color");
	uniformAmbientIntensity = glGetUniformLocation(shaderId, "directionalLight.ambientIntensity");
	uniformDiffuseDirection = glGetUniformLocation(shaderId, "directionalLight.direction");
	uniformDiffuseIntensity = glGetUniformLocation(shaderId, "directionalLight.diffuseIntensity");
}

void Shader::addShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType); // Create empty shader of that type

	const GLchar* shCode[1];
	shCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = static_cast<GLint>(strlen(shaderCode));

	glShaderSource(shader, 1, shCode, codeLength);
	glCompileShader(shader);

	// Get error codes from shader compilation
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result); // Check if compilation worked
	if (!result)
	{
		glGetShaderInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(shaderProgram, shader);
}
