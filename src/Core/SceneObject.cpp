#include "Core/SceneObject.h"
#include "Core/AssetManager.h"
#include <random>

SceneObject::SceneObject() : SceneObject("SceneObject") {}

SceneObject::SceneObject(const std::string& name)
    : name(name), m_parentID(0), transform(this)
{
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint64_t> distr;

    m_ID = distr(eng);
}

void SceneObject::Update()
{
    for (auto& c : m_components)
        c->Update();
}

json SceneObject::ToJson() const
{
    json j;
    j["name"] = name;
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
    m_ID = j.value("id", 0);
    m_parentID = j.value("parent_id", 0);

    if (j.contains("transform"))
        transform.FromJson(j["transform"]);

    /*std::string modelName = j.contains("model") ? j["model"].get<std::string>() : "";
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
    }*/
}

bool SceneObject::AddComponent(std::unique_ptr<Component> component)
{
    if (component && !HasComponent(component->GetComponentType()))
    {
        m_components.push_back(std::move(component));
        return true;
    }
    return false;
}

Component* SceneObject::AddComponent(COMPONENT_TYPE componentType)
{
    if (HasComponent(componentType))
        return nullptr;

    std::unique_ptr<Component> newComp = nullptr;

    switch (componentType)
    {
    case COMPONENT_TYPE::TELEMETRY_VIEWER:
        newComp = std::make_unique<TelemetryViewer>(this);
        break;
    case COMPONENT_TYPE::MESH_RENDERER:
        newComp = std::make_unique<MeshRenderer>(this);
        break;
    default:
        std::cerr << "[SCENE OBJECT] Unknown component type requested" << std::endl;
        return nullptr;
    }

    Component* rawPtr = newComp.get();
    m_components.push_back(std::move(newComp));
    return rawPtr;
}

bool SceneObject::RemoveComponent(COMPONENT_TYPE componentType)
{
    for (int i = 0; i < m_components.size(); i++)
    {
        if (m_components[i]->GetComponentType() == componentType)
        {
            m_components.erase(m_components.begin() + i);
            return true;
        }
    }
    return false;
}

bool SceneObject::HasComponent(COMPONENT_TYPE componentType)
{
    for (auto& c : m_components)
    {
        if (c->GetComponentType() == componentType)
            return true;
    }

    return false;
}

bool SceneObject::IsActiveInHierarchy() const
{
    if (!m_isActive)
        return false;

    Transform* parentTransform = transform.GetParent();
    if (parentTransform)
        return parentTransform->GetSceneObject()->IsActiveInHierarchy();

    return true;
}