#ifndef WORLD_H
#define WORLD_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <queue>

class Game;
class Chunk;
class Face;
class Shader;
class TerrainGenerator;
class LightManager;
class Camera;

class blueGuy;

class World{
	public:
		World();
		~World();

		Game *game;
		glm::vec3 *sun;
		LightManager *lightManager;

		void init(Game *game, glm::vec3 *sun, Face *face, Shader *shader, Camera *camera, int modelLoc, std::string world, int seed, int renderDistance);

		bool isTransparent(int x, int y, int z) const;
		bool isTransparent(unsigned char id) const;

		unsigned char getFaceTexture(int dir, unsigned char id) const;

		int getChunkCoordinate(int val) const;
		int getBlockCoordinate(int val) const;

		void setChunkDirty(int x, int z);

		unsigned char getBlock(int x, int y, int z) const;
		void setBlock(int x, int y, int z, unsigned char id);

		void setLightLevel(int x, int y, int z, int lightLevel);
		int getLightLevel(int x, int y, int z) const;

		void reconstructFaceTracker(int x, int y, int z);

		void update();
		void draw();
	private:
		struct chunkPos{
			Chunk *chunk;
			int x;
			int z;
		};

		Face *face;
		Shader *shader;	
		TerrainGenerator *generator;
		Camera *camera;
		
		int seed;

		int cameraX, cameraZ;

		int modelLoc;
		
		std::vector<std::vector<Chunk*> > loadedChunks;
		std::queue<chunkPos> toLoad;
		std::queue<Chunk*> toMesh;
		int radius;

		std::string world;
};

#endif
