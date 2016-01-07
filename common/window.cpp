#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include <common/shader.hpp>

GLFWwindow* window; 
OpenGLWindow openGLWindow;
GLuint programID;

//Delta calculation
double delta, lastLoopTime;

// Used for FPS calculation
int FPSCount, currentFPS;
double lastSecond;

void logFPS();
void updateDelta();
void frameBufferSizeChanged(GLFWwindow* window, int width, int height);

void updateDelta() {
	delta = glfwGetTime() - lastLoopTime;
	lastLoopTime = glfwGetTime();
}

void logFPS() {
	if ((glfwGetTime() - lastSecond) >= 1)
	{
		lastSecond = glfwGetTime();
		FPSCount = currentFPS;
		currentFPS = 0;
	}
	currentFPS++;
	if (openGLWindow.bShowFPS) {
		char buf[55];
		sprintf(buf, "FPS: %d, V-Sync: %s", FPSCount, openGLWindow.bVerticalSync ? "On" : "Off");
		glfwSetWindowTitle(window, buf);
	}
}

int OpenGLWindow::createAndShowWindow()
{
	bShowFPS = true;

	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		std::cin.get();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Title", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials." << std::endl;
		std::cin.get();
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, frameBufferSizeChanged);
	glfwSetKeyCallback(window, keyEvent);
	glfwSetCursorPosCallback(window, mouseMoved);
    glfwSetMouseButtonCallback(window, mouseClicked);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		std::cin.get();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Use our shader
	glUseProgram(programID);

	//Setup projection and view
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glm::mat4 projection = glm::perspective(45.0f, (float)width / height, 0.001f, 1000.0f);
	GLuint uniProj = glGetUniformLocation(programID, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

	glfwSwapInterval(1);
	bVerticalSync = true;
	initScene();

	do {
		logFPS();
		updateDelta();
        doLogic();
		renderScene();
		glfwPollEvents();
		glfwSwapBuffers(window);

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	releaseScene();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

float sof(float fVal) {
	return fVal*delta;
}

void frameBufferSizeChanged(GLFWwindow* window, int width, int height) {
	if (width != 0 && height != 0) {
		glm::mat4 projection = glm::perspective(45.0f, (float)width / height, 0.001f, 1000.0f);
		GLuint uniProj = glGetUniformLocation(programID, "proj");
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));
	}
}

void OpenGLWindow::disableCursor(bool mode){
    if(mode){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }else{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

int OpenGLWindow::getKey(int key){
    return glfwGetKey(window, key);
}

double OpenGLWindow::getDelta(){
    return delta;
}