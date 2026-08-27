#include "Core/AssetManager.h"
#include "Graphics/PrimitiveFactory.h"
#include "IO/Console.h"
#include "Render/Shader.h"
#include "Render/Material.h"
#include "Render/Model.h"
#include "Render/Texture.h"

void AssetManager::Init()
{
    //SHADERS
    fs::path shadersCodeFolder = fs::path(FileSystem::GetAssetPath("User")) / "ShadersCode";
    if (!fs::exists(shadersCodeFolder))
        fs::create_directories(shadersCodeFolder);

    fs::path defaultShaderPath = fs::path(FileSystem::GetAssetPath("User")) / "Shaders" / "DefaultShader.shader";
    if (!fs::exists(defaultShaderPath))
        CreateNewShader("DefaultShader");

    //MATERIALS
    fs::path materialsFolder = fs::path(FileSystem::GetAssetPath("User")) / "Materials";
    if (!fs::exists(materialsFolder))
        fs::create_directories(materialsFolder);

    fs::path defaultMaterialPath = materialsFolder / "DefaultMaterial.mat";
    if (!fs::exists(defaultMaterialPath))
        CreateNewMaterial("DefaultMaterial");

    LoadAll();

    std::string finalPath = materialsFolder.string();
}

void AssetManager::InitProceduralModels()
{
    if (modelsMap.find("Primitive_Cube") == modelsMap.end())
        modelsMap["Primitive_Cube"] = PrimitiveFactory::CreateCubeModel();
}

void AssetManager::LoadAll()
{
    //SHADERS
    LoadAllTextures();
    LoadAllShaders();
    LoadAllMaterials();
    InitProceduralModels();
    LoadAllModels();
}

void AssetManager::LoadAllShaders()
{
    fs::path shadersCodePath = fs::path(FileSystem::GetAssetPath("User", "Shaders"));
    if (fs::exists(shadersCodePath))
    {
        for (const auto& entry : fs::directory_iterator(shadersCodePath))
        {
            std::string ext = entry.path().extension().string();
            if (entry.is_regular_file() && ext == ".shader")
                LoadShader(entry.path().stem().string());
        }
    }
}

void AssetManager::LoadAllTextures()
{
    fs::path texturesPath = fs::path(FileSystem::GetAssetPath("User", "Textures"));
    if (fs::exists(texturesPath))
    {
        for (const auto& entry : fs::directory_iterator(texturesPath))
        {
            std::string ext = entry.path().extension().string();
            if (entry.is_regular_file() && (ext == ".png" || ext == ".jpg" || ext == ".jpeg"))
                LoadTexture(entry.path().stem().string());
        }
    }
}

void AssetManager::LoadAllMaterials()
{
    fs::path materialsPath = fs::path(FileSystem::GetAssetPath("User", "Materials"));
    if (fs::exists(materialsPath))
    {
        for (const auto& entry : fs::directory_iterator(materialsPath))
        {
            std::string ext = entry.path().extension().string();
            if (entry.is_regular_file() && ext == ".mat")
                LoadMaterial(entry.path().stem().string());
        }
    }
}

void AssetManager::LoadAllModels()
{
    fs::path modelsPath = fs::path(FileSystem::GetAssetPath("User", "Models"));
    if (fs::exists(modelsPath))
    {
        for (const auto& entry : fs::directory_iterator(modelsPath))
        {
            std::string ext = entry.path().extension().string();
            if (entry.is_regular_file() && (ext == ".obj" || ext == ".fbx"))
                LoadModel(entry.path().stem().string());
        }
    }
}

