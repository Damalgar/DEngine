#pragma once

#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <string>

#include "Tools/drawUtils.h"
#include "Icons/IconsLoader.h"
#include "Fonts/FontsLoader.h"
#include "Utils/Utils.h"
#include "IO/FileSystem.h"
#include "Graphics/Camera.h"
#include "Render/Framebuffer.h"
#include "Core/Scene.h"
#include "Core/AssetManager.h"
#include "Render/Shader.h"
#include "Core/SceneManager.h"

class SceneObject;
class Editor;

class Application {
    public:

    inline static Application* Instance = nullptr;

    Application();
    int Init(int argc, char* argv0);
    void MainLoop();

    const std::vector<SceneObject*>& GetSceneObjects() const;
    Camera* GetCamera() { return m_camera; }

    void AddModelToScene(const std::string &filename, SceneObject* mountPoint = nullptr);

    private:
    GLFWwindow* m_window = nullptr;
    Camera* m_camera = nullptr;
    Editor* m_editor = nullptr;
    Framebuffer* m_sceneBuffer = nullptr;

    glm::mat4 m_viewMatrix{1.0f};
    glm::mat4 m_projectionMatrix{1.0f};

    void SetupDefaults();
    void SetupImGuiStyle();
    void SetupImGuizmoStyle();
    
    void StartRenderCycle();
    void EndRenderCycle();
    GLuint GenerateFrameBuffer();
};