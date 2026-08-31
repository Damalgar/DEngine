#pragma once
#include "Components/Component.h"
#include "Vendor/glm/glm.hpp"
using namespace glm;

class MountPoint : public Component {
    public:
    MountPoint(SceneObject* sceneObject) : Component(sceneObject, COMPONENT_TYPE::MOUNT_POINT) {}

    void Update() override {}
    void OnGuiDraw() override;

    json ToJson() const override;
    void FromJson(const json& j) override;

    vec4 GetGizmosTintColor() const { return m_gizmosTintColor; }
    void SetGizmosTintColor(const vec4& tintColor) { m_gizmosTintColor = tintColor; }

    void OnDrawGizmos(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos) const override;

    private:
    vec4 m_gizmosTintColor = vec4(1.0);
};