#pragma once
#include <string>

class Scene;

class SceneManager {
    public:

    static void Init();
    static Scene* GetActiveScene() { return s_activeScene; }
    static void SaveCurrentScene();
    static void LoadScene(const std::string& filename, bool saveCurrent = false);

    private:
    inline static Scene* s_activeScene = nullptr;
};