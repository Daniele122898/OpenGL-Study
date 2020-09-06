#include "Shader.h"

Shader::Shader()
{
	shaderId = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
	uniformPointLightCount = 0;
	uniformSpotLightCount = 0;
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

void Shader::SetDirectionalLight(DirectionalLight* directionalLight)
{
	directionalLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformAmbientColor,
								uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDiffuseDirection);
}

void Shader::SetPointLights(PointLight* pLight, int lightCount)
{
	if (lightCount > MAX_POINT_LIGHTS)
	{
		lightCount = MAX_POINT_LIGHTS; // Just clamp it and fuck it :P
		printf("Error: Too many point lights defined. Ignoring excess.");
	}

	glUniform1i(uniformPointLightCount, lightCount);

	for (int i = 0; i < lightCount; i++)
	{
		pLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformAmbientColor,
			uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition, uniformPointLight[i].uniformConstant,
			uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);
	}
}

void Shader::SetSpotLights(SpotLight* spLight, int lightCount)
{
	if (lightCount > MAX_SPOT_LIGHTS)
	{
		lightCount = MAX_SPOT_LIGHTS; // Just clamp it and fuck it :P
		printf("Error: Too many spot lights defined. Ignoring excess.");
	}

	glUniform1i(uniformSpotLightCount, lightCount);

	for (int i = 0; i < lightCount; i++)
	{
		spLight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformAmbientColor,
			uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformConstant,
			uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformDirection, uniformSpotLight[i].uniformEdge);
	}
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
	
	uniformDirectionalLight.uniformAmbientColor = glGetUniformLocation(shaderId, "directionalLight.base.color");
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderId, "directionalLight.base.ambientIntensity");
	uniformDirectionalLight.uniformDiffuseDirection = glGetUniformLocation(shaderId, "directionalLight.direction");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderId, "directionalLight.base.diffuseIntensity");
	
	uniformSpecularIntensity = glGetUniformLocation(shaderId, "material.specularIntensity");
	uniformShininess = glGetUniformLocation(shaderId, "material.shininess");
	uniformEyePosition = glGetUniformLocation(shaderId, "eyePosition");

	uniformPointLightCount = glGetUniformLocation(shaderId, "pointLightCount");

	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
		uniformPointLight[i].uniformAmbientColor = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity= glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity= glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
		uniformPointLight[i].uniformExponent= glGetUniformLocation(shaderId, locBuff);
	}

	uniformSpotLightCount = glGetUniformLocation(shaderId, "spotLightCount");

	for (int i = 0; i < MAX_SPOT_LIGHTS; i++)
	{
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.color", i);
		uniformSpotLight[i].uniformAmbientColor = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderId, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderId, locBuff);
	}
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
