#include "Core/AssetManager.h"

void AssetManager::Init()
{
    //SHADERS
    fs::path shadersCodeFolder = fs::path(FileSystem::GetAssetPath("User")) / "ShadersCode";
    if (!fs::exists(shadersCodeFolder))
        fs::create_directories(shadersCodeFolder);

    fs::path defaultShadersCodePath = shadersCodeFolder / "DefaultVertex.vert";
    if (!fs::exists(defaultShadersCodePath))
        CreateNewVertexShaderCode("DefaultVertex");

    defaultShadersCodePath = shadersCodeFolder / "DefaultFragment.frag";
    if (!fs::exists(defaultShadersCodePath))
        CreateNewFragmentShaderCode("DefaultFragment");

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

void AssetManager::LoadAll()
{
    //SHADERS
    LoadAllTextures();
    LoadAllShaders();
    LoadAllMaterials();
    LoadAllModels();
}

void AssetManager::LoadAllShaders()
{
    fs::path shadersCodePath = fs::path(FileSystem::GetAssetPath("User", "Shaders"));
    if (fs::exists(shadersCodePath))
    {
        for (const auto& entry : fs::directory_iterator(shadersCodePath))
        {
            std::string ext = entry.path().extension();
            if (entry.is_regular_file() && ext == ".shader")
                LoadShader(entry.path().stem().string());
        }
    }
}

void AssetManager::LoadAllTextures()
{
    fs::path shadersCodePath = fs::path(FileSystem::GetAssetPath("User", "Textures"));
    if (fs::exists(shadersCodePath))
    {
        for (const auto& entry : fs::directory_iterator(shadersCodePath))
        {
            std::string ext = entry.path().extension();
            if (entry.is_regular_file() && (ext == ".png" || ext == ".jpg" || ext == ".jpeg"))
                LoadTexture(entry.path().stem().string());
        }
    }
}

void AssetManager::LoadAllMaterials()
{
    fs::path shadersCodePath = fs::path(FileSystem::GetAssetPath("User", "Materials"));
    if (fs::exists(shadersCodePath))
    {
        for (const auto& entry : fs::directory_iterator(shadersCodePath))
        {
            std::string ext = entry.path().extension();
            if (entry.is_regular_file() && ext == ".mat")
                LoadMaterial(entry.path().stem().string());
        }
    }
}

void AssetManager::LoadAllModels()
{
    fs::path shadersCodePath = fs::path(FileSystem::GetAssetPath("User", "Models"));
    if (fs::exists(shadersCodePath))
    {
        for (const auto& entry : fs::directory_iterator(shadersCodePath))
        {
            std::string ext = entry.path().extension();
            if (entry.is_regular_file() && (ext == ".obj" || ext == ".fbx"))
                LoadModel(entry.path().stem().string());
        }
    }
}

Texture* AssetManager::GetTexture(const std::string& filename)
{
    if (texturesMap.find(filename) == texturesMap.end())
    {
        std::cerr << "[ASSET MANAGER] texture not found: " << filename << std::endl;
        return nullptr;
    }

    return texturesMap[filename];
}

Model* AssetManager::GetModel(const std::string& filename)
{
    if (modelsMap.find(filename) == modelsMap.end())
    {
        std::cerr << "[ASSET MANAGER] model not found: " << filename << std::endl;
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
            std::cerr << "[ASSET MANAGER] Model not found: " << filename << " (use .obj, .fbx, .gltf)" << std::endl;
            return;
        }
    }

    std::string key = inputPath.stem().string();
    Model* loadedModel = AssimpImportManager::LoadModel(finalPath.string());
    
    if (loadedModel)
    {
        modelsMap[key] = loadedModel;
        std::cout << "[ASSET MANAGER] Model loaded: " << key << std::endl;
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
            std::cerr << "[ASSET MANAGER] Texture not found: " << filename << " (use .png, .jpg, .jpeg)" << std::endl;
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
        std::cout << "[ASSET MANAGER] Created new material" << pathString << std::endl;
    }
    else
    {
        std::cerr << "[ASSET MANAGER] Can't load " << pathString << std::endl;
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
        std::cerr << "[ASSET MANAGER] Material file not found: " << finalPath.string() << std::endl;
        return;
    }

    std::ifstream file(finalPath);
    if (!file.is_open())
    {
        std::cerr << "[ASSET MANAGER] Cannot open " << finalPath.string() << std::endl;
        return;
    }

    json j;
    file >> j;
    file.close();

    Material* newMat = new Material(nullptr, nullptr, nullptr, 0);
    newMat->SetName(key);
    newMat->FromJson(j);

    materialsMap[key] = newMat;
    std::cout << "[ASSET MANAGER] Material loaded: " << key << std::endl;
}

