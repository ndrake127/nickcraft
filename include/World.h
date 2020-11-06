#ifndef WORLD_H
#define WORLD_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Chunk;
class Face;
class Shader;

class World{
	public:
		World();

		void init(Face *face, Shader *shader, int modelLoc);

		unsigned char getBlock(int x, int y, int z) const;

		void draw();
	private:
		Face *face;
		Shader *shader;
		int modelLoc;
		
		std::vector<std::vector<Chunk> > loadedChunks;
		int radius;
};

#endif
