#pragma once
#include "Core/SceneObject.h"
#include <filesystem>
namespace fs = std::filesystem;

class UIFileSystem {
    public:
    static void Draw(SceneObject* m_selectedSceneObj);

    private:
    inline static fs::path m_modelToDelete = "";
    inline static fs::path m_textureToDelete = "";
    inline static fs::path m_shaderToDelete = "";
    inline static fs::path m_materialToDelete = "";
    inline static fs::path m_sceneToDelete = "";
    inline static fs::path m_presetToDelete = "";

    static void ProcessOperations();
};