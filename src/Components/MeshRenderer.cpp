#include "Components/MeshRenderer.h"
#include "Core/Transform.h"
#include "Core/SceneObject.h"
#include "Core/AssetManager.h"

MeshRenderer::MeshRenderer(SceneObject* sceneObject, Model* model, Material* material) : Component(sceneObject, COMPONENT_TYPE::MESH_RENDERER)
{
    if (sceneObject == nullptr)
        return;

    SetModel(model);
    SetSharedMaterial(material);
}

void MeshRenderer::SetSharedMaterial(Material* material)
{
    m_sharedMaterial = material;
    m_instancedMaterial.reset();
}

Material* MeshRenderer::GetMaterial()
{
    if (!m_instancedMaterial && m_sharedMaterial)
        m_instancedMaterial = std::make_unique<Material>(m_sharedMaterial->Clone());

    if (m_instancedMaterial)
        return m_instancedMaterial.get();

    return nullptr;
}

void MeshRenderer::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos)
{
    if (!m_model)
        return;

    Material* material = m_instancedMaterial ? m_instancedMaterial.get() : m_sharedMaterial;

    if (!material)
        return;

    material->Apply();
    Shader* currentShader = material->GetShader();

    if (currentShader)
    {
        Transform& transform = m_sceneObject->transform;
        currentShader->SetMat4("modelMatrix", transform.GetModelMatrix());
        currentShader->SetMat3("normalMatrix", transform.GetNormalMatrix());
        currentShader->SetMat4("viewMatrix", viewMatrix);
        currentShader->SetMat4("projectionMatrix", projectionMatrix);
        currentShader->SetVec3("viewPos", viewPos);
    }

    for (unsigned int index : m_meshIndices)
    {
        const Mesh* mesh = m_model->GetMesh(index);
        if (mesh)
            mesh->Draw();
    }
}

//TODO: ToJson e FromJson
json MeshRenderer::ToJson() const
{
    json j = Component::ToJson();
    j["model"] = m_model ? m_model->GetName() : "";
    j["sharedMaterial"] = m_sharedMaterial ? m_sharedMaterial->GetName() : "";
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

    std::string materialName = j.value("sharedMaterial", "");
    if (!materialName.empty())
    {
        m_sharedMaterial = AssetManager::GetMaterial(materialName);
        if (!m_sharedMaterial)
        {
            AssetManager::LoadMaterial(materialName);
            m_sharedMaterial = AssetManager::GetMaterial(materialName);
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

    
    text = "Material: ";
    Material* material = GetMaterial();
    if (material)
        text += material->GetName();

    TextUnformatted(text.c_str());
}