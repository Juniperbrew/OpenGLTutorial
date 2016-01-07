#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
public : 
	Camera(glm::vec3 eye, glm::vec3 target);
	void moveLeft(float amount);
	void moveRight(float amount);
	void moveForward(float amount);
	void moveBackward(float amount);
    void startMouselook(float mouseX, float mouseY);
	void updateMouselook(float mouseX, float mouseY);
	glm::mat4 getView();
private :
	glm::vec3 m_up;
	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec2 lastMousePosition;
	glm::mat4 view;
};

#endif