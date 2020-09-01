#pragma once

#include <cstdio>
#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWindow
{
public:
	GLWindow();
	GLWindow(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLint GetBufferWidth() const { return bufferWidth; }
	GLint GetBufferHeight() const { return bufferHeight; }

	bool GetShouldClose() const { return glfwWindowShouldClose(mainWindow); }

	// TODO don't do this in future...
	bool* GetKeys() { return keys; }

	// TODO change this in future as well
	GLfloat GetXChange();
	GLfloat GetYChange();

	void SwapBuffers() { glfwSwapBuffers(mainWindow); }
	
	~GLWindow();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024];

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;

	void createCallbacks();

	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

