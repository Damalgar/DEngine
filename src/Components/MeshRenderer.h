#pragma once
#include "Components/Component.h"
#include "Render/Model.h"
#include <variant>

using UniformValue = std::variant<float, int, vec3, vec4>;

class Material;

class MeshRenderer : public Component {
    public:
    MeshRenderer(SceneObject* sceneObject, Model* model = nullptr, Material* material = nullptr);

    void Update() override {}
    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos);

    void SetModel(Model* model) { m_model = model; }
    Model* GetModel() const { return m_model; }
    void SetMeshIndices(const std::vector<unsigned int>& indices) { m_meshIndices = indices; }

    Material* GetMaterial() const { return m_material; }
    void SetMaterial(Material* material) { m_material = material; };

    json ToJson() const override;
    void FromJson(const json& j) override;

    void OnGuiDraw() override;
    
    BoundingBox GetGlobalBoundingBox();

    void SetFloatOverride(const std::string& name, float value) { m_materialOverrides[name] = value; }
    void SetIntOverride(const std::string& name, int value) { m_materialOverrides[name] = value; }
    void SetVec3Override(const std::string& name, const vec3& value) { m_materialOverrides[name] = value; }
    void SetVec4Override(const std::string& name, const vec4& value) { m_materialOverrides[name] = value; }

    void ClearOverride(const std::string& name) { m_materialOverrides.erase(name); }
    void ClearAllOverrides() { m_materialOverrides.clear(); }

    private:
    Model* m_model;
    std::vector<unsigned int> m_meshIndices;

    Material* m_material = nullptr;
    std::unordered_map<std::string, UniformValue> m_materialOverrides;
};