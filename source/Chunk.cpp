#include "Game.h"
#include "Chunk.h"
#include "World.h"
#include "TerrainGenerator.h"
#include "LightManager.h"

#include <algorithm>
#include <iostream>

Chunk::Chunk(){
	face = nullptr;
	shader = nullptr;
	world = nullptr;
	modelLoc = 0;

	x = 0;
	z = 0;
	setChunkPos(0, 0);
}

Chunk::Chunk(int x, int z, Face *face, Shader *shader, World *world, int modelLoc, std::string worldpath){
	this->face = face;
	this->shader = shader;
	opaqueMesh.setShader(shader);
	transparentMesh.setShader(shader);
	this->world = world;
	this->modelLoc = modelLoc;

	setChunkPos(x, z);

	path = worldpath + "/chunk" + std::to_string(x) + '-' + std::to_string(z);	
}

Chunk::~Chunk(){
}

void Chunk::generate(TerrainGenerator *generator){
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			for(int y = 0; y < HEIGHT; y++){
				list[x][y][z].id = 0;			// fills with air
			}
		}
	}

	generator->generateChunk(this);

	dirty = true;
	save();
	loaded = true;
}


void Chunk::save(){
	if(!dirty) return;
	
	std::ofstream file;
	file.open(path, std::ifstream::binary);	

	int mode = 0;

	if(file){
		if(mode == 0){
			char *buffer = new char[1+WIDTH*LENGTH*HEIGHT];
			
			int i = 0;
			buffer[i++] = 0;

			for(int x = 0; x < WIDTH; x++){
				for(int y = 0; y < HEIGHT; y++){
					for(int z = 0; z < LENGTH; z++){
						buffer[i++] = list[x][y][z].id;
					}
				}
			}

			file.write(buffer, 1+WIDTH*LENGTH*HEIGHT);
			delete [] buffer;
		}
	}

	file.close();
}

bool Chunk::load(){
	if(loaded) return true;

	std::ifstream file;
	file.open(path, std::ifstream::binary);

	if(file){
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);
			
		bool mode = 1;

		char *buffer = new char[length];
		file.read(buffer, length);

		int i = 0;
		mode = buffer[i++];

		if(mode == 0){
			for(int x = 0; x < WIDTH; x++){
				for(int y = 0; y < HEIGHT; y++){
					for(int z = 0; z < LENGTH; z++){
						list[x][y][z].id = buffer[i++];
					}
				}
			}
		}

		delete [] buffer;
		file.close();
	}else{
		return false;
		file.close();
	}

	loaded = true;	
	return true;
}

void Chunk::setChunkPos(int x, int z){
	this->x = x;
	this->z = z;
	model = glm::translate(model, glm::vec3((float)x*WIDTH, 0.0f, (float)z*LENGTH));
}

const int & Chunk::getX() const {
	return x;
}

const int & Chunk::getZ() const {
	return z;
}

bool Chunk::isTransparent(int x, int y, int z) const {
	return world->game->blockdata[getBlock(x, y, z)].transparent;
}

bool Chunk::isTransparent(unsigned char id) const {
	return world->game->blockdata[id].transparent;
}

int Chunk::getLightLevel(int x, int y, int z) const {
	if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && z >= 0 && z < LENGTH){
		return list[x][y][z].lightLevel;
	}else{
		return world->getLightLevel(16*this->x+x, y, 16*this->z+z);
	}
}

void Chunk::setLightLevel(int x, int y, int z, int lightLevel){
	if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && z >= 0 && z < LENGTH){
		list[x][y][z].lightLevel = lightLevel;
	}else{
		world->setLightLevel(16*this->x+x, y, 16*this->z+z, lightLevel);
	}
}

std::vector<unsigned char> Chunk::getAdjacentBlockTypes(int x, int y, int z) const {
	std::vector<unsigned char> blocks(6);
	
	blocks[0] = getBlock(x-1, y, z);
	blocks[1] = getBlock(x+1, y, z);
	blocks[2] = getBlock(x, y-1, z);
	blocks[3] = getBlock(x, y+1, z);
	blocks[4] = getBlock(x, y, z-1);
	blocks[5] = getBlock(x, y, z+1);

	return blocks;
}

