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
float sof(float fVal);
void updateTimer();
void frameBufferSizeChanged(GLFWwindow* window, int width, int height);

// Used for FPS calculation
int iFPSCount, iCurrentFPS;
clock_t tLastSecond;

float fPyramid[36]; // Pyramid data - 4 triangles of 3 vertices of 3 floats
float fPyramidColor[36]; // Same for color

GLuint uiVBO[2]; // One VBO for vertices positions, one for colors
GLuint uiVAO[1]; // One VAO for pyramid

GLuint programID;

bool bShowFPS = false;
bool bVerticalSync = true;

glm::mat4 mProjection = glm::mat4();
float fRotationAngle = 0.0f;
const float PIover180 = 3.1415f / 180.0f;
float fFrameInterval = 0;
clock_t tLastFrame;

const char *title = "04.) Going 3D With Transformations - Tutorial by Michal Bubnar (www.mbsoftworks.sk)";


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

		updateTimer();
		renderScene();
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteVertexArrays(2, uiVAO);
	glDeleteBuffers(4, uiVBO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void initScene() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	mProjection = glm::perspective(45.0f, (float)width / height, 0.001f, 1000.0f);

	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

	// Setup pyramid

	// Front face
	fPyramid[0] = 0.0f; fPyramid[1] = 5.0f; fPyramid[2] = 0.0f;
	fPyramid[3] = -3.0f; fPyramid[4] = 0.0f; fPyramid[5] = 3.0f;
	fPyramid[6] = 3.0f; fPyramid[7] = 0.0f; fPyramid[8] = 3.0f;

	// Back face
	fPyramid[9] = 0.0f; fPyramid[10] = 5.0f; fPyramid[11] = 0.0f;
	fPyramid[12] = 3.0f; fPyramid[13] = 0.0f; fPyramid[14] = -3.0f;
	fPyramid[15] = -3.0f; fPyramid[16] = 0.0f; fPyramid[17] = -3.0f;

	// Left face
	fPyramid[18] = 0.0f; fPyramid[19] = 5.0f; fPyramid[20] = 0.0f;
	fPyramid[21] = -3.0f; fPyramid[22] = 0.0f; fPyramid[23] = -3.0f;
	fPyramid[24] = -3.0f; fPyramid[25] = 0.0f; fPyramid[26] = 3.0f;

	// Right face
	fPyramid[27] = 0.0f; fPyramid[28] = 5.0f; fPyramid[29] = 0.0f;
	fPyramid[30] = 3.0f; fPyramid[31] = 0.0f; fPyramid[32] = 3.0f;
	fPyramid[33] = 3.0f; fPyramid[34] = 0.0f; fPyramid[35] = -3.0f;

	// Setup pyramid color

	for (int i = 0; i < 4;i++)
	{
		fPyramidColor[i * 9] = 1.0f; fPyramidColor[i * 9 + 1] = 0.0f; fPyramidColor[i * 9 + 2] = 0.0f;
		if (i < 2)
		{
			fPyramidColor[i * 9 + 1] = 0.0f; fPyramidColor[i * 9 + 4] = 1.0f; fPyramidColor[i * 9 + 5] = 0.0f;
			fPyramidColor[i * 9 + 2] = 0.0f; fPyramidColor[i * 9 + 7] = 0.0f; fPyramidColor[i * 9 + 8] = 1.0f;
		}
		else
		{
			fPyramidColor[i * 9 + 2] = 0.0f; fPyramidColor[i * 9 + 7] = 1.0f; fPyramidColor[i * 9 + 8] = 0.0f;
			fPyramidColor[i * 9 + 1] = 0.0f; fPyramidColor[i * 9 + 4] = 0.0f; fPyramidColor[i * 9 + 5] = 1.0f;
		}
	}

	glGenVertexArrays(1, uiVAO);
	glGenBuffers(2, uiVBO);

	// Setup whole pyramid
	glBindVertexArray(uiVAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), fPyramid, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), fPyramidColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Use our shader
	glUseProgram(programID);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
}

void renderScene() {

	clock_t tCurrent = clock();
	if ((tCurrent - tLastSecond) >= CLOCKS_PER_SEC)
	{
		tLastSecond += CLOCKS_PER_SEC;
		iFPSCount = iCurrentFPS;
		iCurrentFPS = 0;
	}
	iCurrentFPS++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(uiVAO[0]);

	int iModelViewLoc = glGetUniformLocation(programID, "modelViewMatrix");
	int iProjectionLoc = glGetUniformLocation(programID, "projectionMatrix");
	glUniformMatrix4fv(iProjectionLoc, 1, GL_FALSE, glm::value_ptr(mProjection));

	glm::mat4 mModelView = glm::lookAt(glm::vec3(0, 15, 40), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Render rotating pyramid in the middle

	glm::mat4 mCurrent = glm::rotate(mModelView, fRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// Render translating pyramids
	
	// One on the left
	mCurrent = glm::translate(mModelView, glm::vec3(-20.0f, 10.0f*float(sin(fRotationAngle*10.0f*PIover180)), 0.0f));
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// One on the right
	mCurrent = glm::translate(mModelView, glm::vec3(20.0f, -10.0f*float(sin(fRotationAngle*10.0f*PIover180)), 0.0f));
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// And one translating and rotating on top

	mCurrent = glm::translate(mModelView, glm::vec3(20.0f*float(sin(fRotationAngle*PIover180)), 10.0f, 0.0f));
	mCurrent = glm::rotate(mCurrent, fRotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// And lastly - render scaling pyramid that rotates

	float fScaleValue = 1.5f + float(sin(fRotationAngle*PIover180))*0.5f;
	mCurrent = glm::translate(mModelView, glm::vec3(0.0f, -10.0f, 0.0f));
	mCurrent = glm::scale(mCurrent, glm::vec3(fScaleValue, fScaleValue, fScaleValue));
	mCurrent = glm::rotate(mCurrent, fRotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	mCurrent = glm::rotate(mCurrent, fRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	mCurrent = glm::rotate(mCurrent, fRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_TRIANGLES, 0, 12);

	fRotationAngle += sof(2.0f);

	// Add some handlers - with F2, you can toggle FPS showing, with F3 you can toggle V-Sync

	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
	{
		bShowFPS = !bShowFPS;
		if (!bShowFPS)glfwSetWindowTitle(window, title);
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
	{
		bVerticalSync = !bVerticalSync;
		glfwSwapInterval(bVerticalSync ? 1 : 0);
	}
	
	if (bShowFPS)
	{
		char buf[55]; sprintf(buf, "FPS: %d, V-Sync: %s", iFPSCount, bVerticalSync ? "On" : "Off");
		glfwSetWindowTitle(window, buf);
	}
	glfwSwapBuffers(window);
}

float sof(float fVal) {
	return fVal*fFrameInterval;
}

void updateTimer()
{
	clock_t tCur = clock();
	fFrameInterval = float(tCur - tLastFrame) / float(CLOCKS_PER_SEC);
	tLastFrame = tCur;
}

void frameBufferSizeChanged(GLFWwindow* window, int width, int height) {
	mProjection = glm::perspective(45.0f, (float)width / height, 0.001f, 1000.0f);
}