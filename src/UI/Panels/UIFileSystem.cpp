#include "UI/Panels/UIFileSystem.h"

#include <string>
#include "imgui/imgui.h"
#include "Tools/drawUtils.h"
#include "Tools/FsDialog.h"
#include "IO/FileSystem.h"
#include "Core/AssetManager.h"
#include "Fonts/FontsLoader.h"
#include "UI/SelectionManager.h"

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
                                SelectionManager::Select(AssetManager::GetTexture(filename));
                            }

                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();

                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                        {
                            ImGui::SetDragDropPayload("TEXTURE_D&D", filename.c_str(), filename.size() + 1);
                            TextUnformatted(filename);
                            ImGui::EndDragDropSource();
                        }
                        
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
                        if (filename == "DefaultShader")
                            ImGui::BeginDisabled();
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Shader"))
                            {
                                SelectionManager::Select(AssetManager::GetShader(filename));
                            }
                        if (filename == "DefaultShader")
                            ImGui::EndDisabled();
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();

                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                        {
                            ImGui::SetDragDropPayload("SHADER_D&D", filename.c_str(), filename.size() + 1);
                            TextUnformatted(filename);
                            ImGui::EndDragDropSource();
                        }

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
                        if (filename == "DefaultMaterial")
                            ImGui::BeginDisabled();
                            if (DrawButtonImage(iconID, ImVec2(fileSystemPref.cellSize, fileSystemPref.cellSize), 
                                    BUTTON_COLORS::NONE, "Material"))
                            {
                                SelectionManager::Select(AssetManager::GetMaterial(filename));
                            }
                        if (filename == "DefaultMaterial")
                            ImGui::EndDisabled();
                            TextElided(filename, fileSystemPref.cellSize);
                        ImGui::EndGroup();

                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                        {
                            ImGui::SetDragDropPayload("MATERIAL_D&D", filename.c_str(), filename.size() + 1);
                            TextUnformatted(filename);
                            ImGui::EndDragDropSource();
                        }

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