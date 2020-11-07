#include "World.h"
#include "Chunk.h"
#include "Face.h"
#include "Shader.h"
#include "TerrainGenerator.h"
#include "Camera.h"

World::World(){
	face = nullptr;
	shader = nullptr;
	camera = nullptr;
	radius = 0;

	generator = new TerrainGenerator(this);
}

World::~World(){
	delete generator;
}

void World::init(Face *face, Shader *shader, Camera *camera, int modelLoc){
	this->face = face;
	this->shader = shader;
	this->modelLoc = modelLoc;
	this->camera = camera;
	cameraX = calculateChunkCoordinate(camera->getX());
	cameraZ = calculateChunkCoordinate(camera->getZ());

	radius = 10;
	loadedChunks.resize(2*radius-1, std::vector<Chunk*>(2*radius-1));

	for(int x = cameraX; x < 2*radius-1+cameraX; x++){
		for(int z = cameraZ; z < 2*radius-1+cameraZ; z++){
			loadedChunks[x-cameraX][z-cameraZ] = new Chunk(x-radius+1, z-radius+1, face, shader, this, glGetUniformLocation(shader->getID(), "model"));
		}
	}

	for(unsigned int i = 0; i < loadedChunks.size(); i++){
		for(unsigned int j = 0; j < loadedChunks[0].size(); j++){
			if(!loadedChunks[i][j]->load()){
				loadedChunks[i][j]->generate(generator);
			}
		}
	}

	for(unsigned int i = 1; i < loadedChunks.size()-1; i++){
		for(unsigned int j = 1; j < loadedChunks[0].size()-1; j++){
			loadedChunks[i][j]->makeChunkmesh();
		}
	}
}

void World::draw(){
	for(unsigned int i = 1; i < loadedChunks.size()-1; i++){
		for(unsigned int j = 1; j < loadedChunks[0].size()-1; j++){
			loadedChunks[i][j]->draw();
		}
	}
}

void World::update(){
	int currCameraX = calculateChunkCoordinate(camera->getX());
	int currCameraZ = calculateChunkCoordinate(camera->getZ());

	if(cameraX == currCameraX && cameraZ == currCameraZ) return;

	if(cameraX != currCameraX){
		if(currCameraX > cameraX){
			for(int i = 0; i < loadedChunks.size(); i++){
				delete loadedChunks[0][i];
				
				for(int j = 1; j < loadedChunks.size(); j++){
					loadedChunks[j-1][i] = loadedChunks[j][i];
				}
				
				loadedChunks[loadedChunks.size()-1][i] = nullptr;
			}
		}else{
			for(int i = 0; i < loadedChunks.size(); i++){
				delete loadedChunks[loadedChunks.size()-1][i];

				for(int j = loadedChunks.size()-2; j >= 0; j--){
					loadedChunks[j+1][i] = loadedChunks[j][i];
				}
				
				loadedChunks[0][i] = nullptr;
			}
		}
		cameraX = currCameraX;
	}
	
	if(cameraZ != currCameraZ){
		if(currCameraZ > cameraZ){
			for(int i = 0; i < loadedChunks[0].size(); i++){
				delete loadedChunks[i][0];
				
				for(int j = 1; j < loadedChunks[0].size(); j++){
					loadedChunks[i][j-1] = loadedChunks[i][j];
				}
				
				loadedChunks[i][loadedChunks[0].size()-1] = nullptr;
			}
		}else{
			for(int i = 0; i < loadedChunks[0].size(); i++){
				delete loadedChunks[i][loadedChunks[0].size()-1];

				for(int j = loadedChunks[0].size()-2; j >= 0; j--){
					loadedChunks[i][j+1] = loadedChunks[i][j];
				}
				
				loadedChunks[i][0] = nullptr;
			}
		}
		cameraZ = currCameraZ;
	}	

	for(int x = cameraX; x < 2*radius-1+cameraX; x++){
		for(int z = cameraZ; z < 2*radius-1+cameraZ; z++){
			if(loadedChunks[x-cameraX][z-cameraZ] != nullptr) continue;
			loadedChunks[x-cameraX][z-cameraZ] = new Chunk(x-radius+1, z-radius+1, face, shader, this, glGetUniformLocation(shader->getID(), "model"));
		}
	}

	for(unsigned int i = 0; i < loadedChunks.size(); i++){
		for(unsigned int j = 0; j < loadedChunks[0].size(); j++){
			if(!loadedChunks[i][j]->load()){
				loadedChunks[i][j]->generate(generator);
			}
		}
	}

	for(unsigned int i = 1; i < loadedChunks.size()-1; i++){
		for(unsigned int j = 1; j < loadedChunks[0].size()-1; j++){
			loadedChunks[i][j]->makeChunkmesh();
		}
	}

}

int World::calculateChunkCoordinate(int coord) const {
	if(coord < 0) return (coord+1)/16-1;
	else return coord/16;
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

	int xIndex = chunkX - loadedChunks[0][0]->getX();
	int zIndex = chunkZ - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return 0;

	return loadedChunks[xIndex][zIndex]->getBlock(blockX, blockY, blockZ);
}

void World::setBlock(int x, int y, int z, unsigned char id){
	int chunkX;
	int chunkZ;
	int blockX;
	int blockY = y;
	int blockZ;
	
	if(x < 0){
		chunkX = (x+1)/16-1;
		blockX = 16-z%16;
	}else{
		chunkX = x/16;
		blockX = x%16;
	}
	if(z < 0){
		chunkZ = (z+1)/16-1;
		blockZ = 16-z%16;
	}else{
		chunkZ = z/16;
		blockZ = z%16;
	}

	blockX = x - (16*chunkX);
	blockZ = z - (16*chunkZ);

	int xIndex = chunkX - loadedChunks[0][0]->getX();
	int zIndex = chunkZ - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return;

	loadedChunks[xIndex][zIndex]->setBlock(blockX, blockY, blockZ, id);
}
