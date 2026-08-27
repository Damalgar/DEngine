#include "UI/Editor.h"
#include <Core/Application.h>
#include "UI/EditorCustomizations.h"
#include "Core/TagManager.h"
#include "UI/SelectionManager.h"
#include "Core/TelemetryManager.h"

#include "UI/Panels/UIFileSystem.h"
#include "UI/Panels/UIConsole.h"
#include "UI/Panels/UIInspector.h"
#include "UI/Panels/UITimeline.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

Editor::Editor(GLuint sceneTextureID)
{
    m_sceneTextureID = sceneTextureID;
}

void Editor::DrawPanels()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DEngine DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    DrawMenuBar();

    ImGuiID dockspace_id = ImGui::GetID("MyEditorDockspace");

    static bool first_time = true;
    if (first_time)
    {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id; 
        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, NULL, &dock_main_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.33f, NULL, &dock_main_id);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.33f, NULL, &dock_main_id);
        ImGuiID dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.25f, NULL, &dock_id_left);

        ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
        ImGui::DockBuilderDockWindow("Tags", dock_id_left_bottom);
        ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
        ImGui::DockBuilderDockWindow("Bottom Panel", dock_id_bottom);
        ImGui::DockBuilderDockWindow("Scene", dock_main_id);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);
    DrawHierarchyPanel();

    ImGui::SetNextWindowClass(&window_class);
    DrawTagsPanel();

    ImGui::SetNextWindowClass(&window_class);
    DrawInspectorPanel();

    ImGui::SetNextWindowClass(&window_class);
    DrawScenePanel();

    ImGui::SetNextWindowClass(&window_class);
    DrawBottomPanel();

    if (m_draggedNodeToMove != nullptr)
    {
        m_draggedNodeToMove->transform.SetParent(m_targetParentNode ? &m_targetParentNode->transform : nullptr);

        m_draggedNodeToMove = nullptr;
        m_targetParentNode = nullptr;
    }

    if (m_addEmptyObject)
    {
        if (SceneManager::GetActiveScene())
        {
            SceneManager::GetActiveScene()->CreateEmptyObject(m_addObjectParent);
            m_addObjectParent = nullptr;
        }

        m_addEmptyObject = false;
        m_addObjectParent = nullptr;
    }

    if (m_addCubeObject)
    {
        if (SceneManager::GetActiveScene())
        {
            SceneManager::GetActiveScene()->CreateCubeObject(m_addObjectParent);
            m_addObjectParent = nullptr;
        }

        m_addCubeObject = false;
        m_addObjectParent = nullptr;
    }

    if (m_objectToDelete != nullptr)
    {
        if (SceneManager::GetActiveScene())
        {
            SceneManager::GetActiveScene()->RemoveObject(m_objectToDelete);

            if (m_objectToDelete == SelectionManager::GetAsSceneObject())
                SelectionManager::Deselect();
        }

        m_objectToDelete = nullptr;
    }

    if (m_selectObjectRaycast)
    {
        SelectionManager::Select(m_objectToSelectRaycast);
        m_selectObjectRaycast = false;
    }


    ImGui::End();
}

