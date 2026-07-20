#include "SceneManager.h"
#include "IO/FileSystem.h"
#include "Core/Scene.h"

void SceneManager::Init()
{
    s_activeScene = new Scene("Scene");
}

void SceneManager::SaveCurrentScene()
{
    if (!s_activeScene) return;

    fs::path scenesFolder = fs::path(FileSystem::GetAssetPath("User")) / "Scenes";
    fs::path finalPath = scenesFolder / (s_activeScene->GetName() + ".scene");

    json sceneData = s_activeScene->ToJson();

    std::ofstream file(finalPath);
    if (file.is_open())
    {
        file << sceneData.dump(4);
        file.close();
        std::cout << "[SCENE MANAGER] Scene saved in: " << finalPath.string() << std::endl;
    }
    else
    {
        std::cerr << "[SCENE MANAGER] Error saving scene" << std::endl;
    }
}

void SceneManager::LoadScene(const std::string& filename, bool saveCurrent)
{
    fs::path scenesFolder = fs::path(FileSystem::GetAssetPath("User")) / "Scenes";

    fs::path targetPath = scenesFolder / filename;
    if (!targetPath.has_extension())
        targetPath += ".scene";

    if (!fs::exists(targetPath))
    {
        std::cerr << "[SCENE MANAGER] Can't find " << targetPath.string() << std::endl;
        return;
    }

    if (s_activeScene && saveCurrent)
        SaveCurrentScene();

    std::ifstream file(targetPath);
    json sceneData;
    file >> sceneData;
    file.close();

    Scene* newScene = new Scene();
    newScene->FromJson(sceneData);

    if (s_activeScene)
        delete s_activeScene;

    s_activeScene = newScene;
    std::cout << "[SCENE MANAGER] Loaded: " << s_activeScene->GetName() << std::endl;
}