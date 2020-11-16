#ifndef CHUNK_H
#define CHUNK_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.h"
#include "Face.h"
#include "Chunkmesh.h"

#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

const unsigned int WIDTH = 16;
const unsigned int LENGTH = 16;
const unsigned int HEIGHT = 256;

class World;
class TerrainGenerator;
class Chunk;

class faceTracker{
	public:
		faceTracker(Chunk *chunk, int x, int y, int z);

		void popFaces();
			
		Chunk *chunk = nullptr;

		int opaqueFaces[6];
		int transparentFaces[6];
};

class Chunk{
	friend class faceTracker;
	friend class World;

	private:
		struct block{
			unsigned char id = 0;
			unsigned char visibleFaces = 0;
			unsigned char lightLevel = 0;
		};

	public:
		Chunk();
		Chunk(int x, int z, Face *face, Shader *shader, World *world, int modelLoc, std::string worldpath);
		~Chunk();

		void setChunkPos(int x, int z);
		const int & getX() const;
		const int & getZ() const;

		std::vector<unsigned char> getAdjacentBlockTypes(int x, int y, int z) const;
		std::vector<std::vector<int> > getAdjacentBlockCoordinates(int x, int y, int z) const;
		
		std::vector<int> getAdjacentLightLevels(int x, int y, int z) const;

		unsigned char getBlock(int x, int y, int z) const;
		void setBlock(int x, int y, int z, unsigned char id);

		bool isTransparent(int x, int y, int z) const;
		bool isTransparent(unsigned char id) const;

		void setLightLevel(int x, int y, int z, int lightLevel);
		int getLightLevel(int x, int y, int z) const;

		void generate(TerrainGenerator *generator);
		bool load();
		void save();

		void calculateVisible();
		void makeChunkmesh();
		void updateChunkmesh(int x, int y, int z);

		void draw(bool drawTransparent);
	
		block list[WIDTH][HEIGHT][LENGTH];	
		int x, z;

		bool loaded = false;
		bool meshed = false;
		bool dirty = true;
		
	private:
		std::string path;

		glm::mat4 model = glm::mat4(1.0f);
		int modelLoc;		

		std::unordered_map<unsigned short, faceTracker> faceTrackers;
		void reconstructFaceTracker(int x, int y, int z);

		Chunkmesh opaqueMesh;
		Chunkmesh transparentMesh;
		
		World *world;
		Face *face;
		Shader *shader;
};

#endif
