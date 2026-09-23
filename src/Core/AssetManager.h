#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>
#include "IO/FileSystem.h"
#include "Utils/Utils.h"
#include "Core/Application.h"
#include "Core/AssimpImportManager.h"

class Material;
class Shader;
class Texture;
class Model;
class SceneObject;

namespace fs = std::filesystem;

class AssetManager {
    public:

    static void Init();

    static void LoadTexture(const std::string& filename);
    static Texture* GetTexture(const std::string& filename);
    static bool DeleteTexture(const std::string& filename);

    static void LoadModel(const std::string& filename);
    static Model* GetModel(const std::string& filename);
    static bool DeleteModel(const std::string& filename);

    static void LoadAllShaders();
    static void LoadAllMaterials();
    static void LoadAllTextures();
    static void LoadAllModels();
    static void LoadAll();

    static void SaveAll();
    static void SaveAllMaterials();

    static void Clear();

    static void CreateNewMaterial(const std::string filename = "Material");
    static Material* GetMaterial(const std::string& filename);
    static void LoadMaterial(const std::string& filename);
    static bool DeleteMaterial(const std::string& filename);
    static bool RenameMaterial(const std::string& oldFilename, const std::string& newFilename);

    static void CreateNewShader(const std::string filename = "Shader");
    static Shader* GetShader(const std::string& filename);
    static void LoadShader(const std::string& filename);
    static bool DeleteShader(const std::string& filename);
    static bool RenameShader(const std::string& oldFilename, const std::string& newFilename);

    static bool CreateNewPreset(SceneObject* rootObj, const std::string& name);
    static SceneObject* InstantiatePreset(const std::string& name, SceneObject* targetMountPoint = nullptr);

    private:
    inline static std::unordered_map<std::string, Texture*> texturesMap;
    inline static std::unordered_map<std::string, Model*> modelsMap;
    inline static std::unordered_map<std::string, Material*> materialsMap;
    inline static std::unordered_map<std::string, Shader*> shadersMap;

    static void InitProceduralModels();
};