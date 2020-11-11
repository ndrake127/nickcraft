#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

class World;
class Chunk;
class FastNoiseLite;

class TerrainGenerator{
	public:
		TerrainGenerator(World *world);
		~TerrainGenerator();

		void generateChunk(Chunk *chunk);	
	private:
		FastNoiseLite *noise;

		float GetNoise(Chunk *chunk, int x, int y, int z);
		float GetNoise(Chunk *chunk, int x, int z);

		void generateBase(Chunk *chunk);
		void generateBlocks(Chunk *chunk);	
		void clampContinent(float &val);

		World *world;

		int waterLevel = 64;
};

#endif
