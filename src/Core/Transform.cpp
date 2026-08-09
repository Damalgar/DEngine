#include "Core/Transform.h"
#include "Utils/Utils.h"
#include <iostream>

Transform::Transform(SceneObject* sceneObject, Transform* parent)
    : m_position(0.0f), m_rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), m_eulerAngles(0.0f), m_scale(1.0f),
        m_modelMatrix(1.0f), m_normalMatrix(1.0f), m_sceneObject(sceneObject)
{
    if (parent)
        SetParent(parent);

    m_isDirty = false;
    SetDirty();
}

void Transform::SetDirty()
{
    if (m_isDirty) return;

    m_isDirty = true;

    for (Transform* child : m_children)
        child->SetDirty();
}

void Transform::SetParent(Transform* newParent, bool keepWorldTransform)
{
    if (m_parent == newParent) return;

    mat4 oldGlobalMatrix = mat4(1.0f);

    if (keepWorldTransform)
        oldGlobalMatrix = GetModelMatrix();

    if (m_parent)
        m_parent->RemoveChild(this);

    m_parent = newParent;
    if (m_parent)
        m_parent->AddChild(this);

    if (keepWorldTransform)
    {
        mat4 parentGlobal = m_parent ? m_parent->GetModelMatrix() : mat4(1.0f);
        vec3 newPos, newScale;
        quat newRot;
        Utils::LocalFromGlobal(oldGlobalMatrix, parentGlobal, newPos, newRot, newScale);
        SetPosition(newPos);
        SetRotation(newRot); 
        SetScale(newScale);
    }

    SetDirty();
}

void Transform::AddChild(Transform* child)
{
    if (std::find(m_children.begin(), m_children.end(), child) == m_children.end())
        m_children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
}

void Transform::UpdateMatrices()
{
    if (!m_isDirty)
        return;

    mat4 localMatrix = glm::translate(mat4(1.0f), m_position) * glm::toMat4(m_rotation) * glm::scale(mat4(1.0f), m_scale);
    if (m_parent) 
        m_modelMatrix = m_parent->GetModelMatrix() * localMatrix;
    else
        m_modelMatrix = localMatrix;
    
    m_normalMatrix = Utils::GetNormalMatrix(m_modelMatrix);

    m_isDirty = false;
}

json Transform::ToJson() const
{
    json j;
    j["position"] = m_position;
    j["rotation"] = m_rotation;
    j["scale"] = m_scale;
    return j;
}

void Transform::FromJson(const json& j)
{
    m_position = j.contains("position") ? j["position"].get<vec3>() : vec3(0.0f);
    m_scale = j.contains("scale") ? j["scale"].get<vec3>() : vec3(1.0f);

    if (j.contains("rotation"))
    {
        if (j["rotation"].size() == 4) 
        {
            //saved in quaternion
            m_rotation = j["rotation"].get<quat>();
            m_eulerAngles = degrees(eulerAngles(m_rotation));
        } else if (j["rotation"].size() == 3) {
            //saved in euler angles
            m_eulerAngles = j["rotation"].get<vec3>();
            m_rotation = quat(radians(m_eulerAngles));
        } else {
            m_rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
            m_eulerAngles = vec3(0.0f);
        }
    }

    m_isDirty = true;
}

bool Transform::IsDescendantOf(Transform* potentialAncestor) const
{
    if (!potentialAncestor) return false;

    Transform* current = m_parent;
    while (current != nullptr)
    {
        if (current == potentialAncestor)
            return true;
        current = current->GetParent();
    }
    return false;
}

void Transform::SetRotation(const vec3& eulerDegrees)
{
    m_eulerAngles = eulerDegrees;
    m_rotation = quat(radians(m_eulerAngles));
    SetDirty();
}

void Transform::SetRotation(const quat& quatRot)
{
    m_rotation = quatRot;
    m_eulerAngles = degrees(eulerAngles(m_rotation));
    SetDirty();
}

