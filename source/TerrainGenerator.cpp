#include "TerrainGenerator.h"
#include "FastNoise/FastNoiseLite.h"
#include "Chunk.h"
#include "World.h"

#include <math.h>
#include <iostream>

TerrainGenerator::TerrainGenerator(World *world){
	noise = new FastNoiseLite;
	
	this->world = world;
}

TerrainGenerator::~TerrainGenerator(){
	delete noise;
}

// returns noise between 0 and 1
inline float TerrainGenerator::GetNoise(Chunk *chunk, int x, int z){
	return ((noise->GetNoise((float)(16*chunk->x+x), (float)(16*chunk->z+z)))+1)/2;
}

void TerrainGenerator::generateChunk(Chunk *chunk){
	std::cout << "generating chunk...\n";
	generateBase(chunk);
	generateWater(chunk);
	//generateWaterBed(chunk);
}

void TerrainGenerator::generateBase(Chunk *chunk){
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			float val = 0.0f;
			
			noise->SetSeed(1);

			noise->SetFrequency(0.0009f);
			val += GetNoise(chunk, x, z);
			clampContinent(val);
			val *= 64.0f;
			
			noise->SetFrequency(0.01);
			val += 10.0f*GetNoise(chunk, x, z);
		
			noise->SetFrequency(0.08);
			val += 1.0f*GetNoise(chunk, x, z);

			int height = val;
		
			if(height < 4) height = 4;
			if(height >= HEIGHT) height = HEIGHT;

			int y = 0;
			for(; y < height-4; y++){
				chunk->list[x][y][z] = 3;
			}
			for(; y < height-1; y++){
				chunk->list[x][y][z] = 2;
			}
			for(; y < height; y++){
				chunk->list[x][y][z] = 1;
			}
		}
	}
}

void TerrainGenerator::generateWater(Chunk *chunk){
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			for(int y = 0; y < 60; y++){
				if(!chunk->list[x][y][z]) chunk->list[x][y][z] = 48;
			}
		}
	}
}

void TerrainGenerator::generateWaterBed(Chunk *chunk){
	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			for(int y = 60; y >= 0; y--){
				if(chunk->list[x][y][z] == 48){
					if(world->getBlock(16*chunk->x+x-1, y, 16*chunk->z+z) != 48){
						world->setBlock(16*chunk->x+x-1, y, 16*chunk->z+z, 4);
					}
					if(world->getBlock(16*chunk->x+x+1, y, 16*chunk->z+z) != 48){
						world->setBlock(16*chunk->x+x+1, y, 16*chunk->z+z, 4);
					}
					if(world->getBlock(16*chunk->x+x, y, 16*chunk->z+z-1) != 48){
						world->setBlock(16*chunk->x+x, y, 16*chunk->z+z-1, 4);
					}
					if(world->getBlock(16*chunk->x+x, y, 16*chunk->z+z+1) != 48){
						world->setBlock(16*chunk->x+x, y, 16*chunk->z+z+1, 4);
					}
				}
				if(chunk->list[x][y+1][z] == 48 && chunk->list[x][y][z] != 48){
					chunk->list[x][y][z] = 4;
					chunk->list[x][y-1][z] = 4;
					continue;
				}
			}
		}
	}	
}

// sigmoid for continental noise layer
void TerrainGenerator::clampContinent(float &val){
	val = 1.0f/(1.0f+5.0f*std::pow(5.0f, -20.0f*(val-0.2f)));
}
