#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <queue>
#include <vector>
#include <map>

#include "Shader.h"

class blueGuy;

struct vertex{
	float position[3] = {0.0f, 0.0f, 0.0f};
	float textureCoord[2] = {0.0f, 0.0f};
	float normal[3] = {0.0f, 0.0f, 0.0f};
	unsigned char id = 0;
	unsigned char lightLevel = 0;
};

struct face{
	vertex vertices[6];
};

class Chunkmesh{
	public:
		Chunkmesh();
		~Chunkmesh();

		void setShader(Shader *shader);
		
		void resetMesh();
		void setupMesh();
		void updateMesh();
		
		void pushFace(vertex * vertices, unsigned char id, unsigned char lightLevel, int index);
		void popFace(int index);

		int getIndex();

		void sort();

		void draw() const;
	
		std::vector<face> vertices;
	private:
		Shader *shader;
	
		std::queue<int> insertAt;
		unsigned int VAO, VBO;
		bool ready = false;
};

#endif
