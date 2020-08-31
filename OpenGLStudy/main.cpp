#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Screen and window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

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
	
	// loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and handle user input events
		glfwPollEvents();

		// Clear the window
		glClearColor(1.0f, 0.0f, 1.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (since each pixel can have more than just color data [depth etc...])

		glfwSwapBuffers(mainWindow); // You have 2 buffers, One that is seen and one that is being drawn to. So you just exchange those (double buffer)
	}
	
	
	return 0;
}
