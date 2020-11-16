#include "World.h"
#include "Game.h"
#include "Chunk.h"
#include "Face.h"
#include "Shader.h"
#include "TerrainGenerator.h"
#include "LightManager.h"
#include "Camera.h"

World::World(){
	face = nullptr;
	shader = nullptr;
	camera = nullptr;
	world = "";
	radius = 0;

	generator = new TerrainGenerator(this);
	lightManager = new LightManager(this);
}

World::~World(){
	delete generator;
	delete lightManager;
}

void World::init(Game *game, glm::vec3 *sun, Face *face, Shader *shader, Camera *camera, int modelLoc, std::string world, int seed, int renderDistance){
	this->game = game;
	this->face = face;
	this->shader = shader;
	this->modelLoc = modelLoc;
	this->camera = camera;
	cameraX = getChunkCoordinate(camera->getX());
	cameraZ = getChunkCoordinate(camera->getZ());
	this->world = world;
	this->sun = sun;

	this->seed = seed;
	generator->SetSeed(this->seed);

	radius = renderDistance;
	loadedChunks.resize(2*radius-1, std::vector<Chunk*>(2*radius-1));

	for(int x = cameraX; x < 2*radius-1+cameraX; x++){
		for(int z = cameraZ; z < 2*radius-1+cameraZ; z++){
			loadedChunks[x-cameraX][z-cameraZ] = new Chunk(x-radius+1, z-radius+1, face, shader, this, glGetUniformLocation(shader->getID(), "model"), world);
		}
	}

	for(unsigned int i = 0; i < loadedChunks.size(); i++){
		for(unsigned int j = 0; j < loadedChunks[0].size(); j++){
			if(!loadedChunks[i][j]->load()){
				loadedChunks[i][j]->generate(generator);
			}
		}
	}

	// once all chunks are generated, we can light and mesh them
	for(unsigned int i = 1; i < loadedChunks.size()-1; i++){
		for(unsigned int j = 1; j < loadedChunks[0].size()-1; j++){
			loadedChunks[i][j]->makeChunkmesh();
		}
	}
}

void World::draw(){
	// begin drawing opaque blocks away from player chunk
	loadedChunks[loadedChunks.size()/2][loadedChunks[0].size()/2]->draw(false);
	for(unsigned int i = 0; i < loadedChunks.size()/2; i++){
		for(unsigned int j = 0; j < loadedChunks[0].size()/2; j++){
			if(i == loadedChunks.size()/2 && j == loadedChunks[0].size()/2) continue;						//dont redraw players chunk

			loadedChunks[loadedChunks.size()/2+i][loadedChunks[0].size()/2+j]->draw(false);
			loadedChunks[loadedChunks.size()/2-i][loadedChunks[0].size()/2-j]->draw(false);
		}
	}
	for(unsigned int i = 1; i < loadedChunks.size()/2; i++){
		for(unsigned int j = 1; j < loadedChunks[0].size()/2; j++){
			loadedChunks[loadedChunks.size()/2-i][loadedChunks[0].size()/2+j]->draw(false);
			loadedChunks[loadedChunks.size()/2+i][loadedChunks[0].size()/2-j]->draw(false);
		}
	}

	
	// begin drawing transparent blocks away from player chunk
	loadedChunks[loadedChunks.size()/2][loadedChunks[0].size()/2]->draw(true);
	for(unsigned int i = 0; i < loadedChunks.size()/2; i++){
		for(unsigned int j = 0; j < loadedChunks[0].size()/2; j++){
			if(i == loadedChunks.size()/2 && j == loadedChunks[0].size()/2) continue;						//dont redraw players chunk

			loadedChunks[loadedChunks.size()/2+i][loadedChunks[0].size()/2+j]->draw(true);
			loadedChunks[loadedChunks.size()/2-i][loadedChunks[0].size()/2-j]->draw(true);
		}
	}
	for(unsigned int i = 1; i < loadedChunks.size()/2; i++){
		for(unsigned int j = 1; j < loadedChunks[0].size()/2; j++){
			loadedChunks[loadedChunks.size()/2-i][loadedChunks[0].size()/2+j]->draw(true);
			loadedChunks[loadedChunks.size()/2+i][loadedChunks[0].size()/2-j]->draw(true);
		}
	}
}

