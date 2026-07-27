#include "UI/Editor.h"
#include <Core/Application.h>
#include "UI/EditorCustomizations.h"
#include "Core/TagManager.h"

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
        ImGui::DockBuilderDockWindow("Asset Browser", dock_id_bottom);
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
    DrawFileSystemPanel();

    ImGui::End();
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

    if (m_draggedNodeToMove != nullptr)
    {
        m_draggedNodeToMove->transform.SetParent(m_targetParentNode ? &m_targetParentNode->transform : nullptr);

        m_draggedNodeToMove = nullptr;
        m_targetParentNode = nullptr;
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void Editor::DrawHierarchyNode(SceneObject* obj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (m_selectedSceneObj == obj)
        flags |= ImGuiTreeNodeFlags_Selected;

    const auto& childTransforms = obj->transform.GetChildren();
    if (childTransforms.empty())
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool nodeOpen = ImGui::TreeNodeEx((void*)obj, flags, "%s", obj->name.c_str());

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        m_selectedSceneObj = obj;

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
                std::cerr << "[EDITOR] invalid drag" << std::endl;
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

    //TODO: tags
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

    if (m_selectedSceneObj == nullptr)
    {
        ImGui::End();
        return;
    }

    bool active = m_selectedSceneObj->IsActive();
    ImGui::Checkbox("##ActiveObjectCheckbox", &active);

    ImGui::SameLine();

    char nameBuf[256];
    strcpy(nameBuf, m_selectedSceneObj->name.c_str());
    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
        m_selectedSceneObj->name = nameBuf;

    if (active != m_selectedSceneObj->IsActive())
        m_selectedSceneObj->SetIsActive(active);

    SpacingH(10);

    const auto& allTags = TagManager::GetTags();
    std::string tag = m_selectedSceneObj->GetTag();
    if (DrawStringCombo("Tag", &tag, allTags))
    {
        m_selectedSceneObj->SetTag(tag);
    }

    Spacing(2);
    TextUnformatted("Transform");
    Indent();

    Transform* tr = &m_selectedSceneObj->transform;
    vec3 pos = tr->GetPosition();
    vec3 rot = tr->GetRotation();
    vec3 scale = tr->GetScale();

    if (DrawFloatCoords3("position", &pos.x, 3))
        tr->SetPosition(pos);

    if (DrawFloatCoords3("rotation", &rot.x, 3, -360.0f, 360.0f))
        tr->SetRotation(rot);

    if (DrawFloatCoords3("scale", &scale.x, 3, 0.0f))
        tr->SetScale(scale);

    Unindent();

    Spacing(2);

    const auto& components = m_selectedSceneObj->GetComponents();

    for (const auto& comp : components)
    {
        if (ImGui::CollapsingHeader(comp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushID(comp.get());
            comp->OnGuiDraw();
            ImGui::PopID();
        }
    }

    if (m_selectedSceneObj->GetComponent<TelemetryViewer>() == nullptr)
    {
        if (DrawButtonColored("+ Telemetry Viewer", BUTTON_COLORS::GREY))
            m_selectedSceneObj->AddComponent(COMPONENT_TYPE::TELEMETRY_VIEWER);
    }


    ImGui::End();
}

void Editor::DrawScenePanel()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene");
    ImGui::PopStyleVar();

    bool isSceneHovered = ImGui::IsWindowHovered();
    Camera* mainCamera = Application::Instance->GetCamera();
    if (mainCamera)
        mainCamera->SetCanInteract(isSceneHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_viewportSize = viewportPanelSize;

    ImGui::Image(
        (void*)(intptr_t)m_sceneTextureID, 
        ImVec2(m_viewportSize.x, m_viewportSize.y), 
        ImVec2(0, 1), 
        ImVec2(1, 0)
    );

    ImGui::SetCursorPos(ImVec2(0, 0));
    TextUnformatted(SceneManager::GetActiveScene()->GetName().c_str());

    ImGui::End();
}

void Editor::DrawFileSystemPanel()
{
    ImGui::Begin("Asset Browser");
    
    if (ImGui::BeginTabBar("AssetsTabs"))
    {
        if (ImGui::BeginTabItem("Models"))
        {
            if (DrawButtonColored("+ Import", BUTTON_COLORS::GREY))
            {
                std::string path = FsDialog::OpenModelDialog("Select model to import");
                if (path != "")
                {
                    std::string ext = fs::path(path).extension();
                    if (ext == ".obj" || ext == ".fbx")
                    {
                        if (FileSystem::ImportAsset(path, "User", "Models"))
                        {
                            AssetManager::LoadModel(fs::path(path).filename().stem().string());
                            NotificationSystem::Show("Model imported");
                        } else
                            NotificationSystem::Show("Import error", TOAST_ERROR);
                    } else {
                        std::string errorMsg = "Wrong file ext (" + ext + ")";
                        NotificationSystem::Show(errorMsg.c_str(), TOAST_ERROR);
                    }
                } else
                    NotificationSystem::Show("Empty path", TOAST_WARNING);
            }

            ImGui::Spacing();

            FontsLoader::PushFont(FontsLoader::FONTS::ROBOTO_SMALL);

            fs::path modelsPath = fs::path(FileSystem::GetAssetPath("User", "Models"));
            if (fs::exists(modelsPath))
            {
                float availableWidth = ImGui::GetContentRegionAvail().x - (fileSystemPref.borderMargins * 2);
                float currentX = 0.0f;
                ImTextureID iconID = (ImTextureID)(intptr_t)IconsLoader::modelIconText;

                for (const auto& entry : fs::directory_iterator(modelsPath))
                {
                    std::string ext = entry.path().extension();
                    if (entry.is_regular_file() && (ext == ".obj" || ext == ".fbx"))
                    {
                        std::string filename = entry.path().stem().string();
                        ImGui::PushID(entry.path().string().c_str());

                        if (currentX == 0.0f)
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fileSystemPref.borderMargins);
                        else
                            ImGui::SameLine(0.0f, fileSystemPref.cellSpacing);

                        ImGui::BeginGroup();
                            
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Model"))
                            {
                                Application::Instance->AddModelToScene(filename);
                            }
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();
                        currentX += fileSystemPref.cellSize + fileSystemPref.cellSpacing;
                        if (currentX + fileSystemPref.cellSize > availableWidth)
                            currentX = 0;

                        ImGui::PopID();
                    }
                }
            }

            FontsLoader::PopFont();
            ImGui::EndTabItem();
        }



        if (ImGui::BeginTabItem("Textures"))
        {
            if (DrawButtonColored("+ Import", BUTTON_COLORS::GREY))
            {
                std::string path = FsDialog::OpenImageDialog("Select an image to import");
                if (path != "")
                {
                    std::string ext = fs::path(path).extension();
                    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
                    {
                        if (FileSystem::ImportAsset(path, "User", "Textures"))
                        {
                            AssetManager::LoadTexture(fs::path(path).filename().stem().string());
                            NotificationSystem::Show("Texture imported");
                        } else
                            NotificationSystem::Show("Import error", TOAST_ERROR);
                    } else {
                        std::string errorMsg = "Wrong file ext (" + ext + ")";
                        NotificationSystem::Show(errorMsg.c_str(), TOAST_ERROR);
                    }
                } else
                    NotificationSystem::Show("Empty path", TOAST_WARNING);
            }

            ImGui::Spacing();

            FontsLoader::PushFont(FontsLoader::FONTS::ROBOTO_SMALL);

            fs::path modelsPath = fs::path(FileSystem::GetAssetPath("User", "Textures"));
            if (fs::exists(modelsPath))
            {
                float availableWidth = ImGui::GetContentRegionAvail().x - (fileSystemPref.borderMargins * 2);
                float currentX = 0.0f;
                ImTextureID iconID = (ImTextureID)(intptr_t)IconsLoader::textureIconText;

                for (const auto& entry : fs::directory_iterator(modelsPath))
                {
                    std::string ext = entry.path().extension();
                    if (entry.is_regular_file() && (ext == ".png" || ext == ".jpg" || ext == ".jpeg"))
                    {
                        std::string filename = entry.path().stem().string();
                        ImGui::PushID(entry.path().string().c_str());

                        if (currentX == 0.0f)
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fileSystemPref.borderMargins);
                        else
                            ImGui::SameLine(0.0f, fileSystemPref.cellSpacing);

                        ImGui::BeginGroup();
                            
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Texture"))
                            {
                                if (m_selectedSceneObj)
                                {
                                    MeshRenderer* renderer = m_selectedSceneObj->GetComponent<MeshRenderer>();
                                    renderer->GetMaterial()->SetColorMap(AssetManager::GetTexture(entry.path().filename().stem().string()));
                                }
                            }
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();
                        currentX += fileSystemPref.cellSize + fileSystemPref.cellSpacing;
                        if (currentX + fileSystemPref.cellSize > availableWidth)
                            currentX = 0;

                        ImGui::PopID();
                    }
                }
            }

            FontsLoader::PopFont();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Shaders Code"))
        {
            if (DrawButtonColored("+ Create Vertex Shader", BUTTON_COLORS::GREY))
                AssetManager::CreateNewFragmentShaderCode();

            SpacingH(10);
            ImGui::SameLine();

            if (DrawButtonColored("+ Create Fragment Shader", BUTTON_COLORS::GREY))
                AssetManager::CreateNewVertexShaderCode();

            ImGui::Spacing();

            FontsLoader::PushFont(FontsLoader::FONTS::ROBOTO_SMALL);

            fs::path shadersCodePath = fs::path(FileSystem::GetAssetPath("User", "ShadersCode"));
            if (fs::exists(shadersCodePath))
            {
                float availableWidth = ImGui::GetContentRegionAvail().x - (fileSystemPref.borderMargins * 2);
                float currentX = 0.0f;

                for (const auto& entry : fs::directory_iterator(shadersCodePath))
                {
                    std::string ext = entry.path().extension();
                    if (entry.is_regular_file() && (ext == ".vert" || ext == ".frag"))
                    {
                        std::string filename = entry.path().stem().string();
                        ImGui::PushID(entry.path().string().c_str());

                        if (currentX == 0.0f)
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fileSystemPref.borderMargins);
                        else
                            ImGui::SameLine(0.0f, fileSystemPref.cellSpacing);

                        ImGui::BeginGroup();
                            ImTextureID iconID = ext == ".vert" ? 
                                (ImTextureID)(intptr_t)IconsLoader::vertexShaderIconText : 
                                (ImTextureID)(intptr_t)IconsLoader::fragmentShaderIconText;
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Shader"))
                            {
                                
                            }
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();
                        currentX += fileSystemPref.cellSize + fileSystemPref.cellSpacing;
                        if (currentX + fileSystemPref.cellSize > availableWidth)
                            currentX = 0;

                        ImGui::PopID();
                    }
                }
            }

            FontsLoader::PopFont();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Shaders"))
        {
            if (DrawButtonColored("+ Create", BUTTON_COLORS::GREY))
                AssetManager::CreateNewShader();

            ImGui::Spacing();

            FontsLoader::PushFont(FontsLoader::FONTS::ROBOTO_SMALL);

            fs::path shadersPath = fs::path(FileSystem::GetAssetPath("User", "Shaders"));
            if (fs::exists(shadersPath))
            {
                float availableWidth = ImGui::GetContentRegionAvail().x - (fileSystemPref.borderMargins * 2);
                float currentX = 0.0f;
                ImTextureID iconID = (ImTextureID)(intptr_t)IconsLoader::shaderIconText;

                for (const auto& entry : fs::directory_iterator(shadersPath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".shader")
                    {
                        std::string filename = entry.path().stem().string();
                        ImGui::PushID(entry.path().string().c_str());

                        if (currentX == 0.0f)
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fileSystemPref.borderMargins);
                        else
                            ImGui::SameLine(0.0f, fileSystemPref.cellSpacing);

                        ImGui::BeginGroup();
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Shader"))
                            {
                                
                            }
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();
                        currentX += fileSystemPref.cellSize + fileSystemPref.cellSpacing;
                        if (currentX + fileSystemPref.cellSize > availableWidth)
                            currentX = 0;

                        ImGui::PopID();
                    }
                }
            }

            FontsLoader::PopFont();
            ImGui::EndTabItem();
        }



        if (ImGui::BeginTabItem("Materials"))
        {
            if (DrawButtonColored("+ Create", BUTTON_COLORS::GREY))
                AssetManager::CreateNewMaterial();

            ImGui::Spacing();

            FontsLoader::PushFont(FontsLoader::FONTS::ROBOTO_SMALL);

            fs::path matPath = fs::path(FileSystem::GetAssetPath("User", "Materials"));
            if (fs::exists(matPath))
            {
                float availableWidth = ImGui::GetContentRegionAvail().x - (fileSystemPref.borderMargins * 2);
                float currentX = 0.0f;
                ImTextureID iconID = (ImTextureID)(intptr_t)IconsLoader::materialIconText;

                for (const auto& entry : fs::directory_iterator(matPath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".mat")
                    {
                        std::string filename = entry.path().stem().string();
                        ImGui::PushID(entry.path().string().c_str());

                        if (currentX == 0.0f)
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fileSystemPref.borderMargins);
                        else
                            ImGui::SameLine(0.0f, fileSystemPref.cellSpacing);

                        ImGui::BeginGroup();
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Material"))
                            {
                                if (m_selectedSceneObj)
                                {
                                    MeshRenderer* renderer = m_selectedSceneObj->GetComponent<MeshRenderer>();
                                    renderer->SetSharedMaterial(AssetManager::GetMaterial(filename));
                                }
                            }
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();
                        currentX += fileSystemPref.cellSize + fileSystemPref.cellSpacing;
                        if (currentX + fileSystemPref.cellSize > availableWidth)
                            currentX = 0;

                        ImGui::PopID();
                    }
                }
            }

            FontsLoader::PopFont();
            ImGui::EndTabItem();
        }


        if (ImGui::BeginTabItem("Scenes"))
        {
            if (DrawButtonColored("+ Create", BUTTON_COLORS::GREY));

            ImGui::Spacing();

            FontsLoader::PushFont(FontsLoader::FONTS::ROBOTO_SMALL);

            fs::path scenesPath = fs::path(FileSystem::GetAssetPath("User", "Scenes"));
            if (fs::exists(scenesPath))
            {
                float availableWidth = ImGui::GetContentRegionAvail().x - (fileSystemPref.borderMargins * 2);
                float currentX = 0.0f;
                ImTextureID iconID = (ImTextureID)(intptr_t)IconsLoader::sceneIconText;

                for (const auto& entry : fs::directory_iterator(scenesPath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".scene")
                    {
                        std::string filename = entry.path().stem().string();
                        ImGui::PushID(entry.path().string().c_str());

                        if (currentX == 0.0f)
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fileSystemPref.borderMargins);
                        else
                            ImGui::SameLine(0.0f, fileSystemPref.cellSpacing);

                        ImGui::BeginGroup();
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Scene"))
                            {
                                SceneManager::LoadScene(entry.path().filename().stem().string());
                            }
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();
                        currentX += fileSystemPref.cellSize + fileSystemPref.cellSpacing;
                        if (currentX + fileSystemPref.cellSize > availableWidth)
                            currentX = 0;

                        ImGui::PopID();
                    }
                }
            }

            FontsLoader::PopFont();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}