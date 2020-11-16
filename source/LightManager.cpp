#include "LightManager.h"
#include "Chunk.h"
#include "World.h"

// this lighting engine works as follows:
// first, it determines which blocks of air in a given chunk are both VISIBLE FROM ABOVE THE BUILDHEIGHT (y = HEIGHT) and ADJACENT TO A SOLID BLOCK. the coordinates of these are added to a vector and are given a light value of 3
// then, it goes through these air blocks and assigns each ADJACENT AIR BLOCK a value of 1 less than its own.
// thats it for lighting, once the air blocks have appropriate values of light, the processes is handed of to the meshing functions
// in the mesher, each face is assigned a light value of the air block that it is adjacent to
//
// this lighting engine will likely be very slow but it must only be called when a chunk is generated. the time of day does not directly correspond to the light level, assigned here


LightManager::LightManager(World *world){
	this->world = world;
}

void LightManager::lightChunk(Chunk *chunk){
	findVisibleAir(chunk);
	assignLightValues(chunk);
	visibleAir.clear();
}

void LightManager::findVisibleAir(Chunk *chunk){
	// first we are checking to see if any interior air blocks are within the suns line of sight
	// we do this separately from checking chunk boundaries because it is much faster this way but is not possible for chunk boundaries

	for(int x = 0; x < WIDTH; x++){
		for(int z = 0; z < LENGTH; z++){
			for(int y = HEIGHT-1; y >= 0; y--){
				chunk->setLightLevel(x, y, z, 0);
				if(chunk->isTransparent(x, y, z)){
					if((chunk->getBlock(x-1, y, z) != 0) ||
					(chunk->getBlock(x+1, y, z) != 0) ||
					(chunk->getBlock(x, y-1, z) != 0) ||
					(chunk->getBlock(x, y, z-1) != 0) ||
					(chunk->getBlock(x, y, z+1) != 0)){
						std::vector<int> coordinates(3);
						coordinates[0] = x;
						coordinates[1] = y;
						coordinates[2] = z;

						visibleAir.push_back(coordinates);
					}
				}else{
					break;
				}
			}
		}
	}
}

void LightManager::assignLightValues(Chunk *chunk){
	for(unsigned int i = 0; i < visibleAir.size(); i++){
		recursiveAssign(chunk, visibleAir[i][0], visibleAir[i][1], visibleAir[i][2], 4);			// this begins the recursive function
	}
}

void LightManager::recursiveAssign(Chunk *chunk, int x, int y, int z, int lightLevel){
	if(lightLevel == 0 || chunk->getLightLevel(x, y, z) >= lightLevel) return;											// if the assigned light value is 0, stop spreading light	
	
	chunk->setLightLevel(x, y, z, lightLevel);									// assign light value

	if(chunk->getBlock(x-1, y, z) == 0) recursiveAssign(chunk, x-1, y, z, lightLevel-1);				// recursive calls to assign light values
	if(chunk->getBlock(x+1, y, z) == 0) recursiveAssign(chunk, x+1, y, z, lightLevel-1);				// recursive calls to assign light values
	if(chunk->getBlock(x, y-1, z) == 0) recursiveAssign(chunk, x, y-1, z, lightLevel-1);				// recursive calls to assign light values
	if(chunk->getBlock(x, y+1, z) == 0) recursiveAssign(chunk, x, y+1, z, lightLevel-1);				// recursive calls to assign light values
	if(chunk->getBlock(x, y, z-1) == 0) recursiveAssign(chunk, x, y, z-1, lightLevel-1);				// recursive calls to assign light values
	if(chunk->getBlock(x, y, z+1) == 0) recursiveAssign(chunk, x, y, z+1, lightLevel-1);				// recursive calls to assign light values
}
