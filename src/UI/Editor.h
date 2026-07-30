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
    SceneObject* m_objectToDelete = nullptr;
    
    bool m_addObject = false;
    SceneObject* m_addObjectParent = nullptr;

    SceneObject* m_draggedNodeToMove = nullptr;
    SceneObject* m_targetParentNode = nullptr;

    bool m_selectObjectRaycast = false;
    SceneObject* m_objectToSelectRaycast = nullptr;

    ImGuizmo::OPERATION m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

    void DrawHierarchyPanel();
    void DrawScenePanel();
    void DrawFileSystemPanel();
    void DrawInspectorPanel();
    void DrawHierarchyNode(SceneObject* obj);
    void DrawTagsPanel();

    inline static const std::vector<ComboEntry<COMPONENT_TYPE>> m_availableComponentsOptions = {
        { "Mesh Renderer", COMPONENT_TYPE::MESH_RENDERER },
        { "Telemetry Viewer", COMPONENT_TYPE::TELEMETRY_VIEWER }
    };
};