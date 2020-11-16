#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

class World;
class Chunk;
class FastNoiseLite;

class TerrainGenerator{
	public:
		TerrainGenerator(World *world);
		~TerrainGenerator();

		void SetSeed(int seed);

		void generateChunk(Chunk *chunk);	
	private:
		FastNoiseLite *noise;

		int seed;
		int y = 0;

		float GetNoise(Chunk *chunk, int x, int y, int z);
		float GetNoise(Chunk *chunk, int x, int z);

		float clamp(float val);
		float hillFilter(float val);
		float step(float val, float threshold);

		void generateBase(Chunk *chunk);
		void generateBlocks(Chunk *chunk);	
		void clampContinent(float &val);

		World *world;

		int waterLevel = 63;
};

#endif
