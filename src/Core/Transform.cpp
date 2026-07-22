#include "Core/Transform.h"
#include <iostream>

Transform::Transform(SceneObject* sceneObject, Transform* parent)
    : m_position(0.0f), m_rotation(0.0f), m_scale(1.0f), m_modelMatrix(1.0f), m_normalMatrix(1.0f), m_sceneObject(sceneObject)
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
        Utils::LocalFromGlobal(oldGlobalMatrix, parentGlobal, m_position, m_rotation, m_scale);
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

    mat4 localMatrix = Utils::GetModelMatrix(m_position, m_rotation, m_scale);
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
    m_position = j.contains("position") ? j["position"].get<vec3>() : vec3(1.0f);
    m_rotation = j.contains("rotation") ? j["rotation"].get<vec3>() : vec3(1.0f);
    m_scale = j.contains("scale") ? j["scale"].get<vec3>() : vec3(1.0f);
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