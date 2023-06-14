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
	unsigned int m_meshVAO, m_meshVBO;
	static const int CHUNK_SIZE = 32; // temporarily just CHUNK_SIZE^3 big

	Chunk();
	~Chunk();
    void CreateMesh();
	void Update(float dt);
	void Render(Shader& shader);
private:
	Block*** m_pBlocks;
	unsigned int m_pNumVertices;
	void CreateCube(glm::vec3 position, std::vector<Vertex>& vertices);

};

