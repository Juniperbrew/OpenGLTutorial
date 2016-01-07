// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//using namespace glm;

#include <ctime>
#include <iostream>

#include "common/window.h"
#include "common/camera.h"

//Render time logging
double lastRenderDuration;

#define HM_SIZE_X 4 // Dimensions of our heightmap
#define HM_SIZE_Y 4

GLuint uiVBOHeightmapData; // Here are stored heightmap data (vertices)
GLuint uiVBOIndices; // And here indices for rendering heightmap

GLuint uiVAOHeightmap; // One VAO for heightmap

float fRotationAngle = 0.0f;
const float PIover180 = 3.1415f / 180.0f;

GLint uniModel;
GLint uniProj;
GLint uniView;

bool mouseLook;

Camera camera(glm::vec3(0, 60, 30), glm::vec3(0, 0, 0));

const char *title = "05.) Indexed Drawing - Tutorial by Michal Bubnar (www.mbsoftworks.sk)";

int main() {
	openGLWindow.createAndShowWindow();
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

	//Get uniforms
	uniModel = glGetUniformLocation(programID, "model");
	uniView = glGetUniformLocation(programID, "view");

	glm::mat4 view = glm::lookAt(glm::vec3(0, 60, 30), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
}

void renderScene() {

	double renderStart = glfwGetTime();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(uiVAOHeightmap);
    
    //Update camera
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getView()));

	glm::mat4 model = glm::mat4();
	model = glm::rotate(model, fRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(uiVAOHeightmap);
	glDrawElements(GL_TRIANGLE_STRIP, HM_SIZE_X*(HM_SIZE_Y - 1) * 2 + HM_SIZE_Y - 2, GL_UNSIGNED_INT, 0);

	fRotationAngle += sof(1.0f);
	
	lastRenderDuration = glfwGetTime() - renderStart;
}

void doLogic(){
    if (openGLWindow.getKey(GLFW_KEY_W) != GLFW_RELEASE) camera.moveForward(1.0f*openGLWindow.getDelta());
    if (openGLWindow.getKey(GLFW_KEY_S) != GLFW_RELEASE) camera.moveBackward(1.0f*openGLWindow.getDelta());
    if (openGLWindow.getKey(GLFW_KEY_A) != GLFW_RELEASE) camera.moveLeft(1.0f*openGLWindow.getDelta());
    if (openGLWindow.getKey(GLFW_KEY_D) != GLFW_RELEASE) camera.moveRight(1.0f*openGLWindow.getDelta());
}

void releaseScene()
{
	glDeleteBuffers(1, &uiVBOHeightmapData);
	glDeleteBuffers(1, &uiVBOIndices);
	glDeleteVertexArrays(1, &uiVAOHeightmap);
	glDeleteProgram(programID);
}

void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
		openGLWindow.bShowFPS = !openGLWindow.bShowFPS;
		if (!openGLWindow.bShowFPS) {
			glfwSetWindowTitle(window, title);
		}
	}

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
        openGLWindow.setVsync(!openGLWindow.getVsync());
	}

	if (key == GLFW_KEY_F4 && action == GLFW_PRESS) {
		std::cout << lastRenderDuration << "ms" << std::endl;
	}
}

void mouseMoved(GLFWwindow* window, double xpos, double ypos){
    if(mouseLook){
         camera.updateMouselook(xpos,ypos);
    }
}

void mouseClicked(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        mouseLook = !mouseLook;
        openGLWindow.disableCursor(mouseLook);
        if(mouseLook){
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            camera.startMouselook(xpos, ypos);
        }
    }
}