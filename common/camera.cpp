#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(glm::vec3 eye, glm::vec3 target){
		m_position = eye;
		m_direction = target - m_position;
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);
		view = glm::lookAt(m_position, m_position + m_direction, m_up);
    }
	void Camera::moveLeft(float amount){
		m_position -= amount * glm::cross(m_direction, m_up);
	}
	void Camera::moveRight(float amount){
		m_position += amount * glm::cross(m_direction, m_up);
	}
	void Camera::moveForward(float amount){
		m_position += amount * m_direction;
	}
	void Camera::moveBackward(float amount){
		m_position -= amount * m_direction;
	}
	void Camera::startMouselook(float mouseX, float mouseY){
        lastMousePosition.x = mouseX;
        lastMousePosition.y = mouseY;
    }
	void Camera::updateMouselook(float mouseX, float mouseY){
		float deltaY = lastMousePosition.y - mouseY;
		float deltaX = lastMousePosition.x - mouseX;
		lastMousePosition.x = mouseX;
		lastMousePosition.y = mouseY;
		m_direction = glm::rotate(m_direction, deltaY*0.003f, glm::cross(m_direction,m_up));
		m_direction = glm::rotate(m_direction, deltaX*0.003f, m_up);
	}
	glm::mat4 Camera::getView(){
		view = glm::lookAt(m_position, m_position + m_direction, m_up);
		return view;
	}