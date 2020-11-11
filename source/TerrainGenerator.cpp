#include "TerrainGenerator.h"
#include "FastNoise/FastNoiseLite.h"
#include "Chunk.h"
#include "World.h"

#include <math.h>
#include <iostream>

const float a = 256.0f;
const float b = 10.0f;

TerrainGenerator::TerrainGenerator(World *world){
	noise = new FastNoiseLite;
	
	this->world = world;
}

TerrainGenerator::~TerrainGenerator(){
	delete noise;
}

inline float TerrainGenerator::GetNoise(Chunk *chunk, int x, int y, int z){
	return ((noise->GetNoise((float)(16*chunk->x+x), (float)y, (float)(16*chunk->z+z)))+1)/2;
}

inline float TerrainGenerator::GetNoise(Chunk *chunk, int x, int z){
	return ((noise->GetNoise((float)(16*chunk->x+x), (float)(16*chunk->z+z)))+1)/2;
}

void TerrainGenerator::generateChunk(Chunk *chunk){
	//std::cout << "generating chunk...\n";
	generateBase(chunk);
}

void TerrainGenerator::generateBase(Chunk *chunk){
	float val = 0.0f;
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			noise->SetFrequency(0.01f);
			val = GetNoise(chunk, x, 0, z);
			val *= 90.0f;

			int y = 0;
			for(; y < val; y++){
				chunk->list[x][y][z] = 3;	
				
				if(y < HEIGHT - 1 && chunk->list[x][y][z] == 3 && !chunk->list[x][y+1][z]) chunk->list[x][y][z] = 1;
				if(y < HEIGHT - 4 && chunk->list[x][y+3][z] == 1){
					chunk->list[x][y][z] = 2;
					chunk->list[x][y+1][z] = 2;
					chunk->list[x][y+2][z] = 2;
				}
			}

			if(val >= waterLevel) continue;

			for(; y < waterLevel; y++){
				if(!chunk->list[x][y][z]) chunk->list[x][y][z] = 48;
			}


		}
	}
}


