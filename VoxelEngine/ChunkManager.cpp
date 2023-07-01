#include "ChunkManager.h"
#include <glm/glm.hpp>
#include <iostream>
#include <PerlinNoise.hpp>

ChunkManager::ChunkManager()
{
	// c++ random number generation looks dumb lol
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<unsigned int> dist(0xFF, 0xFFFFFFFF);
	seed = dist(rng);
	
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

void ChunkManager::InitializeAllChunks()
{
	
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			Chunk* chunk = GetChunk(x, y);
			chunk->ApplyHeightMap(seed);
			chunk->CreateMesh();
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

void ChunkManager::RemoveBlock(glm::vec3 pos)
{
	glm::ivec3 temp((int)pos.x, (int)pos.y, (int)pos.z);
	RemoveBlock(temp.x, temp.y, temp.z);
}

void ChunkManager::RemoveBlock(int x, int y, int z)
{
	// TODO, implement checks for neighboring chunks if block is at edge of chunk
	Chunk* chunk = GetChunk(x / Chunk::CHUNK_SIZE, z / Chunk::CHUNK_SIZE);
	chunk->RemoveBlock(x % Chunk::CHUNK_SIZE, y % Chunk::CHUNK_SIZE, z % Chunk::CHUNK_SIZE);
}

Chunk* ChunkManager::GetChunk(int x, int y)
{
	return &chunks[y + (4 * x)];
}

Block* ChunkManager::GetBlock(int x, int y, int z)
{
	return GetChunk(x / Chunk::CHUNK_SIZE, z / Chunk::CHUNK_SIZE)
		->GetBlock(x % Chunk::CHUNK_SIZE, y % Chunk::CHUNK_SIZE, z % Chunk::CHUNK_SIZE);
}

bool ChunkManager::IsBlockActive(int x, int y, int z)
{
	return GetChunk(x / Chunk::CHUNK_SIZE, z / Chunk::CHUNK_SIZE)->
		GetBlock(x % Chunk::CHUNK_SIZE, y % Chunk::CHUNK_SIZE, z % Chunk::CHUNK_SIZE)
		->IsActive();
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


