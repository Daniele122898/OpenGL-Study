#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Screen and window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformXMove;

bool directionRight = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

// Temporary Vertex Shader
static const char* vertexShader = "#version 330									\n\
layout (location = 0) in vec3 pos;												\n\
out vec3 vPos;																	\n\
uniform float xMove;															\n\
void main()																		\n\
{																				\n\
	vPos = vec3(0.8 * pos.x + xMove, 0.8* pos.y, pos.z);						\n\
	gl_Position = vec4(vPos, 1.0); // gl_Position is the output of this shader	\n\
}																				\n\
";

// Temporary fragment shader
static const char* fragShader = "#version 330									\n\
out vec4 color;																	\n\
in vec3 vPos; \n\
void main()																		\n\
{																				\n\
	color = vec4(vPos.x * 1.0f, vPos.y * 1.0f, 0.0f, 1.0);										\n\
}																				\n\
";

void CreateTriangle()
{
	// Create triangle first
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, // Bottom left corner
		1.0f, -1.0f, 0.0f, // Bottom right corner
		0.0f, 1.0f, 0.0f, // Middle of top
	};

	// Create VAO, :(amount of array we want to create, where to store the ID of the array)
	glGenVertexArrays(1, &VAO); // Will now create space in GPU memory and return ID for VAO
	glBindVertexArray(VAO); // Now bind this VAO.
	{
		// Create VBO, :(amount of buffers we want to create, where to store the ID of the buffer)
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Static draw: Won't change values once in

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

	}
	glBindVertexArray(0); // Unbind VAO
}

void AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint shaderId = glCreateShader(shaderType); // Create empty shader of that type

	const GLchar* shCode[1];
	shCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

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
	uniformXMove = glGetUniformLocation(shader, "xMove");
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

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight); // set width and height to the actual size inside the window we got when getting the buffer sizes

	// Create triangle
	CreateTriangle();
	CompileShaders();
	
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

		// Clear the window
		glClearColor(1.0f, 0.0f, 1.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (since each pixel can have more than just color data [depth etc...])

		glUseProgram(shader);

		glUniform1f(uniformXMove, triOffset);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		
		glBindVertexArray(0);
		
		glUseProgram(0); // Unbind shader program
		
		glfwSwapBuffers(mainWindow); // You have 2 buffers, One that is seen and one that is being drawn to. So you just exchange those (double buffer)
	}
	
	
	return 0;
}
