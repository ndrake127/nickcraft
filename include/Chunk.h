#ifndef CHUNK_H
#define CHUNK_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.h"
#include "Face.h"
#include "Chunkmesh.h"

#include <sstream>
#include <fstream>
#include <string>

const unsigned int WIDTH = 16;
const unsigned int LENGTH = 16;
const unsigned int HEIGHT = 256;

class World;

class Chunk{
	public:
		Chunk();
		Chunk(int x, int z);

		void init(Face *face, Shader *shader, World *world, int modelLoc);
		
		void setChunkPos(int x, int z);
		const int & getX() const;
		const int & getZ() const;
		unsigned char getBlock(int x, int y, int z) const;

		void generate();
		void load(std::string path);
		void save();

		void calculateVisible();
		void makeChunkmesh();

		void draw();
	private:
		std::string path;
		bool dirty = true;

		unsigned short list[WIDTH][HEIGHT][LENGTH];	
		int x, z;
		int blocks;

		glm::mat4 model = glm::mat4(1.0f);
		int modelLoc;
		
		Chunkmesh mesh;

		Face *face;
		Shader *shader;
		World *world;
};

#endif
