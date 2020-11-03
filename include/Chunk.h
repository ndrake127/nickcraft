#ifndef CHUNK_H
#define CHUNK_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.h"
#include "Face.h"
#include "Chunkmesh.h"

const unsigned int WIDTH = 16;
const unsigned int LENGTH = 16;
const unsigned int HEIGHT = 256;

class Chunk{
	public:
		void load(ResourceManager *manager, Face *face, Shader *shader, int modelLoc);
		void translate(glm::vec3 position);

		void calculateVisible();
		void makeChunkmesh();

		void draw();
	private:
		unsigned short list[WIDTH][HEIGHT][LENGTH];
		glm::mat4 model = glm::mat4(1.0f);
		int modelLoc;
		Chunkmesh mesh;

		ResourceManager *manager;
		Face *face;
		Shader *shader;
};

#endif
