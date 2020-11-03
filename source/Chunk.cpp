#include "Chunk.h"
#include <iostream>

void Chunk::load(ResourceManager *manager, Face *face, Shader *shader, int modelLoc){
	this->manager = manager;
	this->face = face;
	this->shader = shader;
	this->modelLoc = modelLoc;

	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				list[x][y][z] = 0;
				if(y < 61){
					list[x][y][z] = 3;
				}else if(y >= 61 && y < 64){
					list[x][y][z] = 2;
				}else if(y == 64){
					list[x][y][z] = 1;
				}else{
					list[x][y][z] = 0;
				}
			}
		}
	}

	for(int x = 3; x < 6; x++){
		for(int y = 3; y < 6; y++){
			for(int z = 0; z < LENGTH; z++){
				list[x][y][z] = 0;
			}
		}
	}
	for(int x = 0; x < WIDTH; x++){
		for(int y = 3; y < 6; y++){
			for(int z = 3; z < 6; z++){
				list[x][y][z] = 0;
			}
		}
	}
	for(int x = 3; x < 6; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 3; z < 6; z++){
				list[x][y][z] = 0;
			}
		}
	}

	calculateVisible();
	makeChunkmesh();
	mesh.setupMesh();
}

void Chunk::translate(glm::vec3 position){
	model = glm::translate(model, position);
}

void Chunk::calculateVisible(){
	for(int x = 1; x < WIDTH-1; x++){
		for(int y = 1; y < HEIGHT-1; y++){
			for(int z = 1; z < LENGTH-1; z++){
				if(!(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
				if(!(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
				if(!(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
				if(!(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
				if(!(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
				if(!(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;
			}
		}
	}
	for(int x = 0; x < WIDTH; x += WIDTH-1){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){	
				if(x+1 >= WIDTH){
					list[x][y][z] |= 512;
					if(!(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
				}else if(x-1 < 0){
					list[x][y][z] |= 256;
					if(!(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
				}else{
					if(!(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
					if(!(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
				}
				
				if(y+1 >= HEIGHT){
					list[x][y][z] |= 2048;
					if(!(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
					
				}else if(y-1 < 0){
					list[x][y][z] |= 1024;
					if(!(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
				}else{
					if(!(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
					if(!(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
				}

				if(z+1 >= LENGTH){
					list[x][y][z] |= 8192;
					if(!(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
				}else if(z-1 < 0){
					list[x][y][z] |= 4096;	
					if(!(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;
				}else{
					if(!(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
					if(!(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;
				}
			}
		}
	}
	for(int x = 1; x < WIDTH-1; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z += LENGTH-1){	
				if(x+1 >= WIDTH){
					list[x][y][z] |= 512;
					if(!(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
				}else if(x-1 < 0){
					list[x][y][z] |= 256;
					if(!(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
				}else{
					if(!(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
					if(!(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
				}
				
				if(y+1 >= HEIGHT){
					list[x][y][z] |= 2048;
					if(!(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
					
				}else if(y-1 < 0){
					list[x][y][z] |= 1024;
					if(!(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
				}else{
					if(!(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
					if(!(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
				}

				if(z+1 >= LENGTH){
					list[x][y][z] |= 8192;
					if(!(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
				}else if(z-1 < 0){
					list[x][y][z] |= 4096;	
					if(!(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;
				}else{
					if(!(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
					if(!(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;
				}
			}
		}
	}
	for(int x = 1; x < WIDTH-1; x++){
		for(int y = 0; y < HEIGHT; y += HEIGHT-1){
			for(int z = 1; z < LENGTH-1; z++){	
				if(y+1 >= HEIGHT){
					list[x][y][z] |= 2048;
					if(!(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
					
				}else if(y-1 < 0){
					list[x][y][z] |= 1024;
					if(!(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
				}else{
					if(!(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
					if(!(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;	
				}
				if(!(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
				if(!(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
				if(!(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
				if(!(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;
			}
		}
	}
}

void Chunk::makeChunkmesh(){
	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if((list[x][y][z] & 255) == 0) continue;	
				
				glm::vec3 position = glm::vec3((float)x, (float)y, (float)z);

				vertex temp[6];

				if(list[x][y][z] & 256){
					face->getVertices(WEST, position, temp);
					mesh.pushFace(temp, (unsigned char)manager->getTexture(list[x][y][z]&255));
				}
				if(list[x][y][z] & 512){
					face->getVertices(EAST, position, temp);
					mesh.pushFace(temp, (unsigned char)manager->getTexture(list[x][y][z]&255));
				}
				if(list[x][y][z] & 1024){
					face->getVertices(BLOCKUP, position, temp);
					mesh.pushFace(temp, (unsigned char)manager->getTexture(list[x][y][z]&255));
				}
				if(list[x][y][z] & 2048){
					face->getVertices(BLOCKDOWN, position, temp);
					mesh.pushFace(temp, (unsigned char)manager->getTexture(list[x][y][z]&255));
				}
				if(list[x][y][z] & 4096){
					face->getVertices(NORTH, position, temp);
					mesh.pushFace(temp, (unsigned char)manager->getTexture(list[x][y][z]&255));
				}
				if(list[x][y][z] & 8192){
					face->getVertices(SOUTH, position, temp);
					mesh.pushFace(temp, (unsigned char)manager->getTexture(list[x][y][z]&255));
				}
			}
		}
	}
	mesh.sort();
}

void Chunk::draw(){
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	mesh.draw();
}
