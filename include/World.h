#ifndef WORLD_H
#define WORLD_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.h"
#include "Mesh.h"

class World{
	public:
		void load(ResourceManager *manager, Mesh *mesh, int modelLoc);
		void draw() const;
	
		int modelLoc;
	private:
		char list[16][16][16];

		ResourceManager *manager;
		Mesh *mesh;

		void drawBlock(char id) const;
};

#endif
