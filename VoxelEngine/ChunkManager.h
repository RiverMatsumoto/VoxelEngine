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

	ChunkManager();
	~ChunkManager();
	
	void BuildAllChunks();
	void RebuildChunk(int x, int y, int z);
	void RebuildChunk(Chunk*& chunk);
	void RemoveBlock(int x, int y, int z);
	Chunk* GetChunk(int x, int y);
	void Render(Shader& shader);
};

