#include "UI/Panels/UIFileSystem.h"

#include <string>
#include "imgui/imgui.h"
#include "Tools/drawUtils.h"
#include "Tools/FsDialog.h"
#include "IO/FileSystem.h"
#include "Core/AssetManager.h"
#include "Fonts/FontsLoader.h"

#include "UI/EditorCustomizations.h"

void UIFileSystem::Draw(SceneObject* m_selectedSceneObj)
{
    if (ImGui::BeginTabBar("AssetsTabs"))
    {
        if (ImGui::BeginTabItem("Models"))
        {
            if (DrawButtonColored("+ Import", BUTTON_COLORS::GREY))
            {
                std::string path = FsDialog::OpenModelDialog("Select model to import");
                if (path != "")
                {
                    std::string ext = fs::path(path).extension().string();
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
                    std::string ext = entry.path().extension().string();
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
                    std::string ext = fs::path(path).extension().string();
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
                    std::string ext = entry.path().extension().string();
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
                    std::string ext = entry.path().extension().string();
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
            if (DrawButtonColored("+ Create", BUTTON_COLORS::GREY))
            {
                SceneManager::SaveCurrentScene();
            }

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
}