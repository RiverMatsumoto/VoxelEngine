#include "Chunk.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PerlinNoise.hpp>
#include <random>

const float Block::BLOCK_RENDER_SIZE = 1.0f;

Chunk::Chunk(glm::ivec2& position) : Chunk(position.x, position.y) { }

Chunk::Chunk(int xPos, int yPos) : m_meshVAO(0), m_meshVBO(0), m_pNumVertices(0)
{ 
	m_position = glm::ivec2(xPos, yPos);

	m_blocks = new Block[CHUNK_SIZE_CUBED];
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				GetBlock(x, y, z)->SetActive(true);
			}
		}
	}
}

Chunk::Chunk(const Chunk& chunk)
{
	m_meshVAO = chunk.m_meshVAO;
	m_meshVBO = chunk.m_meshVBO;
	m_pNumVertices = chunk.m_pNumVertices;
	m_position = chunk.m_position;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				*GetBlock(x, y, z) = *chunk.GetBlock(x,y,z);
				//GetBlock(x, y, z).SetActive(true);
			}
		}
	}
}

Chunk::~Chunk()
{
	delete[] m_blocks;
}

void Chunk::ApplyHeightMap(unsigned int seed)
{
	// terrain generation
	const siv::PerlinNoise::seed_type perlinSeed = seed;
	const siv::PerlinNoise perlin{ perlinSeed };
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			int blockPosX = x + (m_position.x * CHUNK_SIZE);
			int blockPosZ = z + (m_position.y * CHUNK_SIZE); // y because 2d map coords
			double maxHeight = perlin.octave2D_01((blockPosX * 0.02), (blockPosZ * 0.02), 4);
			maxHeight *= CHUNK_SIZE; // Temporarily just chunk size, but can implement chunk height

			for (int y = maxHeight; y < CHUNK_SIZE; y++)
			{ 
				GetBlock(x, y, z)->SetActive(false);
			}
		}
	}
}

void Chunk::CreateMesh()
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	glm::ivec3 blockPos;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		blockPos.x = x;
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			blockPos.y = y;
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				blockPos.z = z;

				if (!GetBlock(x,y,z)->IsActive())
					continue;
				CreateCube(blockPos,  vertices);
			}
		}
	}
	m_pNumVertices = (unsigned int)vertices.size();

	// bind VBO and VAO
	if (m_meshVAO == 0)
	{
		glGenVertexArrays(1, &m_meshVAO);
		glGenBuffers(1, &m_meshVBO);
	}

	glBindVertexArray(m_meshVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_meshVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);

	// texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Chunk::AddBlock(glm::ivec3 pos)
{
	AddBlock(pos.x, pos.y, pos.z);
}

void Chunk::AddBlock(int x, int y, int z)
{
	GetBlock(x,y,z)->SetActive(true);
	CreateMesh();
}

void Chunk::RemoveBlock(glm::ivec3 pos)
{
	RemoveBlock(pos.x, pos.y, pos.z);
}

void Chunk::RemoveBlock(int x, int y, int z)
{
	GetBlock(x,y,z)->SetActive(false);
	CreateMesh();
}

void Chunk::Render(Shader& shader)
{
	// note if we want to use different textures, use texture atlas

	shader.use();
	glBindVertexArray(m_meshVAO);
	glDrawArrays(GL_TRIANGLES, 0, m_pNumVertices); // 6 vertices per face * 6 faces = 36
}

Block* Chunk::GetBlock(int x, int y, int z) const
{
	return &m_blocks[z + (CHUNK_SIZE * (y + CHUNK_SIZE * x))];
}

