#include "Core/AssimpImportManager.h"

glm::mat4 AssimpImportManager::ConvertMatrixToGLM(const aiMatrix4x4& from)
{
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

ModelNode AssimpImportManager::ProcessNode(const aiNode* node, const aiScene* scene)
{
    ModelNode modelNode;
    modelNode.name = node->mName.C_Str();
    modelNode.localTransform = ConvertMatrixToGLM(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
        modelNode.meshIndices.push_back(node->mMeshes[i]);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        modelNode.children.push_back(ProcessNode(node->mChildren[i], scene));

    return modelNode;
}

Mesh AssimpImportManager::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    BoundingBox localBox;

    for (uint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.pos = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasNormals())
            vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0]) //don't use mesh->hasTextureCoords(channel)
        {
            vertex.texCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            vertex.tangent = vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.bitangent = vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        } else {
            vertex.texCoords = vec2(0.0f);
        }

        vertex.color = vec4(1.0f);
        vertices.push_back(vertex);

        localBox.Expand(vertex.pos);
    }

    for (uint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (uint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Mesh meshProcessed = Mesh(vertices, indices);
    meshProcessed.SetBoundingBox(localBox);

    return meshProcessed;
}

Model* AssimpImportManager::LoadModel(const std::string& path)
{
    Assimp::Importer importer;
    
    unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "[ASSIMP] Loading error:\n" << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::string dir = std::filesystem::path(path).parent_path().string();
    std::string name = std::filesystem::path(path).stem().string();

    std::vector<Mesh> allMeshes;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        allMeshes.push_back(ProcessMesh(scene->mMeshes[i], scene));
    }

    ModelNode rootNode = ProcessNode(scene->mRootNode, scene);

    return new Model(dir, name, allMeshes, rootNode);
}