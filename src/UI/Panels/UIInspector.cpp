#include "UI/Panels/UIInspector.h"
#include "imgui/imgui.h"
#include "Tools/drawUtils.h"
#include "UI/SelectionManager.h"
#include "Icons/IconsLoader.h"
#include "Core/SceneObject.h"
#include "Core/TagManager.h"
#include "Components/Component.h"
#include "Core/AssetManager.h"
#include "IO/Console.h"
#include "Vendor/imgui/TextEditor.h"
#include <fstream>
#include <sstream>

#include "Render/Material.h"
#include "Render/Shader.h"
#include "Render/Texture.h"

void UIInspector::Draw()
{
    using type = SELECTION_TYPE;

    type currentType = SelectionManager::GetSelectedType();

    switch (currentType)
    {
        case type::SCENE_OBJECT: DrawSceneObject(); break;
        case type::MATERIAL: DrawMaterial(); break;
        case type::SHADER: DrawShader(); break;
    }
}

void UIInspector::DrawShader()
{
    static TextEditor vertexEditor;
    static TextEditor fragmentEditor;
    static bool editorsInitialized = false;
    static Shader* currentlyEditingShader = nullptr;
    static std::string name = "";

    if (!editorsInitialized)
    {
        vertexEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        vertexEditor.SetPalette(TextEditor::GetDarkPalette());
        
        fragmentEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        fragmentEditor.SetPalette(TextEditor::GetDarkPalette());
        
        editorsInitialized = true;
    }

    Shader* shader = SelectionManager::GetAsShader();
    if (shader == nullptr)
    {
        currentlyEditingShader = nullptr;
        return;
    }

    if (shader != currentlyEditingShader)
    {
        vertexEditor.SetText(shader->GetVertexCode());
        fragmentEditor.SetText(shader->GetFragmentCode());
        currentlyEditingShader = shader;
        name = shader->GetName();
    }

    if (DrawButtonColored("Reload", BUTTON_COLORS::GREEN))
    {
        shader->SetVertexCode(vertexEditor.GetText());
        shader->SetFragmentCode(fragmentEditor.GetText());

        if (shader->TryRecompileAndSave())
        {
            fs::path shadersFolder = FileSystem::GetAssetPath("User", "Shaders");
            fs::path finalPath = shadersFolder / (shader->GetName() + ".shader");
            
            std::ofstream out(finalPath.string());
            if (out.is_open())
            {
                out << "#pragma VERTEX\n";
                out << shader->GetVertexCode();
                if (!shader->GetVertexCode().empty() && shader->GetVertexCode().back() != '\n')
                    out << "\n"; 
                    
                out << "#pragma FRAGMENT\n";
                out << shader->GetFragmentCode();
                
                out.close();
                Console::LogInfo("Shader compiled and saved: " + shader->GetName(), LOG_CATEGORY::ASSETMANAGER);
            }
            else
                Console::LogError("Failed to save shader file: " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
        }
        else
            Console::LogError("Shader compilation failed. See logs for details.", LOG_CATEGORY::ASSETMANAGER);
    }

    ImGui::SameLine();

    if (DrawStringOnEnter("", name, "Shader name"))
    {
        if (name == "DefaultShader")
        {
            name = shader->GetName();
            NotificationSystem::Show("Can't rename a shader into DefaultShader", TOAST_WARNING);
        }
        else if (name == "")
        {
            name = shader->GetName();
            NotificationSystem::Show("Can't give an empty name to a shader", TOAST_WARNING);
        }
        else if (FileSystem::AssetAlreadyExists(name, ".shader", "Shaders"))
        {
            name = shader->GetName();
            NotificationSystem::Show("A shader with that name already exists", TOAST_WARNING);
        } else
            AssetManager::RenameShader(shader->GetName(), name);
    }

    if (ImGui::BeginTabBar("InspectorShaderTabs"))
    {
        if (ImGui::BeginTabItem("Vertex"))
        {
            vertexEditor.Render("VertexEditor", ImVec2(0, 0), true);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fragment"))
        {
            fragmentEditor.Render("FragmentEditor", ImVec2(0, 0), true);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void UIInspector::DrawTexture()
{
    
}

void UIInspector::DrawMaterial()
{
    static Material* currentlyEditingMaterial = nullptr;
    static std::string name = "";
    static int a = 0;
    a++;
    a%=100;

    Material* selectedMaterial = SelectionManager::GetAsMaterial();

    if (selectedMaterial == nullptr)
        return;

    if (selectedMaterial != currentlyEditingMaterial)
    {
        currentlyEditingMaterial = selectedMaterial;
        name = selectedMaterial->GetName();
    }

    if (DrawStringOnEnter("", name, "Material name"))
    {
        if (name == "DefaultMaterial")
        {
            name = selectedMaterial->GetName();
            NotificationSystem::Show("Can't rename a material into DefaultMaterial", TOAST_WARNING);
        }
        else if (name == "")
        {
            name = selectedMaterial->GetName();
            NotificationSystem::Show("Can't give an empty name to a material", TOAST_WARNING);
        }
        else if (FileSystem::AssetAlreadyExists(name, ".mat", "Materials"))
        {
            name = selectedMaterial->GetName();
            NotificationSystem::Show("A material with that name already exists", TOAST_WARNING);
        } else
            AssetManager::RenameMaterial(selectedMaterial->GetName(), name);
    }

    float shininess = selectedMaterial->GetShininess();
    if (DrawHybridFloat("Shininess", &shininess, 2, 2048, "%.1f", false, 1, ImGuiSliderFlags_Logarithmic))
        selectedMaterial->SetShininess(shininess);

    vec4 tintColor = selectedMaterial->GetTintColor();
    if (ImGui::ColorEdit4("tint", &tintColor.x))
        selectedMaterial->SetTintColor(tintColor);


    bool usePlainColor = selectedMaterial->GetUsePlainColor();
    DrawToggleSwitch("Plain color", &usePlainColor);
    if (usePlainColor != selectedMaterial->GetUsePlainColor())
        selectedMaterial->SetUsePlainColor(usePlainColor);

    Texture* colorMap = selectedMaterial->GetColorMap();
    std::string colorMapName = colorMap ? colorMap->GetName() : "None";

    ImGui::BeginGroup();
    DrawAssetSlot("Color map", colorMapName, IconsLoader::textureIconText);
    ImGui::EndGroup();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_D&D"))
        {
            const char* filename = (const char*)payload->Data;
            Texture* newColorMap = AssetManager::GetTexture(filename);
            if (newColorMap)
                selectedMaterial->SetColorMap(newColorMap);
            else
                Console::LogError("Dragged Invalid Payload", LOG_CATEGORY::ASSETMANAGER);
        }
        ImGui::EndDragDropTarget();
    }

    Texture* specularMap = selectedMaterial->GetSpecularMap();
    std::string specularMapName = specularMap ? specularMap->GetName() : "None";

    ImGui::BeginGroup();
    DrawAssetSlot("Specular map", specularMapName, IconsLoader::textureIconText);
    ImGui::EndGroup();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_D&D"))
        {
            const char* filename = (const char*)payload->Data;
            Texture* newSpecularMap = AssetManager::GetTexture(filename);
            if (newSpecularMap)
                selectedMaterial->SetSpecularMap(newSpecularMap);
            else
                Console::LogError("Dragged Invalid Payload", LOG_CATEGORY::ASSETMANAGER);
        }
        ImGui::EndDragDropTarget();
    }

    Shader* usedShader = selectedMaterial->GetShader();
    if (!usedShader)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        TextUnformatted("No Shader");
        ImGui::PopStyleColor();
    } else {
        ImGui::BeginGroup();
        DrawAssetSlot("Shader", usedShader->GetName(), IconsLoader::shaderIconText);
        ImGui::EndGroup();

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHADER_D&D"))
            {
                const char* filename = (const char*)payload->Data;
                Shader* newShader = AssetManager::GetShader(filename);
                if (newShader)
                    selectedMaterial->SetShader(newShader);
                else
                    Console::LogError("Dragged Invalid Payload", LOG_CATEGORY::ASSETMANAGER);
            }
            ImGui::EndDragDropTarget();
        }
    }
}

void UIInspector::DrawSceneObject()
{
    SceneObject* selectedSceneObject = SelectionManager::GetAsSceneObject();

    if (selectedSceneObject == nullptr)
        return;

    bool active = selectedSceneObject->IsActive();
    ImGui::Checkbox("##ActiveObjectCheckbox", &active);

    ImGui::SameLine();

    DrawStringOnEnter("", selectedSceneObject->name, "Scene Object name");

    if (active != selectedSceneObject->IsActive())
        selectedSceneObject->SetIsActive(active);

    const auto& allTags = TagManager::GetTags();
    std::string tag = selectedSceneObject->GetTag();
    if (DrawStringCombo("Tag", &tag, allTags, 3))
    {
        selectedSceneObject->SetTag(tag);
    }

    Spacing(2);
    TextUnformatted("Transform");
    Indent();

    Transform* tr = &selectedSceneObject->transform;
    vec3 pos = tr->GetPosition();
    vec3 rot = tr->GetEulerAngles();
    vec3 scale = tr->GetScale();

    if (DrawFloatCoords3("position", &pos.x, 3))
        tr->SetPosition(pos);

    if (DrawFloatCoords3("rotation", &rot.x, 3, -360.0f, 360.0f))
        tr->SetRotation(rot);

    if (DrawFloatCoords3("scale", &scale.x, 3, 0.0f))
        tr->SetScale(scale);

    Unindent();

    Spacing(2);

    const auto& components = selectedSceneObject->GetComponents();

    for (const auto& comp : components)
    {
        if (ImGui::CollapsingHeader(comp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushID(comp.get());
            comp->OnGuiDraw();
            ImGui::PopID();
        }
    }

    ImGui::Separator();


    COMPONENT_TYPE componentTypeOut;
    if (DrawElementResearchMenu<COMPONENT_TYPE>("+ Component", componentTypeOut, Component::availableComponentsOptions,
        [&](COMPONENT_TYPE type){return selectedSceneObject->HasComponent(type);},
        BUTTON_COLORS::GREY))
    {
        selectedSceneObject->AddComponent(componentTypeOut);
    }
}