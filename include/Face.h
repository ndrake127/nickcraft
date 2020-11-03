#ifndef FACE_H
#define FACE_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <queue>

#include "Chunkmesh.h"

enum faceDirection {SOUTH=0, EAST, NORTH, WEST, BLOCKUP, BLOCKDOWN};

class Face{
	public:
		void getVertices(faceDirection dir, const glm::vec3 &position, vertex returnArray[]);
	private:		
		char vertices[6][30] = 
		{
		{
				1, 1, 1, 1, 1,
				1, 0, 1, 1, 0,
				0, 0, 1, 0, 0,
				1, 1, 1, 1, 1,
				0, 0, 1, 0, 0,
				0, 1, 1, 0, 1
		},

		{
				1, 1, 0, 1, 1,
				1, 0, 0, 1, 0,
				1, 0, 1, 0, 0,
				1, 1, 0, 1, 1,
				1, 0, 1, 0, 0,
				1, 1, 1, 0, 1
		},

		{
				0, 1, 0, 1, 1,
				0, 0, 0, 1, 0,
				1, 0, 0, 0, 0,
				0, 1, 0, 1, 1,
				1, 0, 0, 0, 0,
				1, 1, 0, 0, 1
		},

		{
				0, 1, 1, 1, 1,
				0, 0, 1, 1, 0,
				0, 0, 0, 0, 0,
				0, 1, 1, 1, 1,
				0, 0, 0, 0, 0,
				0, 1, 0, 0, 1
		},

		{
				1, 0, 1, 1, 1,
				1, 0, 0, 1, 0,
				0, 0, 0, 0, 0,
				1, 0, 1, 1, 1,
				0, 0, 0, 0, 0,
				0, 0, 1, 0, 1
		},

		{
				1, 1, 0, 1, 1,
				1, 1, 1, 1, 0,
				0, 1, 1, 0, 0,
				1, 1, 0, 1, 1,
				0, 1, 1, 0, 0,
				0, 1, 0, 0, 1
		}
		};
};

#endif
