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

Chunk::Chunk(int x, int z, Face *face, Shader *shader, World *world, int modelLoc){
	this->face = face;
	this->shader = shader;
	this->world = world;
	this->modelLoc = modelLoc;

	setChunkPos(x, z);
	std::cout << x << ' ' << z << '\n';
	blocks = 0;
}

Chunk::~Chunk(){
}

void Chunk::generate(TerrainGenerator *generator){
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			for(int y = 0; y < HEIGHT; y++){
				list[x][y][z] = 0;
			}
		}
	}

	generator->generateChunk(this);

	loaded = true;
}


void Chunk::save(){
	if(!dirty) return;
	
	std::string path = "./world/chunk" + std::to_string(x) + '-' + std::to_string(z);
	std::cout << path << '\n';
	std::ofstream file;
	file.open(path, std::ifstream::binary);	

	int mode = 0;

	//if(blocks*3 < WIDTH*LENGTH*HEIGHT) mode = 1;

	if(file){
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

	std::string path = "./world/chunk" + std::to_string(x) + '-' + std::to_string(z);
	std::ifstream file;
	file.open(path, std::ifstream::binary);

	if(file){
		file.seekg(0, file.end);
		int length = file.tellg();
		std::cout << length << '\n';
		file.seekg(0, file.beg);
			
		std::cout << "Loading chunk...\n";

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
				blocks++;
			}
		}else{
			for(int x = 0; x < WIDTH; x++){
				for(int y = 0; y < HEIGHT; y++){
					for(int z = 0; z < LENGTH; z++){
						list[x][y][z] = buffer[i++];
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

unsigned char Chunk::getBlock(int x, int y, int z) const {
	return (unsigned char)(list[x][y][z]&255);
}

void Chunk::setBlock(int x, int y, int z, unsigned char id){
	//std::cout << this->x << ' ' << this->z << '\n';
	std::cout << x << ' ' << y << ' ' << z << '\n';
	list[x][y][z] = id;
}

void Chunk::calculateVisible(){
	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if(!(list[x][y][z] & 255)) continue;
				
				if(x > 0        && !(list[x-1][y][z] & 255)) list[x][y][z] |= 256;
				if(x < WIDTH-1  && !(list[x+1][y][z] & 255)) list[x][y][z] |= 512;
				if(y > 0        && !(list[x][y-1][z] & 255)) list[x][y][z] |= 1024;
				if(y < HEIGHT-1 && !(list[x][y+1][z] & 255)) list[x][y][z] |= 2048;
				if(z > 0        && !(list[x][y][z-1] & 255)) list[x][y][z] |= 4096;
				if(z < LENGTH-1 && !(list[x][y][z+1] & 255)) list[x][y][z] |= 8192;
					
				if(x == 0        && !world->getBlock(16*this->x+x-1, y,   16*this->z+z)  ) list[x][y][z] |= 256;
				if(x == WIDTH-1  && !world->getBlock(16*this->x+x+1, y,   16*this->z+z)  ) list[x][y][z] |= 512;
				if(y == 0        && !world->getBlock(16*this->x+x,   y-1, 16*this->z+z)  ) list[x][y][z] |= 1024;
				if(y == HEIGHT-1 && !world->getBlock(16*this->x+x,   y+1, 16*this->z+z)  ) list[x][y][z] |= 2048;
				if(z == 0        && !world->getBlock(16*this->x+x,   y,   16*this->z+z-1)) list[x][y][z] |= 4096;
				if(z == LENGTH-1 && !world->getBlock(16*this->x+x,   y,   16*this->z+z+1)) list[x][y][z] |= 8192;	
			}
		}
	}
}


void Chunk::makeChunkmesh(){
	if(!dirty) return;

	//std::cout << "Meshing...\n";

	calculateVisible();

	mesh.resetMesh();

	for(int x = 0; x < WIDTH; x++){
		for(int y = 0; y < HEIGHT; y++){
			for(int z = 0; z < LENGTH; z++){
				if((list[x][y][z] & 255) == 0) continue;	
				
				glm::vec3 position = glm::vec3((float)x, (float)y, (float)z);

				vertex temp[6];

				if(list[x][y][z] & 256){
					face->getVertices(WEST, position, temp);
					mesh.pushFace(temp, list[x][y][z]&255);
				}
				if(list[x][y][z] & 512){
					face->getVertices(EAST, position, temp);
					mesh.pushFace(temp, list[x][y][z]&255);
				}
				if(list[x][y][z] & 1024){
					face->getVertices(BLOCKDOWN, position, temp);
					mesh.pushFace(temp, list[x][y][z]&255);
				}
				if(list[x][y][z] & 2048){
					face->getVertices(BLOCKUP, position, temp);
					if((list[x][y][z]&255) == 1){
						mesh.pushFace(temp, 0);
					}else{
						mesh.pushFace(temp, list[x][y][z]&255);
					}
				}
				if(list[x][y][z] & 4096){
					face->getVertices(NORTH, position, temp);
					mesh.pushFace(temp, list[x][y][z]&255);
				}
				if(list[x][y][z] & 8192){
					face->getVertices(SOUTH, position, temp);
					mesh.pushFace(temp, list[x][y][z]&255);
				}
			}
		}
	}

	meshed = false;	
	mesh.setupMesh();
	meshed = true;
	dirty = false;
}

void Chunk::draw(){
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	mesh.draw();
}
