#include "Core/AssetManager.h"
#include "Graphics/PrimitiveFactory.h"
#include "IO/Console.h"
#include "Render/Shader.h"
#include "Render/Material.h"
#include "Render/Model.h"
#include "Render/Texture.h"
#include "Core/SceneObject.h"

void AssetManager::Init()
{
    fs::path defaultShaderPath = fs::path(FileSystem::GetAssetPath("User", "Shaders")) / "DefaultShader.shader";
    if (!fs::exists(defaultShaderPath))
        CreateNewShader("DefaultShader");

    //MATERIALS
    fs::path materialsFolder = fs::path(FileSystem::GetAssetPath("User", "Materials"));
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

bool AssetManager::DeleteTexture(const std::string& filename)
{
    if (texturesMap.find(filename) == texturesMap.end())
    {
        Console::LogError("texture not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    Texture* texture = texturesMap[filename];

    for (auto& it : materialsMap)
    {
        Material* mat = it.second;
        if (mat->GetColorMap() == texture)
            mat->SetColorMap(nullptr);
        if (mat->GetSpecularMap() == texture)
            mat->SetSpecularMap(nullptr);
    }

    if (texturesMap.erase(filename) == 0)
    {
        Console::LogError("texture not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    delete texture;
    return FileSystem::DeleteAsset(filename, ".png", "Textures") || FileSystem::DeleteAsset(filename, ".jpg", "Textures") || 
            FileSystem::DeleteAsset(filename, ".jpeg", "Textures");
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

bool AssetManager::DeleteModel(const std::string& filename)
{
    if (modelsMap.find(filename) == modelsMap.end())
    {
        Console::LogError("model not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    Model* model = modelsMap[filename];
    if (SceneManager::GetActiveScene())
        SceneManager::GetActiveScene()->OnModelDeleted(model);

    if (modelsMap.erase(filename) == 0)
    {
        Console::LogError("model not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    delete model;
    return FileSystem::DeleteAsset(filename, ".fbx", "Models") || FileSystem::DeleteAsset(filename, ".obj", "Models");
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

bool AssetManager::DeleteMaterial(const std::string& filename)
{
    if (filename == "DefaultMaterial")
    {
        Console::LogError("Can't delete default material", LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    if (materialsMap.find(filename) == materialsMap.end())
    {
        Console::LogError("material not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    Material* mat = materialsMap[filename];
    if (SceneManager::GetActiveScene())
        SceneManager::GetActiveScene()->OnMaterialDeleted(mat);

    if (materialsMap.erase(filename) == 0)
    {
        Console::LogError("material not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    delete mat;
    return FileSystem::DeleteAsset(filename, ".mat", "Materials");
}

bool AssetManager::RenameMaterial(const std::string& oldFilename, const std::string& newFilename)
{
    if (oldFilename == "DefaultMaterial" || newFilename == "DefaultMaterial")
        return false;
        
    if (materialsMap.find(oldFilename) == materialsMap.end())
    {
        Console::LogError("material not found for rename: " + oldFilename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    if (!FileSystem::RenameAsset(oldFilename, newFilename, ".mat", "Materials"))
        return false;

    Material* mat = materialsMap[oldFilename];
    materialsMap[newFilename] = std::move(mat);
    materialsMap.erase(oldFilename);
    mat->SetName(newFilename);

    return true;
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

bool AssetManager::DeleteShader(const std::string& filename)
{
    if (shadersMap.find(filename) == shadersMap.end())
    {
        Console::LogError("shader not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    Shader* shader = shadersMap[filename];
    Shader* defShader = shadersMap.find("DefaultShader") != shadersMap.end() ? shadersMap["DefaultShader"] : nullptr;

    for (auto& it : materialsMap)
    {
        Material* mat = it.second;
        if (mat->GetShader() == shader)
            mat->SetShader(defShader);
    }

    if (shadersMap.erase(filename) == 0)
    {
        Console::LogError("shader not found for delete: " + filename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    delete shader;
    return FileSystem::DeleteAsset(filename, ".shader", "Shaders");
}

bool AssetManager::RenameShader(const std::string& oldFilename, const std::string& newFilename)
{
    if (oldFilename == "DefaultShader" || newFilename == "DefaultShader")
        return false;
        
    if (shadersMap.find(oldFilename) == shadersMap.end())
    {
        Console::LogError("shader not found for rename: " + oldFilename, LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    if (!FileSystem::RenameAsset(oldFilename, newFilename, ".shader", "Shaders"))
        return false;

    Shader* shader = shadersMap[oldFilename];
    shadersMap[newFilename] = std::move(shader);
    shadersMap.erase(oldFilename);
    shader->SetName(newFilename);

    //Materials already have a pointer to Shader, so there's no need to update them
    return true;
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

bool AssetManager::CreateNewPreset(SceneObject* rootObj, const std::string& name)
{
    if (rootObj == nullptr)
        return false;

    std::string filename = Utils::GetFileIncrementalName(name, ".preset", "Presets");
    fs::path finalPath = fs::path(FileSystem::GetAssetPath("User", "Presets")) / (filename + ".preset");
    
    json j;
    j["name"] = filename;
    
    json objectsJson = json::array();
    std::vector<SceneObject*> allObjects;

    Scene* activeScene = SceneManager::GetActiveScene();
    if (activeScene == nullptr)
        return false;

    activeScene->CollectHierarchy(rootObj, allObjects);

    for (SceneObject* obj : allObjects)
    {
        json objJson = obj->ToJson();
        if (obj == rootObj)
            j["parent_id"] = 0;

            objectsJson.push_back(objJson);
    }

    j["sceneObjects"] = objectsJson;

    std::ofstream out(finalPath);
    if (out.is_open()) {
        out << j.dump(4);
        out.close();
        Console::LogInfo("Saved preset at " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
    } else {
        Console::LogError("Error saving preset at 0" + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
        return false;
    }
    return true;
}

SceneObject* AssetManager::InstantiatePreset(const std::string& name, SceneObject* targetMountPoint)
{
    Scene* activeScene = SceneManager::GetActiveScene();

    if (activeScene == nullptr)
        return nullptr;

    fs::path presetsFolder = fs::path(FileSystem::GetAssetPath("User")) / "Presets";
    fs::path targetPath = presetsFolder / name;

    if (!targetPath.has_extension())
        targetPath += ".preset";

    if (!fs::exists(targetPath))
    {
        Console::LogError("Can't find " + targetPath.string(), LOG_CATEGORY::ASSETMANAGER);
        return nullptr;
    }

    std::ifstream file(targetPath);
    if (!file.is_open())
    {
        Console::LogError("Can't open file " + targetPath.string(), LOG_CATEGORY::ASSETMANAGER);
        return nullptr;
    }

    json presetJson;
    file >> presetJson;

    //(old_id, new_id), remapping to avoid ID dupes
    std::unordered_map<uint64_t, uint64_t> idMap;
    std::vector<std::pair<SceneObject*, json>> loadedObjects;

    for (json& j : presetJson["sceneObjects"])
    {
        uint64_t oldId = j["id"];
        SceneObject* newObj = new SceneObject();
        idMap[oldId] = newObj->GetID();
        loadedObjects.push_back({newObj, j});
    }

    SceneObject* presetRoot = nullptr;

    std::vector<SceneObject*> objectsToAdd;

    for (std::pair<SceneObject*, json> pair : loadedObjects)
    {
        SceneObject* obj = pair.first;
        json j = pair.second;

        uint64_t oldId = j["id"];
        j["id"] = idMap[oldId];

        uint64_t oldParentId = j["parent_id"];

        if (oldParentId == 0) //root
        {
            presetRoot = obj;

            if (targetMountPoint)
                j["parent_id"] = targetMountPoint->GetID();
        } else {
            if (idMap.find(oldParentId) != idMap.end())
            {
                j["parent_id"] = idMap[oldParentId];
            }
        }

        obj->FromJson(j);
        activeScene->AddObject(obj);
    }

    for (auto& pair : loadedObjects)
    {
        SceneObject* obj = pair.first;
        uint64_t parentId = obj->GetParentID();
        
        if (parentId != 0)
        {
            SceneObject* parentObj = activeScene->GetSceneObjectByID(parentId);
            if (parentObj)
                obj->transform.SetParent(&parentObj->transform, false); 
            else
                Console::LogError("Parent with id " + std::to_string(parentId) + " not found for " + obj->name, LOG_CATEGORY::ASSETMANAGER);
        }
    }

    if (presetRoot)
        presetRoot->transform.GetModelMatrix();

    return presetRoot;
}