void Editor::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load CSV"))
            {
                fs::path path = FsDialog::OpenCSVDialog();
                if (path.string() != "")
                {
                    if (path.extension().string() == ".csv")
                    {
                        if (TelemetryManager::LoadFromCSV(path.string()))
                        {
                            NotificationSystem::Show("CSV Loaded");
                            UITimeline::SetStartFrame(1);
                            UITimeline::SetEndFrame(TelemetryManager::GetMaxFrames());
                        }
                        else
                            NotificationSystem::Show("Error loading CSV", TOAST_ERROR);
                    } else
                        NotificationSystem::Show("Not a CSV file", TOAST_ERROR);
                } else
                    NotificationSystem::Show("Error opening CSV", TOAST_ERROR);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void Editor::DrawHierarchyPanel()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
    ImGui::PopStyleVar();

    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::Indent(10.0f);
    ImGui::TextDisabled("Scene Hierarchy");
    ImGui::Unindent(10.0f);
    ImGui::Separator();

    const std::vector<SceneObject*> sceneObjects = Application::Instance->GetSceneObjects();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    for (SceneObject* obj : sceneObjects)
    {
        if (obj->transform.GetParent() == nullptr)
            DrawHierarchyNode(obj);
    }

    ImGui::Dummy(ImGui::GetContentRegionAvail());

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_NODE"))
        {
            SceneObject* draggedObj = *(SceneObject**)payload->Data;

            if (draggedObj)
            {
                m_draggedNodeToMove = draggedObj;
                m_targetParentNode = nullptr;
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void Editor::DrawHierarchyNode(SceneObject* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (SelectionManager::GetAsSceneObject() == obj)
        flags |= ImGuiTreeNodeFlags_Selected;

    const auto& childTransforms = obj->transform.GetChildren();
    if (childTransforms.empty())
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool nodeOpen = ImGui::TreeNodeEx((void*)obj, flags, "%s", obj->name.c_str());

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        SelectionManager::Select(obj);

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete"))
            m_objectToDelete = obj;

        if (ImGui::MenuItem("Add Empty Object"))
        {
            m_addEmptyObject = true;
            m_addObjectParent = obj;
        }

        if (ImGui::MenuItem("Add Cube"))
        {
            m_addCubeObject = true;
            m_addObjectParent = obj;
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("DND_HIERARCHY_NODE", &obj, sizeof(SceneObject*));
        ImGui::Text("Moving %s", obj->name.c_str()); 
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_NODE"))
        {
            SceneObject* draggedObj = *(SceneObject**)payload->Data;

            if (draggedObj != obj && !obj->transform.IsDescendantOf(&draggedObj->transform))
            {
                m_draggedNodeToMove = draggedObj;
                m_targetParentNode = obj;
            } else
                Console::LogWarn("invalid drag", LOG_CATEGORY::SYSTEM);
        }
        ImGui::EndDragDropTarget();
    }

    if (nodeOpen)
    {
        if (!childTransforms.empty())
        {
            for (Transform* childTransform : childTransforms)
            {
                SceneObject* childObj = childTransform->GetSceneObject(); 
                DrawHierarchyNode(childObj);
            }
            ImGui::TreePop();
        }
    }
}

void Editor::DrawTagsPanel()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Tags", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
    ImGui::PopStyleVar();

    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::Indent(10.0f);
    ImGui::TextDisabled("Tags");
    ImGui::Unindent(10.0f);
    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    std::vector<std::string> uniqueTags;
    const std::vector<SceneObject*> sceneObjects = Application::Instance->GetSceneObjects();

    static char newTagBuf[64] = "";
    ImGui::InputText("##NewTag", newTagBuf, sizeof(newTagBuf));
    ImGui::SameLine();
    if (DrawButtonColored("+", BUTTON_COLORS::GREY))
    {
        if (strlen(newTagBuf) > 0)
        {
            bool added = TagManager::AddTag(newTagBuf);
            if (added)
            {
                std::string notify = "Tag " + std::string(newTagBuf) + " added"; 
                NotificationSystem::Show(notify.c_str(), TOAST_INFO);
                newTagBuf[0] = '\0';
            } else
            {
                std::string notify = "Tag " + std::string(newTagBuf) + " already exists"; 
                NotificationSystem::Show(notify.c_str(), TOAST_WARNING);
            }
        }
    }

    const std::vector<std::string>& tags = TagManager::GetTags();
    for (const std::string& tag : tags)
    {
        if (tag.empty() || tag == "")
        {
            TagManager::RemoveTag("");
            continue;
        }

        ImGui::PushID(tag.c_str());
        bool isVisible = TagManager::IsTagVisible(tag);
        if (ImGui::Checkbox(tag.c_str(), &isVisible))
            TagManager::SetTagVisibility(tag, isVisible);

        if (tag != "Default")
        {
            ImGui::SameLine(ImGui::GetWindowWidth() - 50);
            if (DrawButtonColored("X", BUTTON_COLORS::NEGATIVE))
                TagManager::RemoveTag(tag);
        }
        ImGui::PopID();
    }

    Spacing(3);

    ImGui::PopStyleVar();
    ImGui::End();
}

void Editor::DrawInspectorPanel()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
    ImGui::PopStyleVar();

    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::Indent(10.0f);
    ImGui::TextDisabled("Inspector");
    ImGui::Unindent(10.0f);
    ImGui::Separator();

    UIInspector::Draw();

    ImGui::End();
}

void Editor::DrawScenePanel()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene");
    ImGui::PopStyleVar();

    bool isSceneHovered = ImGui::IsWindowHovered();
    Camera* mainCamera = Application::Instance->GetCamera();

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_viewportSize = viewportPanelSize;

    ImGui::Image(
        (void*)(intptr_t)m_sceneTextureID, 
        ImVec2(m_viewportSize.x, m_viewportSize.y), 
        ImVec2(0, 1), 
        ImVec2(1, 0)
    );
    
    ImVec2 imageTopLeft = ImGui::GetItemRectMin();
    ImVec2 imageSize = ImGui::GetItemRectSize();
    Scene* activeScene = SceneManager::GetActiveScene();
    
    if (activeScene)
    {
        //Needed to avoid raycasting pass on UI
        bool isHoveringUI = false;
        bool isHoveringGizmos = false;
        bool isUsingGizmos = false;

        ImGui::SetCursorPos(ImVec2(0, 0));
        TextUnformatted(activeScene->GetName().c_str());

        if (DrawButtonImage(IconsLoader::gizmosTranslateIconText, scenePref.buttonSize, BUTTON_COLORS::POSITIVE, "transform", true, 
            m_currentGizmoOperation==ImGuizmo::OPERATION::TRANSLATE))
            m_currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        isHoveringUI |= ImGui::IsItemHovered();

        if (DrawButtonImage(IconsLoader::gizmosRotateIconText, scenePref.buttonSize, BUTTON_COLORS::POSITIVE, "rotate", true, 
            m_currentGizmoOperation==ImGuizmo::OPERATION::ROTATE))
            m_currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
        isHoveringUI |= ImGui::IsItemHovered();

        if (DrawButtonImage(IconsLoader::gizmosScaleIconText, scenePref.buttonSize, BUTTON_COLORS::POSITIVE, "scale", true, 
            m_currentGizmoOperation==ImGuizmo::OPERATION::SCALE))
            m_currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
        isHoveringUI |= ImGui::IsItemHovered();

        Spacing(5);

        if (DrawButtonImage(IconsLoader::boundingBoxIconText, scenePref.buttonSize, BUTTON_COLORS::POSITIVE, "BoundingBox", true, 
            activeScene->GetShowBoundingBoxes()))
            activeScene->ToggleShowBoundingBoxes();
        isHoveringUI |= ImGui::IsItemHovered();

        SceneObject* selectedSceneObject = SelectionManager::GetAsSceneObject();
        mat4 viewMat = activeScene->GetViewMatrix();
        mat4 projectionMat = activeScene->GetProjectionMatrix();
        mat4 modelMat = selectedSceneObject ? selectedSceneObject->transform.GetModelMatrix() : mat4(1.0);

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(imageTopLeft.x, imageTopLeft.y, imageSize.x, imageSize.y);

        if (selectedSceneObject && mainCamera)
        {
            ImGuizmo::Manipulate
            (
                glm::value_ptr(viewMat),
                glm::value_ptr(projectionMat),
                m_currentGizmoOperation,
                ImGuizmo::MODE::LOCAL,
                glm::value_ptr(modelMat)
            );

            isHoveringGizmos = ImGuizmo::IsOver();
            isUsingGizmos = ImGuizmo::IsUsing();

            if (isUsingGizmos)
            {
                vec3 newTranslation;
                quat newRotationQuat;
                vec3 newScale;

                Transform* parentTransform = selectedSceneObject->transform.GetParent();

                if (parentTransform)
                {
                    mat4 parentGlobal = parentTransform->GetModelMatrix();
                    Utils::LocalFromGlobal(modelMat, parentGlobal, newTranslation, newRotationQuat, newScale);
                } else {
                    vec3 skew;
                    vec4 perspective;
                    glm::decompose(modelMat, newScale, newRotationQuat, newTranslation, skew, perspective);
                }

                selectedSceneObject->transform.SetPosition(newTranslation);
                selectedSceneObject->transform.SetRotation(newRotationQuat);
                selectedSceneObject->transform.SetScale(newScale);
            }
        }

        if (mainCamera)
            mainCamera->SetCanInteract(isSceneHovered && !isUsingGizmos && !isHoveringUI);

        if (isSceneHovered && !isHoveringGizmos && !isHoveringUI && InputsManager::IsMouseButtonPressed(ImGuiMouseButton_Left))
        {
            ImVec2 mouseAbsolutePos = ImGui::GetMousePos();
            float mouseX = mouseAbsolutePos.x - imageTopLeft.x;
            float mouseY = mouseAbsolutePos.y - imageTopLeft.y;
            
            vec3 clickDirection = Utils::GetMouseRayDirection(mouseX, mouseY, imageSize.x, imageSize.y, viewMat, projectionMat);
            m_objectToSelectRaycast = Utils::Raycast(mainCamera->GetPos(), clickDirection);

            m_selectObjectRaycast = true;
        }
    }

    ImGui::End();
}

