#include "Core/SceneObject.h"
#include "Core/AssetManager.h"
#include <random>

SceneObject::SceneObject() : SceneObject("SceneObject", nullptr, nullptr) {}

SceneObject::SceneObject(const std::string& name, Model* model, Material* material)
    : name(name), m_model(model), m_material(material), m_parentID(0), transform(this)
{
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint64_t> distr;

    m_ID = distr(eng);
}

void SceneObject::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos)
{
    if (!m_model || !m_material)
        return;

    m_material->Apply();

    Shader* currentShader = m_material->GetShader();

    if (currentShader)
    {
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

json SceneObject::ToJson() const
{
    json j;
    j["name"] = name;
    j["model"] = m_model ? m_model->GetName() : "";
    j["material"] = m_material ? m_material->GetName() : "";
    j["transform"] = transform.ToJson();
    j["id"] = m_ID;

    if (transform.GetParent() != nullptr && transform.GetParent()->GetSceneObject() != nullptr)
        j["parent_id"] = transform.GetParent()->GetSceneObject()->GetID();
    else
        j["parent_id"] = 0;

    return j;
}

void SceneObject::FromJson(const json& j)
{
    name = j.contains("name") ? j["name"].get<std::string>() : "Unknown Object";
    std::string modelName = j.contains("model") ? j["model"].get<std::string>() : "";
    m_ID = j.value("id", 0);
    m_parentID = j.value("parent_id", 0);

    if (j.contains("transform"))
        transform.FromJson(j["transform"]);

    if (modelName != "")
    {
        m_model = AssetManager::GetModel(modelName);
        if (!m_model)
        {
            AssetManager::LoadModel(modelName);
            m_model = AssetManager::GetModel(modelName);
        }
    }

    std::string materialName = j.contains("material") ? j["material"].get<std::string>() : "";

    if (materialName != "")
    {
        m_material = AssetManager::GetMaterial(materialName);
        if (!m_material)
        {
            AssetManager::LoadMaterial(materialName);
            m_material = AssetManager::GetMaterial(materialName);
        }
    }
}