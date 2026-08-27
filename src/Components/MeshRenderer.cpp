#include "Components/MeshRenderer.h"
#include "Core/Transform.h"
#include "Core/SceneObject.h"
#include "Core/AssetManager.h"
#include "IO/Console.h"
#include "Render/Material.h"

MeshRenderer::MeshRenderer(SceneObject* sceneObject, Model* model, Material* material) : Component(sceneObject, COMPONENT_TYPE::MESH_RENDERER)
{
    if (sceneObject == nullptr)
        return;

    SetModel(model);
    SetMaterial(material);
}

void MeshRenderer::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos)
{
    if (!m_model)
        return;

    if (!m_material)
        return;

    m_material->Apply();
    Shader* currentShader = m_material->GetShader();

    if (currentShader)
    {
        Transform& transform = m_sceneObject->transform;
        currentShader->SetMat4("modelMatrix", transform.GetModelMatrix());
        currentShader->SetMat3("normalMatrix", transform.GetNormalMatrix());
        currentShader->SetMat4("viewMatrix", viewMatrix);
        currentShader->SetMat4("projectionMatrix", projectionMatrix);
        currentShader->SetVec3("viewPos", viewPos);

        //Overrides, required by specific components (like Telemetry Viewer)
        for (auto& it : m_materialOverrides)
        {
            if (std::holds_alternative<float>(it.second))
                currentShader->SetFloat(it.first, std::get<float>(it.second));
            else if (std::holds_alternative<int>(it.second))
                currentShader->SetInt(it.first, std::get<int>(it.second));
            else if (std::holds_alternative<vec3>(it.second))
                currentShader->SetVec3(it.first, std::get<vec3>(it.second));
            else if (std::holds_alternative<vec4>(it.second))
                currentShader->SetVec4(it.first, std::get<vec4>(it.second));
        }
    }

    for (unsigned int index : m_meshIndices)
    {
        const Mesh* mesh = m_model->GetMesh(index);
        if (mesh)
            mesh->Draw();
    }

    if (SceneManager::GetActiveScene()->GetShowBoundingBoxes())
    {
        BoundingBox box = GetGlobalBoundingBox();
        Utils::DrawDebugBox(box, viewMatrix, projectionMatrix);
    }
}

json MeshRenderer::ToJson() const
{
    json j = Component::ToJson();
    j["model"] = m_model ? m_model->GetName() : "";
    j["material"] = m_material ? m_material->GetName() : "";
    j["meshIndices"] = m_meshIndices;
    return j;
}

void MeshRenderer::FromJson(const json& j)
{
    Component::FromJson(j);
    std::string modelName = j.value("model", "");
    if (!modelName.empty())
    {
        m_model = AssetManager::GetModel(modelName);
        if (!m_model)
        {
            AssetManager::LoadModel(modelName);
            m_model = AssetManager::GetModel(modelName);
        }
    }

    std::string materialName = j.value("material", "");
    if (!materialName.empty())
    {
        m_material = AssetManager::GetMaterial(materialName);
        if (!m_material)
        {
            AssetManager::LoadMaterial(materialName);
            m_material = AssetManager::GetMaterial(materialName);
        }
    }

    if (j.contains("meshIndices"))
        m_meshIndices = j["meshIndices"].get<std::vector<unsigned int>>();
}

void MeshRenderer::OnGuiDraw()
{
    std::string text = "Model: ";
    if (m_model)
        text += m_model->GetName();

    TextUnformatted(text.c_str());

    Material* material = GetMaterial();

    ImGui::BeginGroup();
    DrawAssetSlot("Material", material->GetName(), IconsLoader::materialIconText);
    ImGui::EndGroup();
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL_D&D"))
        {
            const char* materialFileName = (const char*) payload->Data;
            Material* newMat = AssetManager::GetMaterial(materialFileName);
            if (newMat)
                SetMaterial(newMat);
            else
                Console::LogError("Dragged invalid payload", LOG_CATEGORY::ASSETMANAGER);
        }
        ImGui::EndDragDropTarget();
    }
}

BoundingBox MeshRenderer::GetGlobalBoundingBox()
{
    if (!m_model || m_meshIndices.empty())
        return BoundingBox();

    BoundingBox combinedLocalBox;

    for (unsigned int index : m_meshIndices)
    {
        const Mesh* mesh = m_model->GetMesh(index);
        if (mesh)
        {
            const BoundingBox& box = mesh->GetBoundingBox();
            combinedLocalBox.Combine(box);
        }
    }

    mat4 modelMat = m_sceneObject->transform.GetModelMatrix();
    return combinedLocalBox.GetTransformed(modelMat);
}