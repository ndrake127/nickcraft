#include "World.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

void World::load(ResourceManager *manager, Mesh *mesh, int modelLoc){
	this->manager = manager;
	this->mesh = mesh;
	this->modelLoc = modelLoc;

	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 16; j++){
			for(int k = 0; k < 16; k++){
				list[i][j][k] = j/4;
				if(j < 12) list[i][j][k] = 2;
				else if(j >= 12 && j < 15) list[i][j][k] = 1;
				else list[i][j][k] = 0;
			}
		}
	}
}

void World::draw() const {
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 16; j++){
			for(int k = 0; k < 16; k++){
				glm::mat4 model(1.0f);
				model = glm::translate(model, glm::vec3((float)i, (float)j, (float)k));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				
				drawBlock(list[i][j][k]);
			}
		}
	}
}

void World::drawBlock(char id) const {
	glBindTexture(GL_TEXTURE_2D, manager->getTexture(id));
	mesh->draw();
}
