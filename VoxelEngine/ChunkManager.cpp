#include "ChunkManager.h"
#include <glm/glm.hpp>
#include <iostream>
#include <PerlinNoise.hpp>

ChunkManager::ChunkManager()
{
	chunks = static_cast<Chunk*>(operator new[](sizeof(Chunk) * 4 * 4));
	for (int x = 0; x < 4; x++) 
	{
		for (int y = 0; y < 4; y++)
		{
			// index is used for flattened 2d array
			new (&chunks[y + (4 * x)]) Chunk(x, y);
		}
	}
}

ChunkManager::~ChunkManager()
{
	// clean up data, then clear memory from heap
	for (int x = 3; x >= 0; x--)
	{
		for (int y = 3; y >= 0; y--)
		{
			chunks[y + (4 * x)].~Chunk();
		}
	}

	operator delete[](chunks);
}

void ChunkManager::BuildAllChunks()
{
	
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			GetChunk(x,y)->CreateMesh();
		}
	}
}

void ChunkManager::RebuildChunk(int x, int y, int z)
{
	Chunk* chunk = GetChunk(x, z);
	chunk->CreateMesh();
}

void ChunkManager::RebuildChunk(Chunk*& chunk)
{
	chunk->CreateMesh();
}

void ChunkManager::RemoveBlock(int x, int y, int z)
{
	Chunk* chunk = GetChunk(x / Chunk::CHUNK_SIZE, z / Chunk::CHUNK_SIZE);
	chunk->RemoveBlock(x,y,z);
}

Chunk* ChunkManager::GetChunk(int x, int y)
{
	return &chunks[y + (4 * x)];
}

void ChunkManager::Render(Shader& shader)
{
	// TODO cull chunks that are not near player
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			GetChunk(x,y)->Render(shader);
		}
	}
}


