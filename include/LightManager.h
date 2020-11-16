#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class World;
class Chunk;

class LightManager{
	public:
		LightManager(World *world);

		void lightChunk(Chunk *chunk);
	private:
		World *world;

		void findVisibleAir(Chunk *chunk);
		void assignLightValues(Chunk *chunk);
		void recursiveAssign(Chunk *chunk, int x, int y, int z, int lightValue);

		float distanceFactor;
		glm::vec3 position;
		glm::vec3 direction;

		std::vector<std::vector<int> > visibleAir;
};

#endif
