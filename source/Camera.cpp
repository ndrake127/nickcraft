#include "Camera.h"
#include <GLFW/glfw3.h>

#include "World.h"

#include <iostream>
#include <iomanip>

Camera::Camera(){
	view = glm::mat4(1.0f);

	position = glm::vec3(0.0f, 80.0f, 0.0f);
	tangent = glm::vec3(0.0f, 0.0f, -1.0f);
	normal = glm::vec3(0.0f, 1.0f, 0.0f);
	binormal = glm::vec3(-1.0f, 0.0f, 0.0f);
	prev = position;
	
	fast = false;
	showPos = false;
	
	speed = 10.92f;
	sprintFactor = 2.0f;

	pitch = 0.0f;
	yaw = 0.0f;
	setRot = false;

	m_id = 32;
}

void Camera::init(const glm::vec3 &pos, const int &viewLoc, const int &positionLoc, float *deltaTime, World *world, bool showPos){
	this->world = world;
	this->viewLoc = viewLoc;
	this->positionLoc = positionLoc;
	this->showPos = showPos;
	
	view = glm::lookAt(position, position + tangent, normal);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	this->deltaTime = deltaTime;
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
	if(fast) direction *= sprintFactor;

	prev = position;

	position -= direction.x*binormal;
	position += direction.y*normal;
	position += direction.z*tangent;

	if(showPos) std::cout << "Camera Position: " << position.x << ' ' << position.y << ' ' << position.z << '\n';

	view = glm::lookAt(position, position + tangent, normal);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3f(positionLoc, position.x, position.y, position.z);
}

void Camera::setBlock(unsigned char id){
	m_id = id;
	std::cout << (int)m_id << '\n';
}

// casts ray into world, returns first block that intersects ray via the parameter
void Camera::rayCast(bool breaking) const {
	float distanceFactor = 0.0f;
	glm::vec3 blockPosition = position + tangent * distanceFactor;
	glm::vec3 prevBlockPosition = blockPosition;
	unsigned char id = world->getBlock(position.x, position.y, position.z);
	
	while(id == 0 && distanceFactor <= 30){
		prevBlockPosition = blockPosition;

		distanceFactor += 0.01f;
		blockPosition = position + tangent * distanceFactor;
		if(blockPosition.x < 0) blockPosition.x--;
		if(blockPosition.z < 0) blockPosition.z--;
		id = world->getBlock(blockPosition.x, blockPosition.y, blockPosition.z);
	}
	
	if(breaking) world->setBlock(blockPosition.x, blockPosition.y, blockPosition.z, 0);
	if(!breaking) world->setBlock(prevBlockPosition.x, prevBlockPosition.y, prevBlockPosition.z, m_id);
}
