#define STB_IMAGE_IMPLEMENTATION

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
#include "Camera.h"
#include "Texture.h"

const float toRadians = 3.14159265f / 180.f; // Degrees times this will give radians


GLWindow mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

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
	//  x		y		z		u     v
		-1.0f,	-1.0f,	0.0f,	0.f,  0.f,	// Bottom left corner
		0.0f,	-1.0f,	1.0f,	0.5f, 0.f,	// for pyramid go into Z
		1.0f,	-1.0f,	0.0f,	1.f,  0.f,	// Bottom right corner
		0.0f,	1.0f,	0.0f,	0.5f, 1.f,		// Middle of top
	};

	Mesh* mesh = new Mesh();
	mesh->CreateMesh(vertices, indices, 20, 12);
	meshList.push_back(mesh);

	Mesh* mesh2 = new Mesh();
	mesh2->CreateMesh(vertices, indices, 20, 12);
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
	mainWindow = GLWindow(1600, 900);
	mainWindow.Initialise();

	// Create triangle
	CreateObjects();
	CreateShaders();

	camera = Camera(
		glm::vec3(0.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f), 
		-90.f, 0.f, 5.0f, 0.1f);

	brickTexture = Texture((char*)"Textures/brick.png");
	brickTexture.LoadTexture();

	dirtTexture = Texture((char*)"Textures/dirt.png");
	dirtTexture.LoadTexture();

	// Projection doesn't change so no need to recalculate
	glm::mat4 projection = glm::perspective(45.0f, 
		static_cast<GLfloat>(mainWindow.GetBufferWidth()) / static_cast<GLfloat>(mainWindow.GetBufferHeight()),
		0.1f, 100.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	
	// loop until window closed
	while (!mainWindow.GetShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get and handle user input events
		glfwPollEvents();

		camera.KeyControl(mainWindow.GetKeys(), deltaTime);
		camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer (since each pixel can have more than just color data [depth etc...])

		shaderList[0]->UseShader();
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();
		uniformView = shaderList[0]->GetViewLocation();

		glm::mat4 model = glm::mat4(1.0f); // Initialise as identity matrix

		// Order of these transformations is very important. If you rotate first and then translate you would also rotate the translation
		// thus the triangle would move at 45° instead of the X axis!
		model = glm::translate(model, glm::vec3(0.f, -0.5f, -2.5f));
		model = glm::scale(model, glm::vec3(.3f, .3f, 0.3f));

		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));

		brickTexture.UseTexture();
		
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, .5f, -2.5f));
		model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		dirtTexture.UseTexture();
		
		meshList[1]->RenderMesh();

		// glUseProgram(0); // Unbind shader program
		Shader::UnbindShader();
		
		mainWindow.SwapBuffers(); // You have 2 buffers, One that is seen and one that is being drawn to. So you just exchange those (double buffer)
	}
	
	
	return 0;
}
