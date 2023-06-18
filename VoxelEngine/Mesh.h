//
// Created by river on 6/9/2023.
//

#ifndef VOXELENGINE_MESH_H
#define VOXELENGINE_MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

#define MAX_BONE_INFLUENCE 4

struct ModelVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<ModelVertex>& vertices,
         std::vector<unsigned int>& indices,
         std::vector<Texture>& texture);
    void Draw(Shader& shader);

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

#endif //VOXELENGINE_MESH_H