std::vector<std::vector<int> > Chunk::getAdjacentBlockCoordinates(int x, int y, int z) const {
	std::vector<std::vector<int> > blockCoordinates(6, std::vector<int>(3, 0));
	
	blockCoordinates[0][0] = x-1;
	blockCoordinates[0][1] = y;
	blockCoordinates[0][2] = z;
	blockCoordinates[1][0] = x+1;
	blockCoordinates[1][1] = y;
	blockCoordinates[1][2] = z;
	blockCoordinates[2][0] = x;
	blockCoordinates[2][1] = y-1;
	blockCoordinates[2][2] = z;
	blockCoordinates[3][0] = x;
	blockCoordinates[3][1] = y+1;
	blockCoordinates[3][2] = z;
	blockCoordinates[4][0] = x;
	blockCoordinates[4][1] = y;
	blockCoordinates[4][2] = z-1;
	blockCoordinates[5][0] = x;
	blockCoordinates[5][1] = y;
	blockCoordinates[5][2] = z+1;

	return blockCoordinates;
}


std::vector<int> Chunk::getAdjacentLightLevels(int x, int y, int z) const {
	std::vector<int> light(6);

	light[0] = getLightLevel(x-1, y, z);
	light[1] = getLightLevel(x+1, y, z);
	light[2] = getLightLevel(x, y-1, z);
	light[3] = getLightLevel(x, y+1, z);
	light[4] = getLightLevel(x, y, z-1);
	light[5] = getLightLevel(x, y, z+1);

	return light;
}

unsigned char Chunk::getBlock(int x, int y, int z) const {
	if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && z >= 0 && z < LENGTH){
		return list[x][y][z].id;
	}else{
		return world->getBlock(16*this->x+x, y, 16*this->z+z);
	}
}

void Chunk::setBlock(int x, int y, int z, unsigned char id){
	list[x][y][z].id = id;
	dirty = true;
	updateChunkmesh(x, y, z);
}

void Chunk::calculateVisible(){
	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if(list[x][y][z].id == 0) continue;													// air is never visible, so skip if the block is air
			
				if(!(world->isTransparent(16*this->x+x, y, 16*this->z+z))){										// if the block being checked is water
					if(x > 0        && world->isTransparent(list[x-1][y][z].id)) list[x][y][z].visibleFaces |= 1;
					if(x < WIDTH-1  && world->isTransparent(list[x+1][y][z].id)) list[x][y][z].visibleFaces |= 2;
					if(y > 0        && world->isTransparent(list[x][y-1][z].id)) list[x][y][z].visibleFaces |= 4;
					if(y < HEIGHT-1 && world->isTransparent(list[x][y+1][z].id)) list[x][y][z].visibleFaces |= 8;
					if(z > 0        && world->isTransparent(list[x][y][z-1].id)) list[x][y][z].visibleFaces |= 16;
					if(z < LENGTH-1 && world->isTransparent(list[x][y][z+1].id)) list[x][y][z].visibleFaces |= 32;

					if(x == 0        && world->isTransparent(16*this->x+x-1, y,   16*this->z+z  )) list[x][y][z].visibleFaces |= 1;
					if(x == WIDTH-1  && world->isTransparent(16*this->x+x+1, y,   16*this->z+z  )) list[x][y][z].visibleFaces |= 2;
					if(y == 0        && world->isTransparent(16*this->x+x,   y-1, 16*this->z+z  )) list[x][y][z].visibleFaces |= 4;
					if(y == HEIGHT-1 && world->isTransparent(16*this->x+x,   y+1, 16*this->z+z  )) list[x][y][z].visibleFaces |= 8;
					if(z == 0        && world->isTransparent(16*this->x+x,   y,   16*this->z+z-1)) list[x][y][z].visibleFaces |= 16;
					if(z == LENGTH-1 && world->isTransparent(16*this->x+x,   y,   16*this->z+z+1)) list[x][y][z].visibleFaces |= 32;
				}else{														// if the block being checked is not water
					if(x > 0        && !list[x-1][y][z].id) list[x][y][z].visibleFaces |= 1;
					if(x < WIDTH-1  && !list[x+1][y][z].id) list[x][y][z].visibleFaces |= 2;
					if(y > 0        && !list[x][y-1][z].id) list[x][y][z].visibleFaces |= 4;
					if(y < HEIGHT-1 && !list[x][y+1][z].id) list[x][y][z].visibleFaces |= 8;
					if(z > 0        && !list[x][y][z-1].id) list[x][y][z].visibleFaces |= 16;
					if(z < LENGTH-1 && !list[x][y][z+1].id) list[x][y][z].visibleFaces |= 32;

					if(x == 0        && !world->getBlock(16*this->x+x-1, y,   16*this->z+z  )) list[x][y][z].visibleFaces |= 1;
					if(x == WIDTH-1  && !world->getBlock(16*this->x+x+1, y,   16*this->z+z  )) list[x][y][z].visibleFaces |= 2;
					if(y == 0        && !world->getBlock(16*this->x+x,   y-1, 16*this->z+z  )) list[x][y][z].visibleFaces |= 4;
					if(y == HEIGHT-1 && !world->getBlock(16*this->x+x,   y+1, 16*this->z+z  )) list[x][y][z].visibleFaces |= 8;
					if(z == 0        && !world->getBlock(16*this->x+x,   y,   16*this->z+z-1)) list[x][y][z].visibleFaces |= 16;
					if(z == LENGTH-1 && !world->getBlock(16*this->x+x,   y,   16*this->z+z+1)) list[x][y][z].visibleFaces |= 32;
				}
			}
		}
	}
}

