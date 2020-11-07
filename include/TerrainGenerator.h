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

		float GetNoise(Chunk *chunk, int x, int z);
		void generateBase(Chunk *chunk);
		void generateWater(Chunk *chunk);
		void generateWaterBed(Chunk *chunk);
	
		void clampContinent(float &val);

		World *world;
};

#endif