void Chunk::CreateCube(glm::ivec3& pos, std::vector<Vertex>& vertices)
{
	// y are negative because opengl y axis is opposite way
	std::vector<glm::vec3> vertexPositions;
	// anchored from bottom left, position of 1 cube = chunk coordinates + block coordinates
	// vertex positions: formula chunkPosition + chunk offset + block size
	vertexPositions.push_back(glm::vec3(
		pos.x + CHUNK_SIZE * m_position.x,
		pos.y,	
		pos.z + CHUNK_SIZE * m_position.y)); 
	vertexPositions.push_back(glm::vec3(
		pos.x + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.x,	
		pos.y,	
		pos.z + CHUNK_SIZE * m_position.y));
	vertexPositions.push_back(glm::vec3(
		pos.x + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.x,
		pos.y + Block::BLOCK_RENDER_SIZE,
		pos.z + CHUNK_SIZE * m_position.y));
	vertexPositions.push_back(glm::vec3(
		pos.x + CHUNK_SIZE * m_position.x,
		pos.y + Block::BLOCK_RENDER_SIZE,
		pos.z + CHUNK_SIZE * m_position.y));
	vertexPositions.push_back(glm::vec3(
		pos.x + CHUNK_SIZE * m_position.x,
		pos.y,
		pos.z + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.y));
	vertexPositions.push_back(glm::vec3(
		pos.x + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.x,
		pos.y,
		pos.z + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.y));
	vertexPositions.push_back(glm::vec3(
		pos.x + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.x,
		pos.y + Block::BLOCK_RENDER_SIZE,
		pos.z + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.y));
	vertexPositions.push_back(glm::vec3(
		pos.x + CHUNK_SIZE * m_position.x, 
		pos.y + Block::BLOCK_RENDER_SIZE,
		pos.z + Block::BLOCK_RENDER_SIZE + CHUNK_SIZE * m_position.y));

	
	// face normals
	glm::vec3 normForward(0.0f, 0.0f, 1.0f);
	glm::vec3 normBack(0.0f, 0.0f, -1.0f);
	glm::vec3 normLeft(-1.0f, 0.0f, 0.0f);
	glm::vec3 normRight(1.0f, 0.0f, 0.0f);
	glm::vec3 normUp(0.0f, 1.0f, 0.0f); // note opengl has reverse y axis
	glm::vec3 normDown(0.0f, -1.0f, 0.0f);

	// texture coordinates
	glm::vec2 texBotLeft(0.0f, 0.0f);
	glm::vec2 texBotRight(1.0f, 0.0f);
	glm::vec2 texTopRight(1.0f, 1.0f);
	glm::vec2 texTopLeft(0.0f, 1.0f);

	// optimization, don't render vertices/faces if adjacent is already active. 
	// TODO, when implementing block removal/placing and updating the meshes
	//			try implement so it only renders the nearby block's vertices and maybe
	//			creates the verties for those meshes
	bool xPosActive = false;
	bool xNegActive = false;
	bool yPosActive = false;
	bool yNegActive = false;
	bool zPosActive = false;
	bool zNegActive = false;
	if (pos.x + 1 < CHUNK_SIZE)
		xPosActive = GetBlock(pos.x + 1, pos.y, pos.z)->IsActive();
	if (pos.x - 1 >= 0)
		xNegActive = GetBlock(pos.x - 1 ,pos.y, pos.z)->IsActive();
	if (pos.y + 1 < CHUNK_SIZE)
		yPosActive = GetBlock(pos.x, pos.y + 1,  pos.z)->IsActive();
	if (pos.y - 1 >= 0)
		yNegActive = GetBlock(pos.x, pos.y - 1, pos.z)->IsActive();
	if (pos.z + 1 < CHUNK_SIZE)
		zPosActive = GetBlock(pos.x, pos.y, pos.z + 1)->IsActive();
	if (pos.z - 1 >= 0)
		zNegActive = GetBlock(pos.x, pos.y, pos.z - 1)->IsActive();

	/*
				indices visual
					forward
				  7 --- 6
				 /|    /|
		left	3 --- 2 | right
				| 4 --| 5
				|/    |/
				0 --- 1
				back
	*/

	if (!zPosActive)
	{
		vertices.push_back(Vertex{ vertexPositions[6], normForward, texTopLeft });
		vertices.push_back(Vertex{ vertexPositions[5], normForward, texBotLeft });
		vertices.push_back(Vertex{ vertexPositions[4], normForward, texBotRight });
		vertices.push_back(Vertex{ vertexPositions[4], normForward, texBotRight });
		vertices.push_back(Vertex{ vertexPositions[7], normForward, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[6], normForward, texTopLeft });
	}
	// front face
	if (!zNegActive)
	{
		vertices.push_back(Vertex{ vertexPositions[0], normBack, texBotLeft });
		vertices.push_back(Vertex{ vertexPositions[1], normBack, texBotRight });
		vertices.push_back(Vertex{ vertexPositions[2], normBack, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[2], normBack, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[3], normBack, texTopLeft });
		vertices.push_back(Vertex{ vertexPositions[0], normBack, texBotLeft });
	}
	// left face
	if (!xNegActive)
	{
		vertices.push_back(Vertex{ vertexPositions[4], normLeft, texBotLeft });
		vertices.push_back(Vertex{ vertexPositions[0], normLeft, texBotRight });
		vertices.push_back(Vertex{ vertexPositions[3], normLeft, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[3], normLeft, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[7], normLeft, texTopLeft });
		vertices.push_back(Vertex{ vertexPositions[4], normLeft, texBotLeft });
	}
	// right face
	if (!xPosActive)
	{
		vertices.push_back(Vertex{ vertexPositions[1], normRight, texBotLeft });
		vertices.push_back(Vertex{ vertexPositions[5], normRight, texBotRight });
		vertices.push_back(Vertex{ vertexPositions[6], normRight, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[6], normRight, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[2], normRight, texTopLeft });
		vertices.push_back(Vertex{ vertexPositions[1], normRight, texBotLeft });
	}
	// top face
	if (!yPosActive)
	{
		vertices.push_back(Vertex{ vertexPositions[3], normUp, texBotLeft });
		vertices.push_back(Vertex{ vertexPositions[2], normUp, texBotRight });
		vertices.push_back(Vertex{ vertexPositions[6], normUp, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[6], normUp, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[7], normUp, texTopLeft });
		vertices.push_back(Vertex{ vertexPositions[3], normUp, texBotLeft });
	}
	// bottom face
	if (!yNegActive)
	{
		vertices.push_back(Vertex{ vertexPositions[5], normDown, texBotRight });
		vertices.push_back(Vertex{ vertexPositions[1], normDown, texTopRight });
		vertices.push_back(Vertex{ vertexPositions[0], normDown, texTopLeft });
		vertices.push_back(Vertex{ vertexPositions[0], normDown, texTopLeft });
		vertices.push_back(Vertex{ vertexPositions[4], normDown, texBotLeft });
		vertices.push_back(Vertex{ vertexPositions[5], normDown, texBotRight });
	}
}


