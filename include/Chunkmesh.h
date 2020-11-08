#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Shader.h"

struct vertex{
	char position[3];
	char textureCoord[2];
	unsigned char id;	
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
		
		void pushFace(vertex * vertices, unsigned char id);
		void sort();

		void draw() const;
	
		std::vector<face> vertices;
	private:
		Shader *shader;
		
		unsigned int VAO, VBO;
		bool ready = false;
};

#endif
