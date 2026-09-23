#include "Core/SceneObject.h"
#include "Core/AssetManager.h"
#include "Core/TagManager.h"
#include "Utils/Utils.h"

SceneObject::SceneObject() : SceneObject("SceneObject") {}

SceneObject::SceneObject(const std::string& name)
    : name(name), m_parentID(0), transform(this), m_tag("Default")
{
    m_ID = Utils::GetRandomID();
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
    j["tag"] = m_tag;

    if (transform.GetParent() != nullptr && transform.GetParent()->GetSceneObject() != nullptr)
        j["parent_id"] = transform.GetParent()->GetSceneObject()->GetID();
    else
        j["parent_id"] = 0;

    json jComponents = json::array();
    for (const auto& comp : m_components)
        jComponents.push_back(comp->ToJson());
    j["components"] = jComponents;

    return j;
}

void SceneObject::FromJson(const json& j)
{
    name = j.contains("name") ? j["name"].get<std::string>() : "Unknown Object";
    m_ID = j.value("id", 0);
    m_parentID = j.value("parent_id", 0);
    m_tag = j.contains("tag") ? j["tag"].get<std::string>() : "Default";
    TagManager::AddTag(m_tag);

    if (j.contains("transform"))
        transform.FromJson(j["transform"]);

    m_components.clear();

    if (j.contains("components"))
    {
        for (const json& jComponent : j["components"])
        {
            COMPONENT_TYPE type = jComponent.contains("componentType") ? static_cast<COMPONENT_TYPE>(jComponent["componentType"].get<int>()) : COMPONENT_TYPE::MESH_RENDERER;
            Component* newComp = AddComponent(type);
            if (newComp)
                newComp->FromJson(jComponent);
        }
    }
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
    case COMPONENT_TYPE::MOUNT_POINT:
        newComp = std::make_unique<MountPoint>(this);
        break;
    default:
        Console::LogWarn("Unknown component type requested for " + name, LOG_CATEGORY::SCENE);
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