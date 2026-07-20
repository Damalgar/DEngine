#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <glad/glad.h>
#include "Core/SceneObject.h"
#include "Fonts/FontsLoader.h"
#include "Core/Scene.h"
#include "Tools/FsDialog.h"

class Editor
{
    public:
    Editor(GLuint sceneTextureID);
    
    void DrawPanels();
    ImVec2 GetViewportSize() const { return m_viewportSize; }

    private:
    GLuint m_sceneTextureID;
    ImVec2 m_viewportSize = ImVec2(1920.0f, 1080.0f);
    SceneObject* m_selectedSceneObj = nullptr;

    SceneObject* m_draggedNodeToMove = nullptr;
    SceneObject* m_targetParentNode = nullptr;

    void DrawHierarchyPanel();
    void DrawScenePanel();
    void DrawFileSystemPanel();
    void DrawInspectorPanel();
    void DrawHierarchyNode(SceneObject* obj);
};