Material* AssetManager::GetMaterial(const std::string& filename)
{
    if (materialsMap.find(filename) == materialsMap.end())
    {
        std::cerr << "[ASSET MANAGER] material not found: " << filename << std::endl;
        return nullptr;
    }

    return materialsMap[filename];
}

void AssetManager::CreateNewVertexShaderCode(const std::string filename)
{
    fs::path dir = fs::path(FileSystem::GetAssetPath("User")) / "ShadersCode";
    std::string extension = ".vert";

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

    std::ofstream file(finalPath);
    if (file.is_open())
    {
        file << R"(#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

out vec2 fragTextCoords;
out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
    //M.V.P
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1);
    fragTextCoords = textCoords;
    fragNormal = normalize(normalMatrix * normal);
    fragWorldPos = vec3(modelMatrix * vec4(pos, 1.0));
})";
        file.close();
        std::cout << "[ASSET MANAGER] Created new Vertex Shader " << finalPath.string() << std::endl;
    }
    else
        std::cerr << "[ASSET MANAGER] Cannot create vertex Shader at " << finalPath.string() << std::endl;
}

void AssetManager::CreateNewFragmentShaderCode(const std::string filename)
{
    fs::path dir = fs::path(FileSystem::GetAssetPath("User")) / "ShadersCode";
    std::string extension = ".frag";

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

    std::ofstream file(finalPath);
    if (file.is_open())
    {
        file << R"(#version 330 core

struct Material {
    vec4 tintColor;
    float shininess;
    int hasColorMap;
    int hasSpecularMap;
};

out vec4 FragColor;

in vec3 fragWorldPos;
in vec2 fragTextCoords;
in vec3 fragNormal;

uniform vec3 viewPos;

uniform Material material;
uniform sampler2D colorMap;
uniform sampler2D specularMap;

void main()
{
    //===LIGHTNING===

    //variables
    vec4 albedo = material.hasColorMap == 1 ? texture(colorMap, fragTextCoords) * material.tintColor : material.tintColor;
    float specularIntensity = material.hasSpecularMap == 1 ? texture(specularMap, fragTextCoords).r : 1.0;
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float Kamb = 0.1;

    vec3 N = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.5));
    vec3 L = -lightDir;
    vec3 V = normalize(viewPos - fragWorldPos);
    vec3 H = normalize(V + L);

    //ambient
    vec3 ambientLight = Kamb * albedo.rgb;

    //diffuse
    float diff = max(0.0, dot(N,L));
    vec3 diffLight = albedo.rgb * diff;

    //specular
    float spec = pow(max(0.0, dot(H, N)), material.shininess);
    vec3 specLight = lightColor * (spec * specularIntensity);

    vec3 finalColor = diffLight + ambientLight + specLight;
    FragColor = vec4(finalColor, 1.0);
})";
        file.close();
        std::cout << "[ASSET MANAGER] Created new Vertex Shader " << finalPath.string() << std::endl;
    }
    else
        std::cerr << "[ASSET MANAGER] Cannot create vertex Shader at " << finalPath.string() << std::endl;
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

    Shader* newShader = new Shader("", "", finalName);
    shadersMap[finalName] = newShader;

    json shaderJson = newShader->ToJson();
    std::ofstream file(finalPath);
    if (file.is_open())
    {
        file << shaderJson.dump(4);
        file.close();
        std::cout << "[ASSET MANAGER] Created new shader" << pathString << std::endl;
    }
    else
    {
        std::cerr << "[ASSET MANAGER] Can't load " << pathString << std::endl;
    }
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
        std::cerr << "[ASSET MANAGER] Shader file not found: " << finalPath.string() << std::endl;
        return;
    }

    std::ifstream file(finalPath);
    if (!file.is_open())
    {
        std::cerr << "[ASSET MANAGER] Cannot open " << finalPath.string() << std::endl;
        return;
    }

    json j;
    file >> j;
    file.close();

    Shader* newShader = new Shader(); 
    newShader->SetName(key);
    newShader->FromJson(j);

    shadersMap[key] = newShader;
    std::cout << "[ASSET MANAGER] Shader loaded: " << key << std::endl;
}

Shader* AssetManager::GetShader(const std::string& filename)
{
    if (shadersMap.find(filename) == shadersMap.end())
    {
        std::cerr << "[ASSET MANAGER] shader not found: " << filename << std::endl;
        return nullptr;
    }

    return shadersMap[filename];
}