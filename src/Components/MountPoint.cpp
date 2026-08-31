#include "Components/MountPoint.h"
#include "Utils/Utils.h"
#include "Tools/drawUtils.h"
#include "Core/SceneObject.h"

void MountPoint::OnGuiDraw()
{
    ImGui::ColorEdit4("tint", &m_gizmosTintColor.x);
}

json MountPoint::ToJson() const
{
    json j = Component::ToJson();
    j["gizmosTintColor"] = m_gizmosTintColor;
    return j;
}

void MountPoint::FromJson(const json& j)
{
    m_gizmosTintColor = j.contains("gizmosTintColor") ? j["gizmosTintColor"].get<vec4>() : vec4(1.0f);
}

void MountPoint::OnDrawGizmos(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& viewPos) const
{
    Utils::DrawWireSphere(m_sceneObject->transform.GetPosition(), 0.2f, m_gizmosTintColor, viewMatrix, projectionMatrix);
}