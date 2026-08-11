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

void UIInspector::Draw()
{
    using type = SELECTION_TYPE;

    type currentType = SelectionManager::GetSelectedType();

    switch (currentType)
    {
        case type::SCENE_OBJECT: DrawSceneObject(); break;
        case type::MATERIAL: DrawMaterial(); break;
    }
}

void UIInspector::DrawTexture()
{
    
}

void UIInspector::DrawMaterial()
{
    Material* selectedMaterial = SelectionManager::GetAsMaterial();

    if (selectedMaterial == nullptr)
        return;

    float shininess = selectedMaterial->GetShininess();
    if (DrawHybridFloat("Shininess", &shininess, 2, 2048, "%.1f", false, 1, ImGuiSliderFlags_Logarithmic))
        selectedMaterial->SetShininess(shininess);

    vec4 tintColor = selectedMaterial->GetTintColor();
    if (ImGui::ColorEdit4("tint", &tintColor.x))
        selectedMaterial->SetTintColor(tintColor); 

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
    } else 
        TextUnformatted(std::string("Shader: " + usedShader->GetName()).c_str());
}

void UIInspector::DrawSceneObject()
{
    SceneObject* selectedSceneObject = SelectionManager::GetAsSceneObject();

    if (selectedSceneObject == nullptr)
        return;

    bool active = selectedSceneObject->IsActive();
    ImGui::Checkbox("##ActiveObjectCheckbox", &active);

    ImGui::SameLine();

    char nameBuf[256];
    strcpy(nameBuf, selectedSceneObject->name.c_str());
    if (ImGui::InputText("##Name", nameBuf, sizeof(nameBuf)))
        selectedSceneObject->name = nameBuf;

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