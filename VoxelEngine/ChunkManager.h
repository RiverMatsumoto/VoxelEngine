#pragma once

#include "Chunk.h"
#include "Shader.h"

#include <vector>

class ChunkManager
{
public:
	// Temporary 2d vectors. Maybe object pool pattern the chunks
	// only have n chunks allowed to load at the same time
	Chunk* chunks;
	unsigned int seed;

	ChunkManager();
	~ChunkManager();
	
	void InitializeAllChunks();
	void RebuildChunk(int x, int y, int z);
	void RebuildChunk(Chunk*& chunk);
	void RemoveBlock(glm::vec3 pos);
	void RemoveBlock(int x, int y, int z);
	Chunk* GetChunk(int x, int y);
	Block* GetBlock(int x, int y, int z);
	bool IsBlockActive(int x, int y, int z);
	void Render(Shader& shader);
};

