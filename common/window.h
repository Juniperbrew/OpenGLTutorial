#ifndef WINDOW_H
#define WINDOW_H

//typedef void(*vFunctionCall)(int args);

class OpenGLWindow {
public : 
	//int createAndShowWindow(void (*funct2)(int a));
	int createAndShowWindow();
	bool bVerticalSync;
	bool bShowFPS;
private :
	int test;
};

void initScene(), renderScene(), releaseScene();
void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
float sof(float fVal);

extern OpenGLWindow openGLWindow;
extern GLuint programID;

#endif