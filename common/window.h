#ifndef WINDOW_H
#define WINDOW_H

//typedef void(*vFunctionCall)(int args);

class OpenGLWindow {
public : 
	//int createAndShowWindow(void (*funct2)(int a));
	int createAndShowWindow();
    void disableCursor(bool mode);
    int getKey(int key);
    double getDelta();
    void setVsync(bool enable);
    bool getVsync();
	bool bShowFPS;
private :
    bool vsync;
};

void initScene(), renderScene(), releaseScene(), doLogic();
void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMoved(GLFWwindow* window, double xpos, double ypos);
void mouseClicked(GLFWwindow* window, int button, int action, int mods);
float sof(float fVal);

extern OpenGLWindow openGLWindow;
extern GLuint programID;

#endif