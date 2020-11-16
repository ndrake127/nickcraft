#include "TerrainGenerator.h"
#include "FastNoise/FastNoiseLite.h"
#include "Chunk.h"
#include "World.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>

const float compression = 20.0f;
const float shift = 0.3f;

TerrainGenerator::TerrainGenerator(World *world){
	noise = new FastNoiseLite;
	
	this->world = world;

	this->seed = seed;
}

TerrainGenerator::~TerrainGenerator(){
	delete noise;
}

void TerrainGenerator::SetSeed(int seed){
	this->seed = seed;
	srand(this->seed);
	noise->SetSeed(this->seed);
}

inline float TerrainGenerator::GetNoise(Chunk *chunk, int x, int y, int z){
	return ((noise->GetNoise((float)(16*chunk->x+x), (float)y, (float)(16*chunk->z+z)))+1)/2;
}

inline float TerrainGenerator::GetNoise(Chunk *chunk, int x, int z){
	return ((noise->GetNoise((float)(16*chunk->x+x), (float)(16*chunk->z+z)))+1)/2;
}

void TerrainGenerator::generateChunk(Chunk *chunk){
	generateBase(chunk);
}

void TerrainGenerator::generateBase(Chunk *chunk){
	float val = 0.0f;
	float val2 = 0.0f;
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			noise->SetFrequency(0.0005f);
			val = clamp(GetNoise(chunk, x, y, z));
			val *= 64.0f;

			noise->SetFrequency(0.01f);
			val += 10.0*hillFilter(GetNoise(chunk, x, 0, z));
			
			noise->SetFrequency(0.02f);
			val += 2.0f*GetNoise(chunk, x, y, z);
			
			noise->SetFrequency(0.1f);
			val += 0.5f*GetNoise(chunk, x, y, z);

			if(val >= HEIGHT) val = HEIGHT;

			for(int y = 0; y < val; y++){
				chunk->list[x][y][z].id = 3;
			}

			noise->SetFrequency(0.01f);
			val2 = 10.0f*step(GetNoise(chunk, x, 20, z), 0.9f);

			if(val > waterLevel){
				for(int y = val; y < val+val2; y++){
					chunk->list[x][y][z].id = 3;
				}
			}

			for(int y = val; y <= waterLevel; y++){
				if(chunk->list[x][y][z].id == 3) continue;
				chunk->list[x][y][z].id = 48;
			}
		}
	}

	for(unsigned int x = 0; x < WIDTH; x++){
		for(unsigned int z = 0; z < LENGTH; z++){
			for(unsigned int y = HEIGHT-2; y >= waterLevel; y--){
				if(chunk->list[x][y+1][z].id == 0 && chunk->list[x][y][z].id == 3){
					chunk->list[x][y][z].id = 1;				// replaces top layer of stone with grass
					if(x >= 2 && x <= 13 && z >= 2 && z <= 13 && std::rand() % 1000 < 5){	
						for(int x2 = -2; x2 <= 2; x2++){
							for(int z2 = -2; z2 <= 2; z2++){
								chunk->list[x+x2][y+5][z+z2].id = 35;
								chunk->list[x+x2][y+6][z+z2].id = 35;
							}
						}
						for(int x2 = -1; x2 <= 1; x2++){
							for(int z2 = -1; z2 <= 1; z2++){
								chunk->list[x+x2][y+7][z+z2].id = 35;
							}
						}
						chunk->list[x-1][y+8][z].id = 35;
						chunk->list[x+1][y+8][z].id = 35;
						chunk->list[x][y+8][z-1].id = 35;
						chunk->list[x][y+8][z+1].id = 35;
						chunk->list[x][y+8][z].id = 35;

						chunk->list[x][y+1][z].id = 33;
						chunk->list[x][y+2][z].id = 33;
						chunk->list[x][y+3][z].id = 33;
						chunk->list[x][y+4][z].id = 33;
						chunk->list[x][y+5][z].id = 33;
					}
				}

				if((chunk->list[x][y][z].id) == 1){
					chunk->list[x][y-1][z].id = 2;				// replaces next 3 layers with dirt
					chunk->list[x][y-2][z].id = 2;
					chunk->list[x][y-3][z].id = 2;
				}
				if((chunk->list[x][y][z].id) == 3 && y < waterLevel){
					chunk->list[x][y][z].id = 4;				// replaces next 3 layers with dirt
					chunk->list[x][y-1][z].id = 4;
					chunk->list[x][y-2][z].id = 4;
				}
			}
		}
	}
}

float TerrainGenerator::clamp(float val){
	return 1.0f/(1.0f+std::exp(-1*compression*(val-shift)));
}

float TerrainGenerator::hillFilter(float val){
	return std::pow(val, 10);
}

float TerrainGenerator::step(float val, float threshold){
	if(val > threshold) return val;
	else return 0.0f;
}
