#pragma once

#include <iostream>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Components/Model.h"

class AssimpImportManager {
    public:
    static glm::mat4 ConvertMatrixToGLM(const aiMatrix4x4& from);

    static ModelNode ProcessNode(const aiNode* node, const aiScene* scene);
    static Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);

    static Model* LoadModel(const std::string& path);
};