#include "SceneManager.h"
#include "IO/FileSystem.h"
#include "Core/Scene.h"
#include "IO/Console.h"
#include "UI/SelectionManager.h"
#include "Utils/Utils.h"

void SceneManager::Init()
{
    s_activeScene = new Scene("Scene");
}


void SceneManager::SaveCurrentSceneAutosave()
{
    if (!s_activeScene) return;

    std::string sceneName = "Autosave";
    fs::path finalPath = fs::path(FileSystem::GetAssetPath("User", "Scenes")) / (sceneName + ".scene");

    json sceneData = s_activeScene->ToJson();

    std::ofstream file(finalPath);
    if (file.is_open())
    {
        file << sceneData.dump(4);
        file.close();
        Console::LogInfo("Scene saved at " + finalPath.string(), LOG_CATEGORY::SCENE);
    }
    else
        Console::LogError("Error saving scene at " + finalPath.string(), LOG_CATEGORY::SCENE);
}

void SceneManager::SaveCurrentScene()
{
    if (!s_activeScene) return;

    std::string sceneName = Utils::GetFileIncrementalName(s_activeScene->GetName(), ".scene", "Scenes");
    fs::path finalPath = fs::path(FileSystem::GetAssetPath("User", "Scenes")) / (sceneName + ".scene");

    json sceneData = s_activeScene->ToJson();

    std::ofstream file(finalPath);
    if (file.is_open())
    {
        file << sceneData.dump(4);
        file.close();
        Console::LogInfo("Scene saved at " + finalPath.string(), LOG_CATEGORY::SCENE);
    }
    else
        Console::LogError("Error saving scene at " + finalPath.string(), LOG_CATEGORY::SCENE);
}

void SceneManager::LoadScene(const std::string& filename, bool saveCurrent)
{
    fs::path scenesFolder = fs::path(FileSystem::GetAssetPath("User")) / "Scenes";

    fs::path targetPath = scenesFolder / filename;
    if (!targetPath.has_extension())
        targetPath += ".scene";

    if (!fs::exists(targetPath))
    {
        Console::LogError("Can't find " + targetPath.string(), LOG_CATEGORY::SCENE);
        return;
    }

    if (s_activeScene && saveCurrent)
        SaveCurrentScene();

    SelectionManager::Deselect();
    std::ifstream file(targetPath);
    json sceneData;
    file >> sceneData;
    file.close();

    Scene* newScene = new Scene();
    newScene->FromJson(sceneData);

    if (s_activeScene)
        delete s_activeScene;

    s_activeScene = newScene;
    Console::LogInfo("Scene Loaded: " + s_activeScene->GetName(), LOG_CATEGORY::SCENE);
}