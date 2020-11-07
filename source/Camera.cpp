#include "Camera.h"
#include <iostream>
#include <iomanip>

void Camera::init(const glm::vec3 &pos, const int &viewLoc, float *deltaTime){
	this->viewLoc = viewLoc;
	
	view = glm::mat4(1.0f);

	position = glm::vec3(0.0f, 70.0f, 0.0f);
	tangent = glm::vec3(0.0f, 0.0f, -1.0f);
	normal = glm::vec3(0.0f, 1.0f, 0.0f);
	binormal = glm::vec3(-1.0f, 0.0f, 0.0f);
	
	view = glm::lookAt(position, position + tangent, normal);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	this->deltaTime = deltaTime;
	speed = 25.0f;

	pitch = 0.0f;
	yaw = 0.0f;
	setRot = false;
}

int Camera::getX() const {
	return position.x;
}

int Camera::getZ() const {
	return position.z;
}

void Camera::set(Direction dir){
	dirArr[dir] = !dirArr[dir];
}

void Camera::sprint(){
	fast = !fast;
}

void Camera::rotate(double x, double y){
	glm::vec2 pair = glm::vec2(x, y);
	pair = glm::normalize(pair);

	xOffset = x - lastX;
	yOffset = lastY - y;
	lastX = x;
	lastY = y;	
	
	float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;


	yaw += xOffset;
	pitch += yOffset;

	if(pitch > 89.0f) pitch = 89.0f;
	if(pitch < -89.0f) pitch = -89.0f;

	if(!setRot){
		yaw = -90.0f;
		pitch = 0.0f;
		setRot = true;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	tangent = glm::normalize(direction);
	binormal = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), tangent));
	normal = glm::normalize(glm::cross(tangent, binormal));

	view = glm::lookAt(position, position + tangent, normal);	
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::update(){
	if(!(dirArr[UP] || dirArr[DOWN] || dirArr[LEFT] || dirArr[RIGHT] || dirArr[FRONT] || dirArr[BACK])){
		return;
	}

	glm::vec3 direction = glm::vec3(0.0f);
	
	if(dirArr[FRONT]^dirArr[BACK])	direction.z = (float)(dirArr[FRONT] - dirArr[BACK]);	
	if(dirArr[UP]^dirArr[DOWN])	direction.y = (float)(dirArr[UP] - dirArr[DOWN]);
	if(dirArr[LEFT]^dirArr[RIGHT])	direction.x = (float)(dirArr[RIGHT] - dirArr[LEFT]);
	
	if(!glm::length2(direction)) return;

	direction = glm::normalize(direction);
	direction *= speed * *deltaTime;
	if(fast) direction *= 10;

	position -= direction.x*binormal;
	position += direction.y*normal;
	position += direction.z*tangent;

	//std::cout << std::setprecision(3) << position.x << ' ' << position.y << ' ' << position.z << '\n';

	view = glm::lookAt(position, position + tangent, normal);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}
