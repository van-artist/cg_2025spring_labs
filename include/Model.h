#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "Mesh.h"

class Model
{
public:
    std::vector<Mesh> meshes;
    std::string directory;

    Model(const std::string &path);
    void Draw(Shader &shader);

private:
    void loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
    unsigned int TextureFromFile(const char *path, const std::string &directory);
};
