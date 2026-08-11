#pragma once
#include <string>
#include <iostream>
#include "Vendor/json.hpp"
#include "Core/ISerializable.h"
#include "Tools/drawUtils.h"
#include "IO/Console.h"

class SceneObject;

enum class COMPONENT_TYPE
{
    TELEMETRY_VIEWER = 0,
    MESH_RENDERER
};

class Component : public ISerializable
{
    public:
    Component(SceneObject* sceneObject, COMPONENT_TYPE componentType) : m_componentType(componentType)
    {
        m_name = GetNameByComponentType(m_componentType);

        if (sceneObject == nullptr)
        {
            Console::LogError("added component to a null sceneObject");
            return;
        }

        m_sceneObject = sceneObject;
    }
    
    virtual ~Component() = default;
    virtual void OnGuiDraw() = 0;
    virtual void Update() = 0;

    virtual SceneObject* GetSceneObject() const { return m_sceneObject; }
    std::string GetName() const { return m_name; }
    COMPONENT_TYPE GetComponentType() const { return m_componentType; }
    bool IsOfType(COMPONENT_TYPE type) const { return m_componentType == type; }

    std::string GetNameByComponentType(COMPONENT_TYPE type)
    {
        switch (type)
        {
        case COMPONENT_TYPE::TELEMETRY_VIEWER: return "Telemetry Viewer";
        case COMPONENT_TYPE::MESH_RENDERER: return "Mesh Renderer"; 
        
        default: return "Unknown component";
        }
    }

    virtual json ToJson() const override
    {
        json j;
        j["name"] = m_name;
        j["componentType"] = static_cast<int>(m_componentType);
        return j;
    }

    virtual void FromJson(const json& j) override
    {
        m_name = j.contains("name") ? j["name"].get<std::string>() : "Unknown component";
        m_componentType = j.contains("componentType") ? static_cast<COMPONENT_TYPE>(j["componentType"].get<int>()) : COMPONENT_TYPE::TELEMETRY_VIEWER;
    }

    inline static const std::vector<ComboEntry<COMPONENT_TYPE>> availableComponentsOptions = {
        { "Mesh Renderer", COMPONENT_TYPE::MESH_RENDERER },
        { "Telemetry Viewer", COMPONENT_TYPE::TELEMETRY_VIEWER }
    };

    protected:
    std::string m_name = "";
    SceneObject* m_sceneObject = nullptr;
    COMPONENT_TYPE m_componentType;
};