#ifndef CHUNK_H
#define CHUNK_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.h"
#include "Face.h"

#include <sstream>
#include <fstream>
#include <string>

const unsigned int WIDTH = 16;
const unsigned int LENGTH = 16;
const unsigned int HEIGHT = 128;

class World;
class TerrainGenerator;
class Chunkmesh;

class Chunk{
	public:
		Chunk();
		Chunk(int x, int z, Face *face, Shader *shader, World *world, int modelLoc);
		~Chunk();

		void setChunkPos(int x, int z);
		const int & getX() const;
		const int & getZ() const;
		
		unsigned char getBlock(int x, int y, int z) const;
		void setBlock(int x, int y, int z, unsigned char id);

		void generate(TerrainGenerator *generator);
		bool load();
		void save();

		void calculateVisible();
		void makeChunkmesh();

		void draw();
	
		unsigned short list[WIDTH][HEIGHT][LENGTH];	
		int x, z;

		bool loaded = false;
		bool dirty = true;
	private:
		std::string path;

		int blocks;

		glm::mat4 model = glm::mat4(1.0f);
		int modelLoc;
		
		Chunkmesh *mesh;

		Face *face;
		Shader *shader;
		World *world;
};

#endif
