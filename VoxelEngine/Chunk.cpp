#include "Chunk.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const float Block::BLOCK_RENDER_SIZE = 1.0f;

Chunk::Chunk()
{ 
	m_meshVAO = 0;
	m_meshVBO = 0;
	m_pNumVertices = 0;

	m_pBlocks = new Block * *[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		m_pBlocks[i] = new Block * [CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			m_pBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
}

Chunk::~Chunk()
{
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			delete[] m_pBlocks[i][j];
		}
		delete[] m_pBlocks[i];
	}
	delete[] m_pBlocks;
}

void Chunk::CreateMesh()
{
	// generate data for mesh
	std::vector<Vertex> vertices;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				CreateCube(glm::vec3(x,y,z),  vertices);
			}
		}
	}
	m_pNumVertices = vertices.size();

	// bind VBO and VAO
	glGenVertexArrays(1, &m_meshVAO);
	glGenBuffers(1, &m_meshVBO);

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

void Chunk::Update(float dt)
{
}

void Chunk::Render(Shader& shader)
{
	// note if we want to use different textures, use texture atlas

	shader.use();
	glBindVertexArray(m_meshVAO);
	glDrawArrays(GL_TRIANGLES, 0, m_pNumVertices); // 6 vertices per face * 6 faces = 36
}

void Chunk::CreateCube(glm::vec3 pos, std::vector<Vertex>& vertices)
{
	std::vector<glm::vec3> vertexPositions;
	// anchored from bottom left, position of 1 cube = chunk coordinates + block coordinates
	// vertex positions
	vertexPositions.push_back(glm::vec3(pos.x,								pos.y,								pos.z)); 
	vertexPositions.push_back(glm::vec3(pos.x + Block::BLOCK_RENDER_SIZE,	pos.y,								pos.z));
	vertexPositions.push_back(glm::vec3(pos.x + Block::BLOCK_RENDER_SIZE,	pos.y + Block::BLOCK_RENDER_SIZE,	pos.z));
	vertexPositions.push_back(glm::vec3(pos.x,								pos.y + Block::BLOCK_RENDER_SIZE,	pos.z));
	vertexPositions.push_back(glm::vec3(pos.x,								pos.y,								pos.z + Block::BLOCK_RENDER_SIZE));
	vertexPositions.push_back(glm::vec3(pos.x + Block::BLOCK_RENDER_SIZE,	pos.y,								pos.z + Block::BLOCK_RENDER_SIZE));
	vertexPositions.push_back(glm::vec3(pos.x + Block::BLOCK_RENDER_SIZE,	pos.y + Block::BLOCK_RENDER_SIZE,	pos.z + Block::BLOCK_RENDER_SIZE));
	vertexPositions.push_back(glm::vec3(pos.x,								pos.y + Block::BLOCK_RENDER_SIZE,	pos.z + Block::BLOCK_RENDER_SIZE));
	
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


	/*
		indices visual
			forward
		  7 --- 6
		 /|    /|
		3 --- 2 |
		| 4 --| 5
		|/    |/
		0 --- 1
		back
	*/

	// back face
	vertices.push_back(Vertex{ vertexPositions[0], normBack, texBotLeft });
	vertices.push_back(Vertex{ vertexPositions[1], normBack, texBotRight });
	vertices.push_back(Vertex{ vertexPositions[2], normBack, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[2], normBack, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[3], normBack, texTopLeft });
	vertices.push_back(Vertex{ vertexPositions[0], normBack, texBotLeft });
	// front face
	vertices.push_back(Vertex{ vertexPositions[4], normForward, texBotLeft });
	vertices.push_back(Vertex{ vertexPositions[5], normForward, texBotRight });
	vertices.push_back(Vertex{ vertexPositions[6], normForward, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[6], normForward, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[7], normForward, texTopLeft });
	vertices.push_back(Vertex{ vertexPositions[4], normForward, texBotLeft });
	// left face
	vertices.push_back(Vertex{ vertexPositions[4], normLeft, texBotLeft });
	vertices.push_back(Vertex{ vertexPositions[0], normLeft, texBotRight });
	vertices.push_back(Vertex{ vertexPositions[3], normLeft, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[3], normLeft, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[7], normLeft, texTopLeft });
	vertices.push_back(Vertex{ vertexPositions[4], normLeft, texBotLeft });
	// right face
	vertices.push_back(Vertex{ vertexPositions[1], normRight, texBotLeft });
	vertices.push_back(Vertex{ vertexPositions[5], normRight, texBotRight });
	vertices.push_back(Vertex{ vertexPositions[6], normRight, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[6], normRight, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[2], normRight, texTopLeft });
	vertices.push_back(Vertex{ vertexPositions[1], normRight, texBotLeft });
	// top face
	vertices.push_back(Vertex{ vertexPositions[3], normUp, texBotLeft });
	vertices.push_back(Vertex{ vertexPositions[2], normUp, texBotRight });
	vertices.push_back(Vertex{ vertexPositions[6], normUp, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[6], normUp, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[7], normUp, texTopLeft });
	vertices.push_back(Vertex{ vertexPositions[3], normUp, texBotLeft });
	// bottom face
	vertices.push_back(Vertex{ vertexPositions[0], normDown, texBotLeft });
	vertices.push_back(Vertex{ vertexPositions[1], normDown, texBotRight });
	vertices.push_back(Vertex{ vertexPositions[5], normDown, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[5], normDown, texTopRight });
	vertices.push_back(Vertex{ vertexPositions[4], normDown, texTopLeft });
	vertices.push_back(Vertex{ vertexPositions[0], normDown, texBotLeft });
}


