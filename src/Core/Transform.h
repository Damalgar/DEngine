#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/ISerializable.h"
using namespace glm;

class SceneObject;

class Transform : public ISerializable {
    public:
    Transform(SceneObject* sceneObject, Transform* parent = nullptr);
    
    void SetPosition(const vec3 position) { m_position = position; SetDirty(); }
    void SetScale(const vec3 scale) { m_scale = scale; SetDirty(); }

    void SetRotation(const vec3& eulerDegrees);
    void SetRotation(const quat& quatRot);
    
    void SetParent(Transform* newParent, bool keepWorldTransform = true);
    void AddChild(Transform* child);
    void RemoveChild(Transform* child);

    vec3 GetPosition() const { return m_position; }
    vec3 GetScale() const { return m_scale; }

    vec3 GetEulerAngles() const { return m_eulerAngles; }
    quat GetRotation() const { return m_rotation; }

    Transform* GetParent() const { return m_parent; }

    const std::vector<Transform*>& GetChildren() const { return m_children; }
    std::vector<Transform*>& GetChildren() { return m_children; }

    glm::mat4& GetModelMatrix() { UpdateMatrices(); return m_modelMatrix; }
    glm::mat3& GetNormalMatrix() { UpdateMatrices(); return m_normalMatrix; }

    void SetSceneObject(SceneObject* sceneObject) { m_sceneObject = sceneObject; }
    SceneObject* GetSceneObject() const { return m_sceneObject; }

    json ToJson() const override;
    void FromJson(const json& j) override;

    bool IsDescendantOf(Transform* potentialAncestor) const;

    private:
    vec3 m_position;
    quat m_rotation;
    vec3 m_eulerAngles;
    vec3 m_scale;

    mat4 m_modelMatrix;
    mat3 m_normalMatrix;

    Transform* m_parent = nullptr;
    std::vector<Transform*> m_children;
    SceneObject* m_sceneObject = nullptr;

    bool m_isDirty = false;

    void UpdateMatrices();
    void SetDirty();
};