Texture* AssetManager::GetTexture(const std::string& filename)
{
    if (texturesMap.find(filename) == texturesMap.end())
    {
        Console::LogError("texture not found: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return nullptr;
    }

    return texturesMap[filename];
}

Model* AssetManager::GetModel(const std::string& filename)
{
    if (modelsMap.find(filename) == modelsMap.end())
    {
        Console::LogError("model not found: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return nullptr;
    }

    return modelsMap[filename];
}

void AssetManager::LoadModel(const std::string& filename)
{
    fs::path modelsDir = fs::path(FileSystem::GetAssetPath("User")) / "Models";
    fs::path inputPath(filename);
    fs::path finalPath;
    
    if (inputPath.has_extension())
        finalPath = modelsDir / filename;
    else
    {
        std::vector<std::string> supportedExts = {".obj", ".fbx", ".gltf"};
        bool found = false;
        
        for (const auto& ext : supportedExts)
        {
            fs::path testPath = modelsDir / (filename + ext);
            if (fs::exists(testPath))
            {
                finalPath = testPath;
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            Console::LogError("Model not found: " + filename + " (use .obj, .fbx, .gltf)", LOG_CATEGORY::ASSETMANAGER);
            return;
        }
    }

    std::string key = inputPath.stem().string();
    Model* loadedModel = AssimpImportManager::LoadModel(finalPath.string());
    
    if (loadedModel)
    {
        modelsMap[key] = loadedModel;
        Console::Log("Model loaded: " + key, LOG_LEVEL::INFO, LOG_CATEGORY::ASSETMANAGER);
    }
}

void AssetManager::LoadTexture(const std::string& filename)
{
    fs::path texturesDir = fs::path(FileSystem::GetAssetPath("User")) / "Textures";
    fs::path inputPath(filename);
    fs::path finalPath;
    
    if (inputPath.has_extension())
        finalPath = texturesDir / filename;
    else
    {
        std::vector<std::string> supportedExts = {".png", ".jpg", ".jpeg"};
        bool found = false;
        
        for (const auto& ext : supportedExts)
        {
            fs::path testPath = texturesDir / (filename + ext);
            if (fs::exists(testPath))
            {
                finalPath = testPath;
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            Console::LogError("Texture not found: " + filename + " (use .png, .jpg, .jpeg)", LOG_CATEGORY::ASSETMANAGER);
            return;
        }
    }

    std::string key = inputPath.stem().string();
    texturesMap[key] = new Texture(finalPath.string());
}

void AssetManager::Clear()
{
    for(auto& it : texturesMap) delete it.second;
    for(auto& it : modelsMap) delete it.second;
    for(auto& it : materialsMap) delete it.second;
    for(auto& it : shadersMap) delete it.second;

    texturesMap.clear();
    modelsMap.clear();
    materialsMap.clear();
    shadersMap.clear();
}

void AssetManager::CreateNewMaterial(const std::string filename)
{
    fs::path dir = fs::path(FileSystem::GetAssetPath("User")) / "Materials";
    std::string extension = ".mat";

    if (!fs::exists(dir))
        fs::create_directories(dir);

    fs::path finalPath = dir / (filename + extension);
    int counter = 0;
    while (fs::exists(finalPath))
    {
        counter++;
        std::ostringstream oss;
        oss << filename << "_" << std::setw(3) << std::setfill('0') << counter;
        
        finalPath = dir / (oss.str() + extension);
    }

    std::string pathString = finalPath.string();
    Material* newMat = new Material(GetShader("DefaultShader"), nullptr, nullptr, 32.0f, glm::vec4(1.0f));
    std::string finalName = finalPath.stem().string();
    materialsMap[finalName] = newMat;
    newMat->SetName(finalName);

    json matJson = newMat->ToJson();
    std::ofstream file(finalPath);
    if (file.is_open())
    {
        file << matJson.dump(4);
        file.close();
        Console::Log("Created new material" + pathString, LOG_LEVEL::INFO, LOG_CATEGORY::ASSETMANAGER);
    }
    else
    {
        Console::Log("Can't load" + pathString, LOG_LEVEL::WARNING, LOG_CATEGORY::ASSETMANAGER);
    }
}

void AssetManager::LoadMaterial(const std::string& filename)
{
    fs::path matDir = fs::path(FileSystem::GetAssetPath("User")) / "Materials";
    fs::path inputPath(filename);
    fs::path finalPath;

    if (inputPath.has_extension())
        finalPath = matDir / filename;
    else
        finalPath = matDir / (filename + ".mat");

    std::string key = finalPath.stem().string();

    if (!fs::exists(finalPath))
    {
        Console::Log("Material file not found: " + finalPath.string(), LOG_LEVEL::WARNING, LOG_CATEGORY::ASSETMANAGER);
        return;
    }

    std::ifstream file(finalPath);
    if (!file.is_open())
    {
        Console::Log("Cannot open: " + finalPath.string(), LOG_LEVEL::WARNING, LOG_CATEGORY::ASSETMANAGER);
        return;
    }

    json j;
    file >> j;
    file.close();

    Material* newMat = new Material(nullptr, nullptr, nullptr, 0);
    newMat->SetName(key);
    newMat->FromJson(j);

    materialsMap[key] = newMat;
    Console::Log("Material loaded: " + key, LOG_LEVEL::INFO, LOG_CATEGORY::ASSETMANAGER);
}

Material* AssetManager::GetMaterial(const std::string& filename)
{
    if (materialsMap.find(filename) == materialsMap.end())
    {
        Console::LogError("Material not found " + filename, LOG_CATEGORY::ASSETMANAGER);
        return nullptr;
    }

    return materialsMap[filename];
}

void AssetManager::CreateNewShader(const std::string filename)
{
    fs::path dir = fs::path(FileSystem::GetAssetPath("User")) / "Shaders";
    std::string extension = ".shader";

    if (!fs::exists(dir))
        fs::create_directories(dir);

    fs::path finalPath = dir / (filename + extension);
    int counter = 0;
    while (fs::exists(finalPath))
    {
        counter++;
        std::ostringstream oss;
        oss << filename << "_" << std::setw(3) << std::setfill('0') << counter;
        
        finalPath = dir / (oss.str() + extension);
    }

    std::string pathString = finalPath.string();
    std::string finalName = finalPath.stem().string();

    Shader* newShader = new Shader(filename, false);
    shadersMap[finalName] = newShader;
}

void AssetManager::LoadShader(const std::string& filename)
{
    fs::path shadersDir = fs::path(FileSystem::GetAssetPath("User")) / "Shaders";
    fs::path inputPath(filename);
    fs::path finalPath;

    if (inputPath.has_extension())
        finalPath = shadersDir / filename;
    else
        finalPath = shadersDir / (filename + ".shader");

    std::string key = finalPath.stem().string();

    if (!fs::exists(finalPath))
    {
        Console::LogError("Shader file not found: " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
        return;
    }

    std::ifstream file(finalPath);
    if (!file.is_open())
    {
        Console::LogError("Cannot open shader: " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
        return;
    }

    Shader* newShader = new Shader(key); 

    shadersMap[key] = newShader;
    Console::LogInfo("Shader loaded " + key, LOG_CATEGORY::ASSETMANAGER);
}

Shader* AssetManager::GetShader(const std::string& filename)
{
    if (shadersMap.find(filename) == shadersMap.end())
    {
        Console::LogError("Shader not found: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return nullptr;
    }

    return shadersMap[filename];
}

void AssetManager::SaveAll()
{
    SaveAllMaterials();
}

void AssetManager::SaveAllMaterials()
{
    fs::path materialsFolder = fs::path(FileSystem::GetAssetPath("User")) / "Materials";
    for(const auto it : materialsMap)
    {
        Material* mat = it.second;
        json j = mat->ToJson();
        fs::path finalPath = materialsFolder / (mat->GetName() + ".mat");
        
        std::ofstream file(finalPath);
        if (file.is_open())
        {
            file << j.dump(4);
            file.close();
        }
        else
            Console::LogError("Error saving material at " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
    }
}