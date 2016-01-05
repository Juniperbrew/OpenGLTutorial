// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <ctime>

#include <common/shader.hpp>

void initScene();
void renderScene();
void releaseScene();
float sof(float fVal);
void updateDelta();
void frameBufferSizeChanged(GLFWwindow* window, int width, int height);
void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
void logFPS();

//Delta
double delta, lastLoopTime;

//Delta
float fFrameInterval = 0;
clock_t tLastFrame;

// Used for FPS calculation
int FPSCount, currentFPS;
double lastSecond;

// Used for FPS calculation
clock_t tLastSecond;

#define HM_SIZE_X 4 // Dimensions of our heightmap
#define HM_SIZE_Y 4

GLuint uiVBOHeightmapData; // Here are stored heightmap data (vertices)
GLuint uiVBOIndices; // And here indices for rendering heightmap

GLuint uiVAOHeightmap; // One VAO for heightmap

GLuint programID;

bool bShowFPS;
bool bVerticalSync;

float fRotationAngle = 0.0f;
const float PIover180 = 3.1415f / 180.0f;

GLint uniModel;
GLint uniProj;
GLint uniView;

const char *title = "05.) Indexed Drawing - Tutorial by Michal Bubnar (www.mbsoftworks.sk)";


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, title, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, frameBufferSizeChanged);
	glfwSetKeyCallback(window, keyEvent);

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	initScene();

	do {
		logFPS();
		updateDelta();
		if (bShowFPS) {
			char buf[55]; sprintf(buf, "FPS: %d, V-Sync: %s", FPSCount, bVerticalSync ? "On" : "Off");
			glfwSetWindowTitle(window, buf);
		}
		renderScene();
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	releaseScene();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void initScene() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Setup heightmap

	glGenVertexArrays(1, &uiVAOHeightmap); // Create one VAO
	glGenBuffers(1, &uiVBOHeightmapData); // One VBO for data
	glGenBuffers(1, &uiVBOIndices); // And finally one VBO for indices

	glBindVertexArray(uiVAOHeightmap);
	glBindBuffer(GL_ARRAY_BUFFER, uiVBOHeightmapData);

	glm::vec3 vHeightmapData[HM_SIZE_X*HM_SIZE_Y]; // Here the heightmap vertices will be stored temporarily

	float fHeights[HM_SIZE_X*HM_SIZE_Y] =
	{
		4.0f, 2.0f, 3.0f, 1.0f,
		3.0f, 5.0f, 8.0f, 2.0f,
		7.0f, 10.0f, 12.0f, 6.0f,
		4.0f, 6.0f, 8.0f, 3.0f
	};

	float fSizeX = 40.0f, fSizeZ = 40.0f;

	for (int i = 0; i < HM_SIZE_X*HM_SIZE_Y;i++)
	{
		float column = float(i%HM_SIZE_X), row = float(i / HM_SIZE_X);
		vHeightmapData[i] = glm::vec3(
			-fSizeX / 2 + fSizeX*column / float(HM_SIZE_X - 1), // X Coordinate
			fHeights[i],									// Y Coordinate (it's height)
			-fSizeZ / 2 + fSizeZ*row / float(HM_SIZE_Y - 1)	// Z Coordinate
			);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*HM_SIZE_X*HM_SIZE_Y, vHeightmapData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &uiVBOIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiVBOIndices);
	int iIndices[] =
	{
		0, 4, 1, 5, 2, 6, 3, 7, 16, // First row, then restart
		4, 8, 5, 9, 6, 10, 7, 11, 16, // Second row, then restart
		8, 12, 9, 13, 10, 14, 11, 15, // Third row, no restart
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iIndices), iIndices, GL_STATIC_DRAW);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(HM_SIZE_X*HM_SIZE_Y);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Use our shader
	glUseProgram(programID);

	//Get uniforms
	uniModel = glGetUniformLocation(programID, "model");
	uniProj = glGetUniformLocation(programID, "proj");
	uniView = glGetUniformLocation(programID, "view");

	//Setup projection and view
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glm::mat4 projection = glm::perspective(45.0f, (float)width / height, 0.001f, 1000.0f);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(glm::vec3(0, 60, 30), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
}

void renderScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(uiVAOHeightmap);

	glm::mat4 model = glm::mat4();
	model = glm::rotate(model, fRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(uiVAOHeightmap);
	glDrawElements(GL_TRIANGLE_STRIP, HM_SIZE_X*(HM_SIZE_Y - 1) * 2 + HM_SIZE_Y - 2, GL_UNSIGNED_INT, 0);

	fRotationAngle += sof(1.0f);
	
	glfwSwapBuffers(window);
}

void releaseScene()
{
	glDeleteBuffers(1, &uiVBOHeightmapData);
	glDeleteBuffers(1, &uiVBOIndices);
	glDeleteVertexArrays(1, &uiVAOHeightmap);
	glDeleteProgram(programID);
}

void frameBufferSizeChanged(GLFWwindow* window, int width, int height) {
	if (width != 0 && height != 0) {
		glm::mat4 projection = glm::perspective(45.0f, (float)width / height, 0.001f, 1000.0f);
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));
	}
}

void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
		bShowFPS = !bShowFPS;
		if (!bShowFPS) {
			glfwSetWindowTitle(window, title);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
	{
		bVerticalSync = !bVerticalSync;
		glfwSwapInterval(bVerticalSync ? 1 : 0);
	}
}


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
}

float sof(float fVal) {
	return fVal*delta;
}


/*
void logFPS() {
	clock_t tCurrent = clock();
	if ((tCurrent - tLastSecond) >= CLOCKS_PER_SEC)
	{
		tLastSecond += CLOCKS_PER_SEC;
		FPSCount = CurrentFPS;
		CurrentFPS = 0;
	}
	CurrentFPS++;
}

void updateDelta()
{
	clock_t tCur = clock();
	fFrameInterval = float(tCur - tLastFrame) / float(CLOCKS_PER_SEC);
	tLastFrame = tCur;
}

float sof(float fVal) {
	return fVal*fFrameInterval;
}*/
