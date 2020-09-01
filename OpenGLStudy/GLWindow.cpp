#include "GLWindow.h"


GLWindow::GLWindow()
{
	width = 800;
	height = 600;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;

	lastX = 0;
	lastY = 0;
	xChange = 0;
	yChange = 0;

	mouseFirstMoved = false;

	memset(keys, 0, sizeof(bool) * 1024);
}

GLWindow::GLWindow(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	bufferWidth = 0;
	bufferHeight = 0;
	mainWindow = nullptr;

	lastX = 0;
	lastY = 0;
	xChange = 0;
	yChange = 0;
	
	mouseFirstMoved = false;

	memset(keys, 0, sizeof(bool) * 1024);
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

	// Handle key and mouse inputs
	createCallbacks();
	// To capture mouse in game
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
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

	glfwSetWindowUserPointer(mainWindow, this); // So the window has a pointer to this class
	
	return 0;
}

GLfloat GLWindow::GetXChange()
{
	GLfloat change = xChange;
	xChange = 0;
	return change;
}

GLfloat GLWindow::GetYChange()
{
	GLfloat change = yChange;
	yChange = 0;
	return change;
}

GLWindow::~GLWindow()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}

void GLWindow::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

void GLWindow::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	GLWindow* ourWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if(action == GLFW_PRESS)
		{
			ourWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			ourWindow->keys[key] = false;
		}
	}
}

void GLWindow::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	GLWindow* ourWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

	if (!ourWindow->mouseFirstMoved)
	{
		ourWindow->lastX = xPos;
		ourWindow->lastY = yPos;
		ourWindow->mouseFirstMoved = true;
	}

	ourWindow->xChange = xPos - ourWindow->lastX;
	ourWindow->yChange = ourWindow->lastY - yPos; // Flipped to not have inverted controls.

	ourWindow->lastX = xPos;
	ourWindow->lastY = yPos;
}
