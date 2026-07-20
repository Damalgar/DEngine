#pragma once

#include "Components/Transform.h"
#include "Render/Material.h"
#include "Components/Model.h"
#include "Core/ISerializable.h"

class SceneObject : public ISerializable {
    public:
    SceneObject();
    SceneObject(const std::string& name, Model* model, Material* material);

    std::string name = "SceneObject";
    Transform transform;

    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos);

    Material* GetMaterial() { return m_material; }
    void SetMaterial(Material* material) { m_material = material; }

    json ToJson() const override;
    void FromJson(const json& j) override;

    uint64_t GetID() const { return m_ID; }
    uint64_t GetParentID() const { return m_parentID; }

    void SetMeshIndices(const std::vector<unsigned int>& indices) { m_meshIndices = indices; }

    private:
    Model* m_model = nullptr;
    Material* m_material = nullptr;
    uint64_t m_ID;
    uint64_t m_parentID;
    std::vector<unsigned int> m_meshIndices;
};