void World::update(){
	int currCameraX = getChunkCoordinate(camera->getX());
	int currCameraZ = getChunkCoordinate(camera->getZ());
	
	while(!toLoad.empty() && (toLoad.front().x-currCameraX > radius || toLoad.front().x-currCameraX < -1*radius || toLoad.front().z-currCameraZ > radius || toLoad.front().z-currCameraZ < -1*radius)){
		toLoad.pop();
	}

	if(!toLoad.empty()){
		if(!toLoad.front().chunk->load()){
			toLoad.front().chunk->generate(generator);
		}
		if(toLoad.front().chunk->loaded) toLoad.pop();
	}

	while(!toMesh.empty()){
		toMesh.front()->makeChunkmesh();
		toMesh.pop();
	}
	
	for(unsigned int i = 1; i < loadedChunks.size()-1; i++){
		for(unsigned int j = 1; j < loadedChunks[0].size()-1; j++){
			if(loadedChunks[i][j]->loaded && loadedChunks[i+1][j]->loaded && loadedChunks[i-1][j]->loaded && loadedChunks[i][j+1]->loaded && loadedChunks[i][j-1]->loaded){
				loadedChunks[i][j]->makeChunkmesh();
			}
		}
	}

	if(cameraX == currCameraX && cameraZ == currCameraZ) return;
	
	int chunkX;
	int chunkZ;

	if(cameraX != currCameraX){
		if(currCameraX > cameraX){
			for(int i = 0; i < loadedChunks.size(); i++){
				chunkX = loadedChunks[loadedChunks.size()-1][i]->getX();
				chunkZ = loadedChunks[loadedChunks.size()-1][i]->getZ();
			
				if(loadedChunks[0][i] != nullptr) delete loadedChunks[0][i];
				
				for(int j = 1; j < loadedChunks.size(); j++){
					loadedChunks[j-1][i] = loadedChunks[j][i];
				}
				
				loadedChunks[loadedChunks.size()-1][i] = new Chunk(chunkX+1, chunkZ, face, shader, this, glGetUniformLocation(shader->getID(), "model"), world);
				chunkPos temp;
				temp.chunk = loadedChunks[loadedChunks.size()-1][i];
				temp.x = temp.chunk->getX();
				temp.z = temp.chunk->getZ();
				toLoad.push(temp);
			}
		}else{
			for(int i = 0; i < loadedChunks.size(); i++){
				chunkX = loadedChunks[0][i]->getX();
				chunkZ = loadedChunks[0][i]->getZ();
				
				if(loadedChunks[loadedChunks.size()-1][i] != nullptr) delete loadedChunks[loadedChunks.size()-1][i];

				for(int j = loadedChunks.size()-2; j >= 0; j--){
					loadedChunks[j+1][i] = loadedChunks[j][i];
				}
				
				loadedChunks[0][i] = new Chunk(chunkX-1, chunkZ, face, shader, this, glGetUniformLocation(shader->getID(), "model"), world);
				chunkPos temp;
				temp.chunk = loadedChunks[0][i];
				temp.x = temp.chunk->getX();
				temp.z = temp.chunk->getZ();
				toLoad.push(temp);
			}
		}
		cameraX = currCameraX;
	}
	
	if(cameraZ != currCameraZ){
		if(currCameraZ > cameraZ){
			for(int i = 0; i < loadedChunks[0].size(); i++){
				chunkX = loadedChunks[i][loadedChunks[0].size()-1]->getX();
				chunkZ = loadedChunks[i][loadedChunks[0].size()-1]->getZ();
				
				if(loadedChunks[i][0] != nullptr) delete loadedChunks[i][0];
				
				for(int j = 1; j < loadedChunks[0].size(); j++){
					loadedChunks[i][j-1] = loadedChunks[i][j];
				}
				
				loadedChunks[i][loadedChunks[0].size()-1] = new Chunk(chunkX, chunkZ+1, face, shader, this, glGetUniformLocation(shader->getID(), "model"), world);
				chunkPos temp;
				temp.chunk = loadedChunks[i][loadedChunks[0].size()-1];
				temp.x = temp.chunk->getX();
				temp.z = temp.chunk->getZ();
				toLoad.push(temp);
			}
		}else{
			for(int i = 0; i < loadedChunks[0].size(); i++){
				chunkX = loadedChunks[i][0]->getX();
				chunkZ = loadedChunks[i][0]->getZ();
				
				if(loadedChunks[i][loadedChunks[0].size()-1] != nullptr) delete loadedChunks[i][loadedChunks[0].size()-1];

				for(int j = loadedChunks[0].size()-2; j >= 0; j--){
					loadedChunks[i][j+1] = loadedChunks[i][j];
				}
				
				loadedChunks[i][0] = new Chunk(chunkX, chunkZ-1, face, shader, this, glGetUniformLocation(shader->getID(), "model"), world);
				chunkPos temp;
				temp.chunk = loadedChunks[i][0];
				temp.x = temp.chunk->getX();
				temp.z = temp.chunk->getZ();
				toLoad.push(temp);
			}
		}
		cameraZ = currCameraZ;
	}	
}

