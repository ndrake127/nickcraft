#include "World.h"
#include "Chunk.h"
#include "Face.h"
#include "Shader.h"

World::World(){
	face = nullptr;
	shader = nullptr;
	radius = 0;
}

void World::init(Face *face, Shader *shader, int modelLoc){
	this->face = face;
	this->shader = shader;
	this->modelLoc = modelLoc;
	radius = 20;
	loadedChunks.resize(2*radius-1, std::vector<Chunk>(2*radius-1));

	for(int x = 0; x < 2*radius-1; x++){
		for(int z = 0; z < 2*radius-1; z++){
			loadedChunks[x][z] = Chunk(x-radius+1, z-radius+1);
			loadedChunks[x][z].init(face, shader, this, glGetUniformLocation(shader->getID(), "model"));
			std::cout << x-radius+1 << ' ' << z-radius+1 << '\n';
		}
	}

	for(unsigned int i = 0; i < loadedChunks.size(); i++){
		for(unsigned int j = 0; j < loadedChunks.at(0).size(); j++){
			std::cout << i << ' ' << j << '\n';
			loadedChunks[i][j].makeChunkmesh();
		}
	}
}

void World::draw(){
	for(unsigned int i = 0; i < loadedChunks.size(); i++){
		for(unsigned int j = 0; j < loadedChunks.at(0).size(); j++){
			loadedChunks[i][j].draw();
		}
	}
}

unsigned char World::getBlock(int x, int y, int z) const {
	int chunkX;
	int chunkZ;
	int blockX;
	int blockY = y;
	int blockZ;
	
	if(x < 0){
		chunkX = (x+1)/16-1;
		blockX = 16-z%16;
	}
	if(z < 0){
		chunkZ = (z+1)/16-1;
		blockZ = 16-z%16;
	}
	if(x >= 0){
		chunkX = x/16;
		blockX = x%16;
	}
	if(z >= 0){
		chunkZ = z/16;
		blockZ = z%16;
	}
	blockX = x - (16*chunkX);
	blockZ = z - (16*chunkZ);

	int xIndex = chunkX - loadedChunks[0][0].getX();
	int zIndex = chunkZ - loadedChunks[0][0].getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return 0;

	return loadedChunks[xIndex][zIndex].getBlock(blockX, blockY, blockZ);
}
