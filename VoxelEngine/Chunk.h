#pragma once

#include "Block.h"
#include <glm/glm.hpp>
#include <vector>

#include "Shader.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Chunk
{
public:
	static const int CHUNK_SIZE = 32; // temporarily just CHUNK_SIZE^3 big
	unsigned int m_meshVAO, m_meshVBO;
	glm::ivec2 m_position;
	//Block m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	Block* m_blocks;

	Chunk(glm::ivec2& position);
	Chunk(int xPos, int yPos);
	Chunk(const Chunk& chunk);
	~Chunk();
    void CreateMesh();
	void Render(Shader& shader);
	Block& GetBlock(int x, int y, int z) const ;
	void AddBlock(glm::ivec3 position);
	void AddBlock(int x, int y, int z);
	void RemoveBlock(glm::ivec3 position);
	void RemoveBlock(int x, int y, int z);
private:
	unsigned int m_pNumVertices;
	void CreateCube(glm::ivec3& position, std::vector<Vertex>& vertices);

};

