#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Screen and window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.f; // Degrees times this will give radians

GLuint VAO, VBO, EBO, shader, uniformModel, uniformProjection; // model matrix calculates the world pos from the model specific origin pos

bool directionRight = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

float currRot = 0;

// Temporary Vertex Shader
static const char* vertexShader = "#version 330									\n\
layout (location = 0) in vec3 pos;												\n\
uniform mat4 model;															\n\
uniform mat4 projection;															\n\
out vec4 vCol;																	\n\
void main()																		\n\
{																				\n\
	gl_Position = projection * model * vec4(pos, 1.0); // gl_Position is the output of this shader	\n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);						\n\
}																				\n\
";

// Temporary fragment shader
static const char* fragShader = "#version 330									\n\
in vec4 vCol; \n\
out vec4 color;																	\n\
void main()																		\n\
{																				\n\
	color = vCol;										\n\
}																				\n\
";

void CreateTriangle()
{
	unsigned int indices[] = {
		0, 3, 1, // side
		1, 3, 2, // side
		2, 3, 0, // front
		0, 1, 2 // bottom
	};
	
	// Create triangle first
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, // Bottom left corner
		0.0f, -1.0f, 1.0f, // for pyramid go into Z
		1.0f, -1.0f, 0.0f, // Bottom right corner
		0.0f, 1.0f, 0.0f, // Middle of top
	};

	// Create VAO, :(amount of array we want to create, where to store the ID of the array)
	glGenVertexArrays(1, &VAO); // Will now create space in GPU memory and return ID for VAO
	glBindVertexArray(VAO); // Now bind this VAO.
	{
		// Create and bind EBO
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		// Create VBO, :(amount of buffers we want to create, where to store the ID of the buffer)
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Static draw: Won't change values once in

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

	}
	glBindVertexArray(0); // Unbind VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO
}

void AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint shaderId = glCreateShader(shaderType); // Create empty shader of that type

	const GLchar* shCode[1];
	shCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = static_cast<GLint>(strlen(shaderCode));

	glShaderSource(shaderId, 1, shCode, codeLength);
	glCompileShader(shaderId);

	// Get error codes from shader compilation
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result); // Check if compilation worked
	if (!result)
	{
		glGetShaderInfoLog(shaderId, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(shaderProgram, shaderId);
}

void CompileShaders()
{
	shader = glCreateProgram(); // Create shader program and return ID
	if (!shader)
	{
		printf("Error creating shader program! \n");
		return;
	}

	AddShader(shader, vertexShader, GL_VERTEX_SHADER);
	AddShader(shader, fragShader, GL_FRAGMENT_SHADER);

	// Get error codes from shader creation
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader); // create the shader
	glGetProgramiv(shader, GL_LINK_STATUS, &result); // Check if linking worked
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader); // Check if shader is valid in current OpenGL context
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result); // Check if validation worked
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	// Get uniform Id
	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");
}

int main()
{
	// Initialize GLFW
	if(!glfwInit())
	{
		printf("Failed to initialize GLFW");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// With OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // This amounts to OpenGL version 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Won't be backwards compatible. So wont use old deprecated stuff
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Will be forward compatible

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Study", NULL, NULL);
	if (!mainWindow)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		return 1;
	}

	// Get the buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight); // Pass reference so glfw can put the values inside :D

	// set context for GLEW to use (incase of multiple windows)
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	// Initialize lib
	if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	
	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight); // set width and height to the actual size inside the window we got when getting the buffer sizes

	// Create triangle
	CreateTriangle();
	CompileShaders();

	// Projection doesn't change so no need to recalculate
	glm::mat4 projection = glm::perspective(45.0f, 
		static_cast<GLfloat>(bufferWidth) / static_cast<GLfloat>(bufferHeight),
		0.1f, 100.0f);
	
	// loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and handle user input events
		glfwPollEvents();

		if (directionRight)
		{
			triOffset += triIncrement;
		}
		else
		{
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			directionRight = !directionRight;
		}

		// rotate
		currRot += 0.3f;
		if (currRot >= 360)
			currRot = -360;

		// Clear the window
		glClearColor(1.0f, 0.0f, 1.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer (since each pixel can have more than just color data [depth etc...])

		glUseProgram(shader);

		glm::mat4 model = glm::mat4(1.0f); // Initialise as identity matrix

		// Order of these transformations is very important. If you rotate first and then translate you would also rotate the translation
		// thus the triangle would move at 45° instead of the X axis!
		model = glm::translate(model, glm::vec3(0.f, 0.f, -2.5f));
		// The triangle will be distorted because we dont use a projection matrix. The triangle is created based on the
		// viewport dimensions. Thus if you'd rotate the triangle by 90° it would be stretched and distorted. We need to use
		// a projection matrix to tell it to scale to 'world positions' so when it gets rotate it would keep its aspect ratio and size
		model = glm::rotate(model, currRot * toRadians, glm::vec3(0.f, 1.f, 0.0f)); // Rotate around Z axis from origin
		model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));

		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glUseProgram(0); // Unbind shader program
		
		glfwSwapBuffers(mainWindow); // You have 2 buffers, One that is seen and one that is being drawn to. So you just exchange those (double buffer)
	}
	
	
	return 0;
}
