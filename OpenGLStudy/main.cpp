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
#include "DirectionalLight.h"
#include "Material.h"
#include "Constants.h"
#include "PointLight.h"
#include "SpotLight.h"

const float toRadians = 3.14159265f / 180.f; // Degrees times this will give radians


GLWindow mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture floorTexture;

Material shinyMaterial;
Material dullMaterial;
Material floorMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

static const char* vertexShader = "Shaders/shader.vert";
static const char* fragShader = "Shaders/shader.frag";

// So the vertices get averaged normals to not have linear lightning (interpolate the normal vectors)
void calcAverageNormals(const unsigned int* indices, unsigned int indicesCount, GLfloat* vertices, 
						unsigned int vertexCount, unsigned int vLength, unsigned normalOffset)
{
	for (size_t i = 0; i < indicesCount; i+=3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i+1] * vLength;
		unsigned int in2 = indices[i+2] * vLength;

		// Create two lines between the indexed triangle to then use the cross product to get a 90 degree vector which will be our normal
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	const unsigned int n = vertexCount / vLength;
	for (size_t i = 0; i < n; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

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
	//  x		y		z		u     v		nx	   ny   nz
		-1.0f,	-1.0f,	-0.6f,	0.f,  0.f,	0.0f, 0.0f, 0.0f, 		// Bottom left corner
		0.0f,	-1.0f,	1.0f,	0.5f, 0.f,	0.0f, 0.0f, 0.0f,		// for pyramid go into Z
		1.0f,	-1.0f,	-0.6f,	1.f,  0.f,	0.0f, 0.0f, 0.0f,		// Bottom right corner
		0.0f,	1.0f,	0.0f,	0.5f, 1.f,	0.0f, 0.0f, 0.0f,		// Middle of top
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* mesh = new Mesh();
	mesh->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(mesh);

	Mesh* mesh2 = new Mesh();
	mesh2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(mesh2);

	unsigned int floorIndices[] = {
		0, 2, 1, // go anti-clock wise from top left to bottom left to top right
		1, 2, 3
	};

	// Create flat floor, use 10 for uv so it tiles and repeats, with manual normals so its perfectly flat
	GLfloat floorVertices[] = {
		-10.f, 0.0f, -10.f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f, // top left
		10.0f, 0.0f, -10.f,		10.0f, 0.0f,	0.0f, -1.0f, 0.0f, // top right
		-10.0f, 0.0f, 10.f,		0.0f, 10.0f,	0.0f, -1.0f, 0.0f, // bottom left
		10.0f, 0.0f, 10.f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f, // bottom right
	};

	Mesh* mesh3 = new Mesh();
	mesh3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(mesh3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vertexShader, fragShader);
	shaderList.push_back(shader1);
}

int main()
{
	mainWindow = GLWindow(1920, 1080);
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

	floorTexture = Texture((char*)"Textures/floor.jpg", GL_RGB);
	floorTexture.LoadTexture();

	shinyMaterial = Material(1.f, 64.f);
	dullMaterial = Material(0.3f, 8.f);
	floorMaterial = Material(0.8f, 256.f);

	mainLight = DirectionalLight(1.f, 1.f, 0.7f, 0.1f, 0.3f, 
								2.0f, -1.0f, -2.f);

	int pointLightCount = 0;
	
	pointLights[0] = PointLight(0.f, 1.f, 0.f, 
		0.1f, 0.3f, 
		0.f, 1.5f, 7.f,
		0.3f, 0.2f, 0.1f);

	++pointLightCount;

	pointLights[1] = PointLight(0.5f, 0.f, 1.f,
		0.2f, 0.3f,
		6.f, -2.f, -2.f,
		0.3f, 0.2f, 0.1f);

	++pointLightCount;

	int spotLightCount = 0;

	spotLights[0] = SpotLight(1.f, 1.f, 1.f,
		0.1f, 2.0f,
		0.f, 1.5f, -7.f,
		0.3f, 0.2f, 0.1f,
		0.0f, -1.f, 0.1f, 20.f);

	++spotLightCount;

	// Projection doesn't change so no need to recalculate
	glm::mat4 projection = glm::perspective(45.0f, 
		static_cast<GLfloat>(mainWindow.GetBufferWidth()) / static_cast<GLfloat>(mainWindow.GetBufferHeight()),
		0.1f, 100.0f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
			uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	
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
		
		uniformSpecularIntensity = shaderList[0]->GetSpecularIntensityLocation();
		uniformShininess = shaderList[0]->GetShininessLocation();
		uniformEyePosition = shaderList[0]->GetEyeLocation();

		glm::vec3 lowerLight = camera.GetCameraPosition();
		lowerLight.y -= 0.3f;
		
		spotLights[0].SetFlash(lowerLight, camera.GetCameraDirection());

		shaderList[0]->SetDirectionalLight(&mainLight);
		shaderList[0]->SetPointLights(pointLights, pointLightCount);
		shaderList[0]->SetSpotLights(spotLights, spotLightCount);

		// mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColor, 
		// 					uniformDiffuseIntensity, uniformDirection);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.CalculateViewMatrix()));
		const glm::vec3 eyePos = camera.GetCameraPosition();
		glUniform3f(uniformEyePosition, eyePos.x, eyePos.y, eyePos.z);
		

		glm::mat4 model = glm::mat4(1.0f); // Initialise as identity matrix

		// Order of these transformations is very important. If you rotate first and then translate you would also rotate the translation
		// thus the triangle would move at 45° instead of the X axis!
		model = glm::translate(model, glm::vec3(0.f, -1.f, -2.5f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		brickTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, 1.f, -2.5f));
		//model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		dirtTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[1]->RenderMesh();

		// Floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, -2.f, 0.f));
		//model = glm::scale(model, glm::vec3(.5f, .5f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		floorTexture.UseTexture();
		floorMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		// glUseProgram(0); // Unbind shader program
		Shader::UnbindShader();
		
		mainWindow.SwapBuffers(); // You have 2 buffers, One that is seen and one that is being drawn to. So you just exchange those (double buffer)
	}
	
	
	return 0;
}
