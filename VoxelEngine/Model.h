//
// Created by river on 6/9/2023.
//

#ifndef VOXELENGINE_MODEL_H
#define VOXELENGINE_MODEL_H

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Mesh.h"


class Model
{
public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    Model(std::string const& path, bool gamma = false);
    void Draw(Shader &shader);

private:

    void loadModel(std::string const& path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string name);
};


#endif //VOXELENGINE_MODEL_H
