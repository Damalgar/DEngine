#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <glad/glad.h>
#include "Core/SceneObject.h"
#include "Fonts/FontsLoader.h"
#include "Core/Scene.h"
#include "Tools/FsDialog.h"
#include "Components/include_components.h"
#include "Vendor/imgui/ImGuizmo.h"

enum class BOTTOM_PANEL
{
    FILESYSTEM = 0,
    CONSOLE,
    TIMELINE
};

class Editor
{
    public:
    Editor(GLuint sceneTextureID);
    
    void DrawPanels();
    ImVec2 GetViewportSize() const { return m_viewportSize; }

    private:
    GLuint m_sceneTextureID;
    ImVec2 m_viewportSize = ImVec2(1920.0f, 1080.0f);

    SceneObject* m_objectToDelete = nullptr;
    
    bool m_addEmptyObject = false;
    bool m_addCubeObject = false;

    bool m_gizmosLocal = false;
    bool m_originPivoting = false;

    SceneObject* m_addObjectParent = nullptr;

    SceneObject* m_draggedNodeToMove = nullptr;
    SceneObject* m_targetParentNode = nullptr;

    bool m_selectObjectRaycast = false;
    SceneObject* m_objectToSelectRaycast = nullptr;

    BOTTOM_PANEL m_bottomPanelActiveType = BOTTOM_PANEL::FILESYSTEM;

    ImGuizmo::OPERATION m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

    void DrawHierarchyPanel();
    void DrawScenePanel();
    void DrawBottomPanel();
    void DrawInspectorPanel();
    void DrawHierarchyNode(SceneObject* obj);
    void DrawTagsPanel();
    void DrawMenuBar();
};