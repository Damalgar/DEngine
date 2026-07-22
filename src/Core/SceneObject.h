#pragma once

#include "Core/Transform.h"
#include "Core/ISerializable.h"
#include "include_components.h"

class SceneObject : public ISerializable {
    public:
    SceneObject();
    SceneObject(const std::string& name);

    std::string name = "SceneObject";
    Transform transform;

    void Update();

    json ToJson() const override;
    void FromJson(const json& j) override;

    uint64_t GetID() const { return m_ID; }
    uint64_t GetParentID() const { return m_parentID; }

    Component* AddComponent(COMPONENT_TYPE componentType);
    bool AddComponent(std::unique_ptr<Component> component);
    bool RemoveComponent(COMPONENT_TYPE componentType);
    bool HasComponent(COMPONENT_TYPE componentType);
    const std::vector<std::unique_ptr<Component>>& GetComponents() const { return m_components; }

    void SetIsActive(const bool value) { m_isActive = value; }
    bool IsActive() const { return m_isActive; }
    bool IsActiveInHierarchy() const;
    
    template<typename T>
    T* GetComponent()
    {
        for (auto& c : m_components)
        {
            T* componentCasted = dynamic_cast<T*>(c.get());
            if (componentCasted) return componentCasted;
        }
        return nullptr;
    }

    private:
    uint64_t m_ID;
    uint64_t m_parentID;
    std::vector<std::unique_ptr<Component>> m_components;
    bool m_isActive = true;
};