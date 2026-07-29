#pragma once
#include "Components/Component.h"
#include "Render/Material.h"
#include "Render/Model.h"

class MeshRenderer : public Component {
    public:
    MeshRenderer(SceneObject* sceneObject, Model* model = nullptr, Material* material = nullptr);

    void Update() override {}
    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos);

    void SetModel(Model* model) { m_model = model; }
    Model* GetModel() const { return m_model; }
    void SetMeshIndices(const std::vector<unsigned int>& indices) { m_meshIndices = indices; }

    Material* GetSharedMaterial() { return m_sharedMaterial; }
    void SetSharedMaterial(Material* material);

    Material* GetMaterial();

    json ToJson() const override;
    void FromJson(const json& j) override;

    void OnGuiDraw() override;
    
    BoundingBox GetGlobalBoundingBox();

    private:
    Model* m_model;
    std::vector<unsigned int> m_meshIndices;

    Material* m_sharedMaterial = nullptr;
    std::unique_ptr<Material> m_instancedMaterial = nullptr;
};