/*
void Chunk::makeChunkmesh(){
	if(!dirty) return;

	calculateVisible();

	opaqueMesh.resetMesh();
	transparentMesh.resetMesh();
	world->lightManager->lightChunk(this);

	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if((list[x][y][z].id) == 0) continue;	
				
				glm::vec3 position = glm::vec3((float)x, (float)y, (float)z);
				vertex temp[6];
				int lightLevel = std::max({getLightLevel(x-1, y, z), getLightLevel(x+1, y, z), getLightLevel(x, y-1, z), getLightLevel(x, y+1, z), getLightLevel(x, y, z-1), getLightLevel(x, y, z+1)});

				if(world->isTransparent(16*this->x+x, y, 16*this->z+z) == 0){							// Non transparent blocks should be added to the opaque mesh
					if(list[x][y][z].visibleFaces & 1){
						face->getVertices(WEST, position, temp);
						opaqueMesh.pushFace(temp, world->getFaceTexture(WEST, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 2){
						face->getVertices(EAST, position, temp);
						opaqueMesh.pushFace(temp, world->getFaceTexture(EAST, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 4){
						face->getVertices(BLOCKDOWN, position, temp);
						opaqueMesh.pushFace(temp, world->getFaceTexture(BLOCKDOWN, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 8){
						face->getVertices(BLOCKUP, position, temp);
						opaqueMesh.pushFace(temp, world->getFaceTexture(BLOCKUP, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 16){
						face->getVertices(NORTH, position, temp);
						opaqueMesh.pushFace(temp, world->getFaceTexture(NORTH, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 32){
						face->getVertices(SOUTH, position, temp);
						opaqueMesh.pushFace(temp, world->getFaceTexture(SOUTH, (list[x][y][z].id)), lightLevel);
					}
				}else{														// Transparent blocks should be added to the transparent mesh
					if(list[x][y][z].visibleFaces & 1){
						face->getVertices(WEST, position, temp);
						transparentMesh.pushFace(temp, world->getFaceTexture(WEST, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 2){
						face->getVertices(EAST, position, temp);
						transparentMesh.pushFace(temp, world->getFaceTexture(EAST, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 4){
						face->getVertices(BLOCKDOWN, position, temp);
						transparentMesh.pushFace(temp, world->getFaceTexture(BLOCKDOWN, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 8){
						face->getVertices(BLOCKUP, position, temp);
						transparentMesh.pushFace(temp, world->getFaceTexture(BLOCKUP, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 16){
						face->getVertices(NORTH, position, temp);
						transparentMesh.pushFace(temp, world->getFaceTexture(NORTH, (list[x][y][z].id)), lightLevel);
					}
					if(list[x][y][z].visibleFaces & 32){
						face->getVertices(SOUTH, position, temp);
						transparentMesh.pushFace(temp, world->getFaceTexture(SOUTH, (list[x][y][z].id)), lightLevel);
					}
				}
			}
		}
	}

}
*/

void Chunk::makeChunkmesh(){
	if(!dirty) return;

	world->lightManager->lightChunk(this);
	
	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT-1; y++){
			for(int z = 0; z < LENGTH; z++){
				if(isTransparent(x, y, z)){
					std::vector<unsigned char> adjacentBlocks = getAdjacentBlockTypes(x, y, z);
					for(unsigned int i = 0; i < adjacentBlocks.size(); i++){
						if(adjacentBlocks[i] != getBlock(x, y, z)){
							faceTrackers.emplace(x << 12 | y << 4 | z << 0, faceTracker(this, x, y, z));
							break;
						}
					}					
				}else{
					continue;					//if a block is not air, then we move on
				}
			}
		}
	}
	
	opaqueMesh.setupMesh();
	transparentMesh.setupMesh();
	meshed = true;
	save();
	dirty = false;
}

void Chunk::updateChunkmesh(int x, int y, int z){	
	std::vector<int> position(3);
	position[0] = x;
	position[1] = y;
	position[2] = z;

	std::vector<std::vector<int> > toRemake = getAdjacentBlockCoordinates(x, y, z);
	toRemake.push_back(position);
	
	for(unsigned int i = 0; i < toRemake.size(); i++){
		reconstructFaceTracker(toRemake[i][0], toRemake[i][1], toRemake[i][2]);
	}

	save();
	dirty = false;
}