bool World::isTransparent(int x, int y, int z) const {
	return game->blockdata[getBlock(x, y, z)].transparent; 
}

bool World::isTransparent(unsigned char id) const {
	return game->blockdata[id].transparent; 
}

unsigned char World::getFaceTexture(int dir, unsigned char id) const {
	return game->blockdata[id].faceTextureID[dir];
}

unsigned char World::getBlock(int x, int y, int z) const {
	int chunkX = getChunkCoordinate(x);
	int chunkZ = getChunkCoordinate(z);
	int blockX = getBlockCoordinate(x);
	int blockY = y;
	int blockZ = getBlockCoordinate(z);;
	
	blockX = x - (16*chunkX);
	blockZ = z - (16*chunkZ);

	int xIndex = chunkX - loadedChunks[0][0]->getX();
	int zIndex = chunkZ - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return 0;

	return loadedChunks[xIndex][zIndex]->list[blockX][blockY][blockZ].id;
}

void World::setBlock(int x, int y, int z, unsigned char id){
	int chunkX = getChunkCoordinate(x);
	int chunkZ = getChunkCoordinate(z);
	int blockX = getBlockCoordinate(x);
	int blockY = y;
	int blockZ = getBlockCoordinate(z);;
	
	blockX = x - (16*chunkX);
	blockZ = z - (16*chunkZ);

	int xIndex = chunkX - loadedChunks[0][0]->getX();
	int zIndex = chunkZ - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return;

	loadedChunks[xIndex][zIndex]->setBlock(blockX, blockY, blockZ, id);
}

void World::setChunkDirty(int x, int z){
	int xIndex = x - loadedChunks[0][0]->getX();
	int zIndex = z - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return;
	
	loadedChunks[xIndex][zIndex]->dirty = true;
	toMesh.push(loadedChunks[xIndex][zIndex]);
}

void World::setLightLevel(int x, int y, int z, int lightLevel){
	int chunkX = getChunkCoordinate(x);
	int chunkZ = getChunkCoordinate(z);
	int blockX = getBlockCoordinate(x);
	int blockY = y;
	int blockZ = getBlockCoordinate(z);;
	
	blockX = x - (16*chunkX);
	blockZ = z - (16*chunkZ);

	int xIndex = chunkX - loadedChunks[0][0]->getX();
	int zIndex = chunkZ - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return;

	loadedChunks[xIndex][zIndex]->list[blockX][blockY][blockZ].lightLevel = lightLevel;
}

int World::getLightLevel(int x, int y, int z) const {
	int chunkX = getChunkCoordinate(x);
	int chunkZ = getChunkCoordinate(z);
	int blockX = getBlockCoordinate(x);
	int blockY = y;
	int blockZ = getBlockCoordinate(z);;
	
	blockX = x - (16*chunkX);
	blockZ = z - (16*chunkZ);

	int xIndex = chunkX - loadedChunks[0][0]->getX();
	int zIndex = chunkZ - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return 0;

	return loadedChunks[xIndex][zIndex]->list[blockX][blockY][blockZ].lightLevel;
}

int World::getChunkCoordinate(int val) const {
	if(val < 0) return (val+1)/16-1;
	else        return val/16;
}

int World::getBlockCoordinate(int val) const {
	if(val < 0) return 16-val%16;
	else        return val%16;
}

void World::reconstructFaceTracker(int x, int y, int z){
	int chunkX = getChunkCoordinate(x);
	int chunkZ = getChunkCoordinate(z);
	int blockX = getBlockCoordinate(x);
	int blockY = y;
	int blockZ = getBlockCoordinate(z);;
	
	blockX = x - (16*chunkX);
	blockZ = z - (16*chunkZ);

	int xIndex = chunkX - loadedChunks[0][0]->getX();
	int zIndex = chunkZ - loadedChunks[0][0]->getZ();

	if(xIndex < 0 || zIndex < 0 || xIndex >= loadedChunks.size() || zIndex >= loadedChunks[0].size()) return;

	loadedChunks[xIndex][zIndex]->reconstructFaceTracker(blockX, blockY, blockZ);
	loadedChunks[xIndex][zIndex]->transparentMesh.updateMesh();
	loadedChunks[xIndex][zIndex]->opaqueMesh.updateMesh();
}
