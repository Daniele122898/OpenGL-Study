#include "GLWindow.h"

GLWindow::GLWindow()
{
	width = 800;
	height = 600;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;
}

GLWindow::GLWindow(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;
}

int GLWindow::Initialise()
{
	// Initialize GLFW
	if (!glfwInit())
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

	mainWindow = glfwCreateWindow(width, height, "OpenGL Study", NULL, NULL);
	if (!mainWindow)
	{
		printf("Failed to create GLFW window");
		glfwTerminate();
		return 1;
	}

	// Get the buffer size information
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
	return 0;
}

GLWindow::~GLWindow()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