void Chunk::reconstructFaceTracker(int x, int y, int z){
	if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && z >= 0 && z < LENGTH){	
		std::vector<unsigned char> adjacentBlocks = getAdjacentBlockTypes(x, y, z);
		unsigned short key = (x << 12 | y << 4 | z << 0);

		if(faceTrackers.count(key)){
			faceTrackers.find(key)->second.popFaces();
			faceTrackers.erase(key);
		}
		
		if(isTransparent(x, y, z)){
			for(unsigned int j = 0; j < adjacentBlocks.size(); j++){
				if(adjacentBlocks[j] != getBlock(x, y, z) && adjacentBlocks[j] != 0){
					faceTrackers.emplace(key, faceTracker(this, x, y, z));
					break;
				}
			}
		}
		transparentMesh.updateMesh();
		opaqueMesh.updateMesh();
	}else{
		world->reconstructFaceTracker(16*this->x+x, y, 16*this->z+z);
	}
}	

faceTracker::faceTracker(Chunk *chunk, int x, int y, int z){	
	this->chunk = chunk;
	for(int i = 0; i < 6; i++){
		transparentFaces[i] = -1;
		opaqueFaces[i] = -1;
	}
	
	std::vector<unsigned char> adjacentBlocks = chunk->getAdjacentBlockTypes(x, y, z);
	std::vector<int> adjacentLightLevels = chunk->getAdjacentLightLevels(x, y, z);
	int lightLevel = chunk->getLightLevel(x, y, z);
	vertex temp[6];

	for(unsigned int i = 0; i < adjacentBlocks.size(); i++){
		if(adjacentBlocks[i] != chunk->getBlock(x, y, z) && adjacentBlocks[i] != 0){
			if(chunk->isTransparent(adjacentBlocks[i])){
				chunk->face->getVertices(i, glm::vec3((float)x, (float)y, (float)z), temp);
				transparentFaces[i] = chunk->transparentMesh.getIndex();
				chunk->transparentMesh.pushFace(temp, chunk->world->getFaceTexture(i, adjacentBlocks[i]), lightLevel, transparentFaces[i]);
			}else{
				chunk->face->getVertices(i, glm::vec3((float)x, (float)y, (float)z), temp);
				opaqueFaces[i] = chunk->opaqueMesh.getIndex();
				chunk->opaqueMesh.pushFace(temp, chunk->world->getFaceTexture(i, adjacentBlocks[i]), lightLevel, opaqueFaces[i]);
			}
		}else{
			transparentFaces[i] = -1;
			opaqueFaces[i] = -1;
		}
	}
}

void faceTracker::popFaces(){
	for(int i = 0; i < 6; i++){
		if(opaqueFaces[i] != -1){
			chunk->opaqueMesh.popFace(opaqueFaces[i]);
		}
		if(transparentFaces[i] != -1){
			chunk->transparentMesh.popFace(transparentFaces[i]);
		}
	}
}

/*
void Chunk::makeChunkmesh(){
	if(!dirty) return;

	//world->lightManager->lightChunk(this);
	
	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if(getBlock(x, y, z) != 0){
					std::vector<unsigned char> adjacentBlocks = getAdjacentBlockTypes(x, y, z);
					std::vector<int> adjacentLightLevels = getAdjacentLightLevels(x, y, z);
					vertex temp[6];

					for(unsigned int i = 0; i < adjacentBlocks.size(); i++){
						if(adjacentBlocks[i] != 0){
							if(isTransparent(adjacentBlocks[i])){
								face->getVertices(i, glm::vec3((float)x, (float)y, (float)z), temp);
								transparentMesh.pushFace(temp, world->getFaceTexture(i, adjacentBlocks[i]), adjacentLightLevels[i]);
							}else{
								face->getVertices(i, glm::vec3((float)x, (float)y, (float)z), temp);
								opaqueMesh.pushFace(temp, world->getFaceTexture(i, adjacentBlocks[i]), adjacentLightLevels[i]);
							}
						}
					}
				}else{
					continue;					//if a block is not air, then we move on
				}
			}
		}
	}
	
	opaqueMesh.setupMesh();
	transparentMesh.setupMesh();
	meshed = true;
	save();
	dirty = false;
}
*/

void Chunk::draw(bool drawTransparent){
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	if(!drawTransparent){
		opaqueMesh.draw();
	}else{
		transparentMesh.draw();
	}
}
