#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include "Core/SceneObject.h"
#include "Core/ISerializable.h"
#include "Core/AssetManager.h"

class Scene : public ISerializable {
    public:
    Scene(std::string name = "Scene");
    ~Scene();

    const std::vector<SceneObject*>& GetSceneObjects() { return m_sceneObjects; }
    
    void SetName(const std::string& name) { m_name = name; }
    std::string GetName() const { return m_name; }
    
    void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos);
    void AddObject(SceneObject* object);
    void RemoveObject(SceneObject* object);
    void RemoveObject(int index);
    
    json ToJson() const override;
    void FromJson(const json& j) override;

    SceneObject* InstantiateModelNode(Model* model, const ModelNode& node, SceneObject* parentObject);

    private:
    std::vector<SceneObject*> m_sceneObjects;
    std::string m_name;
};