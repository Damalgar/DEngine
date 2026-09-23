#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include "Core/SceneObject.h"
#include "Core/ISerializable.h"
#include "Core/AssetManager.h"
#include "Render/Model.h"

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
    SceneObject* GetSceneObjectByID(uint64_t ID);
    SceneObject* InstantiatePreset(const std::string& name, SceneObject* targetMountPoint = nullptr);
    
    json ToJson() const override;
    void FromJson(const json& j) override;

    void OnTagDeleted(const std::string& tag);

    SceneObject* InstantiateModelNode(Model* model, const ModelNode& node, SceneObject* parentObject);

    void SetDrawGizmos(const bool value) { m_drawGizmos = value; }
    void ToggleDrawGizmos() { SetDrawGizmos(!m_drawGizmos); }
    bool GetDrawGizmos() const { return m_drawGizmos; }

    const mat4& GetViewMatrix() const { return m_currentViewMatrix; }
    const mat4& GetProjectionMatrix() const { return m_currentProjectionMatrix; }

    bool SavePreset(SceneObject* obj, const std::string& name);

    void OnModelDeleted(Model* model);
    void OnMaterialDeleted(Material* mat);

    private:
    std::vector<SceneObject*> m_sceneObjects;
    std::string m_name;

    mat4 m_currentViewMatrix = mat4(1.0f);
    mat4 m_currentProjectionMatrix = mat4(1.0f);
    
    bool m_drawGizmos = false;
};