void Editor::DrawBottomPanel()
{
    using panel = BOTTOM_PANEL;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Bottom Panel");
    ImGui::PopStyleVar();

    float spacing = 5.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImVec2 tabSize = ImVec2(150, 40);

    if (DrawButtonColored("Assets", BUTTON_COLORS::UTILITY, true, m_bottomPanelActiveType == panel::FILESYSTEM, tabSize)
        && m_bottomPanelActiveType != panel::FILESYSTEM)
            m_bottomPanelActiveType = panel::FILESYSTEM;

    SpacingH(spacing);
    ImGui::SameLine();
    if (DrawButtonColored("Console", BUTTON_COLORS::UTILITY, true, m_bottomPanelActiveType == panel::CONSOLE, tabSize)
        && m_bottomPanelActiveType != panel::CONSOLE)
            m_bottomPanelActiveType = panel::CONSOLE;

    SpacingH(spacing);
    ImGui::SameLine();
    bool showTimeline = TelemetryManager::IsCsvLoaded();
    if (!showTimeline)
        ImGui::BeginDisabled();
        
    if (DrawButtonColored("Timeline", BUTTON_COLORS::UTILITY, true, m_bottomPanelActiveType == panel::TIMELINE, tabSize)
        && m_bottomPanelActiveType != panel::TIMELINE)
            m_bottomPanelActiveType = panel::TIMELINE;

    if (!showTimeline)
        ImGui::EndDisabled();

    ImGui::PopStyleVar();

    Separator();

    switch (m_bottomPanelActiveType)
    {
        case panel::FILESYSTEM: UIFileSystem::Draw(SelectionManager::GetAsSceneObject()); break;
        case panel::CONSOLE: UIConsole::Draw(); break;
        case panel::TIMELINE: UITimeline::Draw(); break;
    }

    ImGui::End();
}