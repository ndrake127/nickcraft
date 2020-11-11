#include "Chunk.h"
#include "World.h"
#include "TerrainGenerator.h"

#include <iostream>

Chunk::Chunk(){
	face = nullptr;
	shader = nullptr;
	world = nullptr;
	modelLoc = 0;

	x = 0;
	z = 0;
	setChunkPos(0, 0);
	blocks = 0;
}

Chunk::Chunk(int x, int z, Face *face, Shader *shader, World *world, int modelLoc, std::string worldpath){
	this->face = face;
	this->shader = shader;
	opaqueMesh.setShader(shader);
	transparentMesh.setShader(shader);
	this->world = world;
	this->modelLoc = modelLoc;
	blocks = 0;

	setChunkPos(x, z);

	path = worldpath + "/chunk" + std::to_string(x) + '-' + std::to_string(z);	
}

Chunk::~Chunk(){
}

void Chunk::generate(TerrainGenerator *generator){
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			for(int y = 0; y < HEIGHT; y++){
				list[x][y][z] = 16384;			//this sets the id portion to 0 and the transparency bit to true
			}
		}
	}

	generator->generateChunk(this);

	loaded = true;
}


void Chunk::save(){
	if(!dirty) return;
	
	std::ofstream file;
	file.open(path, std::ifstream::binary);	

	int mode = 0;

	//if(blocks*3 < WIDTH*LENGTH*HEIGHT) mode = 1;

	if(file){
		std::cout << "test" << '\n';
		if(mode == 1){
			for(int x = 3; x < 6; x++){
				for(int y = 0; y < HEIGHT; y++){
					for(int z = 3; z < 6; z++){
						if(list[x][y][z]&255 == 0) blocks++;
					}
				}
			}
			
			char *buffer = new char[1+blocks*3];
			
			int i = 0;
			buffer[i++] = 1;
	
			for(unsigned int x = 0; x < WIDTH; x++){
				for(unsigned int y = 0; y < HEIGHT; y++){
					for(unsigned int z = 0; z < LENGTH; z++){
						if(list[x][y][z]&255 != 0){
							buffer[i++] = (unsigned char)((x<<4)|z);
							buffer[i++] = (unsigned char)y;
							buffer[i++] = (unsigned char)list[x][y][z]&255;
						}
					}
				}
			}			
	
			file.write(buffer, 1+blocks*3);
			delete [] buffer;
		}else{
			char *buffer = new char[1+WIDTH*LENGTH*HEIGHT];
			
			int i = 0;
			buffer[i++] = 0;

			for(int x = 0; x < WIDTH; x++){
				for(int y = 0; y < HEIGHT; y++){
					for(int z = 0; z < LENGTH; z++){
						buffer[i++] = list[x][y][z]&255;
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

		if(mode){
			while(i < length){
				unsigned int x = (unsigned char)buffer[i++];
				unsigned int z = x;
				x = x >> 4;
				z = z & 15;
				unsigned int y = (unsigned char)buffer[i++];
				list[x][y][z] = (unsigned char)buffer[i++];
				if(list[x][y][z] == 0 || list[x][y][z] == 48) list[x][y][z] |= 16384;
				blocks++;
			}
		}else{
			for(int x = 0; x < WIDTH; x++){
				for(int y = 0; y < HEIGHT; y++){
					for(int z = 0; z < LENGTH; z++){
						list[x][y][z] = buffer[i++];
						if(list[x][y][z] == 0 || list[x][y][z] == 48 || list[x][y][z] == 35) list[x][y][z] |= 16384;
						if(list[x][y][z]) blocks++;
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
	return (bool)(list[x][y][z]&16384);
}

unsigned char Chunk::getBlock(int x, int y, int z) const {
	return (unsigned char)(list[x][y][z]&255);
}

void Chunk::setBlock(int x, int y, int z, unsigned char id){
	list[x][y][z] = id;
}

void Chunk::calculateVisible(){
	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if(!(list[x][y][z] & 255)) continue;										// air is never visible, so skip if the block is air
			
				if((list[x][y][z] & 255) != 48 && (list[x][y][z] & 255) != 35 && (list[x][y][z] & 255) != 5){										// if the block being checked is water
					if(x > 0        && (list[x-1][y][z] & 16384)) list[x][y][z] |= 256;
					if(x < WIDTH-1  && (list[x+1][y][z] & 16384)) list[x][y][z] |= 512;
					if(y > 0        && (list[x][y-1][z] & 16384)) list[x][y][z] |= 1024;
					if(y < HEIGHT-1 && (list[x][y+1][z] & 16384)) list[x][y][z] |= 2048;
					if(z > 0        && (list[x][y][z-1] & 16384)) list[x][y][z] |= 4096;
					if(z < LENGTH-1 && (list[x][y][z+1] & 16384)) list[x][y][z] |= 8192;

					if(x == 0        && world->isTransparent(16*this->x+x-1, y,   16*this->z+z  )) list[x][y][z] |= 256;
					if(x == WIDTH-1  && world->isTransparent(16*this->x+x+1, y,   16*this->z+z  )) list[x][y][z] |= 512;
					if(y == 0        && world->isTransparent(16*this->x+x,   y-1, 16*this->z+z  )) list[x][y][z] |= 1024;
					if(y == HEIGHT-1 && world->isTransparent(16*this->x+x,   y+1, 16*this->z+z  )) list[x][y][z] |= 2048;
					if(z == 0        && world->isTransparent(16*this->x+x,   y,   16*this->z+z-1)) list[x][y][z] |= 4096;
					if(z == LENGTH-1 && world->isTransparent(16*this->x+x,   y,   16*this->z+z+1)) list[x][y][z] |= 8192;
				}else{														// if the block being checked is not water
					if(x > 0        && !(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
					if(x < WIDTH-1  && !(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
					if(y > 0        && !(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
					if(y < HEIGHT-1 && !(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
					if(z > 0        && !(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
					if(z < LENGTH-1 && !(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;

					if(x == 0        && !world->getBlock(16*this->x+x-1, y,   16*this->z+z  )) list[x][y][z] |= 256;
					if(x == WIDTH-1  && !world->getBlock(16*this->x+x+1, y,   16*this->z+z  )) list[x][y][z] |= 512;
					if(y == 0        && !world->getBlock(16*this->x+x,   y-1, 16*this->z+z  )) list[x][y][z] |= 1024;
					if(y == HEIGHT-1 && !world->getBlock(16*this->x+x,   y+1, 16*this->z+z  )) list[x][y][z] |= 2048;
					if(z == 0        && !world->getBlock(16*this->x+x,   y,   16*this->z+z-1)) list[x][y][z] |= 4096;
					if(z == LENGTH-1 && !world->getBlock(16*this->x+x,   y,   16*this->z+z+1)) list[x][y][z] |= 8192;
				}
			}
		}
	}
}


void Chunk::makeChunkmesh(){
	if(!dirty) return;

	//std::cout << "Meshing...\n";

	calculateVisible();

	opaqueMesh.resetMesh();
	transparentMesh.resetMesh();

	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if((list[x][y][z] & 255) == 0) continue;	
				
				glm::vec3 position = glm::vec3((float)x, (float)y, (float)z);

				vertex temp[6];
				
				if((list[x][y][z]&255) != 48 && (list[x][y][z]&255) != 35){
					if(list[x][y][z] & 256){
						face->getVertices(WEST, position, temp);
						opaqueMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 512){
						face->getVertices(EAST, position, temp);
						opaqueMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 1024){
						face->getVertices(BLOCKDOWN, position, temp);
						if((list[x][y][z]&255) == 33){
							opaqueMesh.pushFace(temp, 34);
						}else{
							opaqueMesh.pushFace(temp, list[x][y][z]&255);
						}
					}
					if(list[x][y][z] & 2048){
						face->getVertices(BLOCKUP, position, temp);
						if((list[x][y][z]&255) == 1){
							opaqueMesh.pushFace(temp, 0);
						}else if((list[x][y][z]&255) == 33){						
							opaqueMesh.pushFace(temp, 34);
						}else{
							opaqueMesh.pushFace(temp, list[x][y][z]&255);
						}
					}
					if(list[x][y][z] & 4096){
						face->getVertices(NORTH, position, temp);
						opaqueMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 8192){
						face->getVertices(SOUTH, position, temp);
						opaqueMesh.pushFace(temp, list[x][y][z]&255);
					}
				}else{
					if(list[x][y][z] & 256){
						face->getVertices(WEST, position, temp);
						transparentMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 512){
						face->getVertices(EAST, position, temp);
						transparentMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 1024){
						face->getVertices(BLOCKDOWN, position, temp);
						transparentMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 2048){
						face->getVertices(BLOCKUP, position, temp);
						transparentMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 4096){
						face->getVertices(NORTH, position, temp);
						transparentMesh.pushFace(temp, list[x][y][z]&255);
					}
					if(list[x][y][z] & 8192){
						face->getVertices(SOUTH, position, temp);
						transparentMesh.pushFace(temp, list[x][y][z]&255);
					}
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

void Chunk::draw(bool drawTransparent){
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	if(!drawTransparent){
		opaqueMesh.draw();
	}else{
		transparentMesh.draw();
	}
}
