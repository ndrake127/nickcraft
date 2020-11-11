#ifndef CAMERA_H
#define CAMERA_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>

enum Direction { UP, DOWN, LEFT, RIGHT, FRONT, BACK };

class Camera{
	public:
		void init(const glm::vec3 &pos, const int &viewLoc, float *deltaTime);
	
		void set(Direction dir);
		void rotate(double x, double y);

		int getX() const;
		int getZ() const;

		void update();

		void sprint();
	private:
		glm::mat4 view;
		int viewLoc;
		
		bool dirArr[6] = {0,0,0,0,0,0};
		
		float *deltaTime;
		float speed, sprintFactor;
		bool fast = false;

		glm::vec3 tangent, normal, binormal, position, prev;

		float xOffset, yOffset, lastX, lastY;
		float pitch, yaw;
		bool setRot;
};

#endif
