#include <cstring>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "GLWindow.h"

const float toRadians = 3.14159265f / 180.f; // Degrees times this will give radians


GLWindow mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

float currRot = 0;

static const char* vertexShader = "Shaders/shader.vert";
static const char* fragShader = "Shaders/shader.frag";

void CreateObjects()
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

	Mesh* mesh = new Mesh();
	mesh->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(mesh);

	Mesh* mesh2 = new Mesh();
	mesh2->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(mesh2);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vertexShader, fragShader);
	shaderList.push_back(shader1);
}

int main()
{
	mainWindow = GLWindow(800, 600);
	mainWindow.Initialise();

	// Create triangle
	CreateObjects();
	CreateShaders();

	// Projection doesn't change so no need to recalculate
	glm::mat4 projection = glm::perspective(45.0f, 
		static_cast<GLfloat>(mainWindow.GetBufferWidth()) / static_cast<GLfloat>(mainWindow.GetBufferHeight()),
		0.1f, 100.0f);

	GLuint uniformProjection = 0, uniformModel = 0;
	
	// loop until window closed
	while (!mainWindow.GetShouldClose())
	{
		// Get and handle user input events
		glfwPollEvents();

		// rotate
		currRot += 0.3f;
		if (currRot >= 360)
			currRot = -360;

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer (since each pixel can have more than just color data [depth etc...])

		shaderList[0]->UseShader();
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();

		glm::mat4 model = glm::mat4(1.0f); // Initialise as identity matrix

		// Order of these transformations is very important. If you rotate first and then translate you would also rotate the translation
		// thus the triangle would move at 45° instead of the X axis!
		model = glm::translate(model, glm::vec3(0.f, -0.5f, -2.5f));
		// The triangle will be distorted because we dont use a projection matrix. The triangle is created based on the
		// viewport dimensions. Thus if you'd rotate the triangle by 90° it would be stretched and distorted. We need to use
		// a projection matrix to tell it to scale to 'world positions' so when it gets rotate it would keep its aspect ratio and size
		model = glm::rotate(model, currRot * toRadians, glm::vec3(0.f, 1.f, 0.0f)); // Rotate around Z axis from origin
		model = glm::scale(model, glm::vec3(.3f, .3f, 0.3f));

		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, .5f, -2.5f));
		model = glm::rotate(model, -currRot * toRadians, glm::vec3(0.f, 1.f, 0.0f));
		model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		meshList[1]->RenderMesh();

		// glUseProgram(0); // Unbind shader program
		Shader::UnbindShader();
		
		mainWindow.SwapBuffers(); // You have 2 buffers, One that is seen and one that is being drawn to. So you just exchange those (double buffer)
	}
	
	
	return 0;
}
