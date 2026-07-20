#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>
#include "Components/Model.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "IO/FileSystem.h"
#include "Utils/Utils.h"
#include "Core/Application.h"
#include "Render/Shader.h"
#include "Core/AssimpImportManager.h"

namespace fs = std::filesystem;

class AssetManager {
    public:

    static void Init();

    static Texture* GetTexture(const std::string& filename);
    static Model* GetModel(const std::string& filename);

    static void LoadModel(const std::string& filename);
    static void LoadTexture(const std::string& filename);

    static void LoadAllShaders();
    static void LoadAllMaterials();
    static void LoadAllTextures();
    static void LoadAllModels();
    static void LoadAll();

    static void Clear();

    static void CreateNewMaterial(const std::string filename = "Material");
    static Material* GetMaterial(const std::string& filename);
    static void LoadMaterial(const std::string& filename);

    static void CreateNewVertexShaderCode(const std::string filename = "ShaderV");
    static void CreateNewFragmentShaderCode(const std::string filename = "ShaderF");

    static void CreateNewShader(const std::string filename = "Shader");
    static Shader* GetShader(const std::string& filename);
    static void LoadShader(const std::string& filename);

    private:
    inline static std::unordered_map<std::string, Texture*> texturesMap;
    inline static std::unordered_map<std::string, Model*> modelsMap;
    inline static std::unordered_map<std::string, Material*> materialsMap;
    inline static std::unordered_map<std::string, Shader*> shadersMap;
};