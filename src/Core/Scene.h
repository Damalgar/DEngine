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
    
    void Update();
    void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos);

    void AddObject(SceneObject* object);
    SceneObject* CreateEmptyObject(SceneObject* parent = nullptr);
    SceneObject* CreateCubeObject(SceneObject* parent = nullptr);

    void RemoveObject(SceneObject* object);
    void RemoveObject(int index);
    void CollectHierarchy(SceneObject* root, std::vector<SceneObject*>& hierarchy);
    
    json ToJson() const override;
    void FromJson(const json& j) override;

    void OnTagDeleted(const std::string& tag);

    SceneObject* InstantiateModelNode(Model* model, const ModelNode& node, SceneObject* parentObject);

    void SetShowBoundingBoxes(const bool value) { m_showBoundingBoxes = value; }
    void ToggleShowBoundingBoxes() { SetShowBoundingBoxes(!m_showBoundingBoxes); }
    bool GetShowBoundingBoxes() const { return m_showBoundingBoxes; }

    const mat4& GetViewMatrix() const { return m_currentViewMatrix; }
    const mat4& GetProjectionMatrix() const { return m_currentProjectionMatrix; }

    private:
    std::vector<SceneObject*> m_sceneObjects;
    std::string m_name;

    mat4 m_currentViewMatrix = mat4(1.0f);
    mat4 m_currentProjectionMatrix = mat4(1.0f);
    bool m_showBoundingBoxes = false;
};