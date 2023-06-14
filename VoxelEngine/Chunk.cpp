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

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				m_pBlocks[x][y][z].SetActive(true);
			}
		}
	}

	m_pBlocks[4][3][8].SetActive(false);
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

				if (!m_pBlocks[x][y][z].IsActive())
					continue;
				CreateCube(blockPos,  vertices);
			}
		}
	}
	m_pNumVertices = (unsigned int)vertices.size();

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

void Chunk::CreateCube(glm::ivec3& pos, std::vector<Vertex>& vertices)
{
	// y are negative because opengl y axis is opposite way
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
		xPosActive = m_pBlocks[pos.x + 1][pos.y][pos.z].IsActive();
	if (pos.x - 1 >= 0)
		xNegActive = m_pBlocks[pos.x - 1][pos.y][pos.z].IsActive();
	if (pos.y + 1 < CHUNK_SIZE)
		yPosActive = m_pBlocks[pos.x][pos.y + 1][pos.z].IsActive();
	if (pos.y - 1 >= 0)
		yNegActive = m_pBlocks[pos.x][pos.y - 1][pos.z].IsActive();
	if (pos.z + 1 < CHUNK_SIZE)
		zPosActive = m_pBlocks[pos.x][pos.y][pos.z + 1].IsActive();
	if (pos.z - 1 >= 0)
		zNegActive = m_pBlocks[pos.x][pos.y][pos.z - 1].IsActive();

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

	// back face
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


