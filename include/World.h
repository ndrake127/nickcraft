#ifndef WORLD_H
#define WORLD_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <queue>

class Chunk;
class Face;
class Shader;
class TerrainGenerator;
class Camera;

class World{
	public:
		World();
		~World();

		void init(Face *face, Shader *shader, Camera *camera, int modelLoc);

		unsigned char getBlock(int x, int y, int z) const;
		void setBlock(int x, int y, int z, unsigned char id);

		void update();
		void draw();
	private:
		Face *face;
		Shader *shader;
		TerrainGenerator *generator;
		
		Camera *camera;
		int cameraX, cameraZ;

		int modelLoc;
		
		std::vector<std::vector<Chunk*> > loadedChunks;
		int radius;

		std::queue<Chunk*> toLoad;

		int calculateChunkCoordinate(int coord) const;
};

#endif
