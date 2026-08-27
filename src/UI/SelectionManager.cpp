#include "UI/SelectionManager.h"

#include "Core/SceneObject.h"
#include "Render/Material.h"
#include "Render/Texture.h"
#include "IO/Console.h"

using type = SELECTION_TYPE;

void SelectionManager::Deselect()
{
    m_selectedType = type::NONE;
    m_data = nullptr;
}

void SelectionManager::Select(SceneObject* obj)
{
    if (!obj)
    {
        Deselect();
        return;
    }

    m_selectedType = type::SCENE_OBJECT;
    m_data = obj;
}

void SelectionManager::Select(Material* mat)
{
    if (!mat)
    {
        Deselect();
        return;
    }

    m_selectedType = type::MATERIAL;
    m_data = mat;
}

void SelectionManager::Select(Texture* text)
{
    if (!text)
    {
        Deselect();
        return;
    }

    m_selectedType = type::TEXTURE;
    m_data = text;
}

void SelectionManager::Select(Shader* shader)
{
    if (!shader)
    {
        Deselect();
        return;
    }

    m_selectedType = type::SHADER;
    m_data = shader;
}

SceneObject* SelectionManager::GetAsSceneObject()
{
    if (m_selectedType != type::SCENE_OBJECT)
        return nullptr;

    return static_cast<SceneObject*>(m_data);
}

Material* SelectionManager::GetAsMaterial()
{
    if (m_selectedType != type::MATERIAL)
        return nullptr;

    return static_cast<Material*>(m_data);
}

Texture* SelectionManager::GetAsTexture()
{
    if (m_selectedType != type::TEXTURE)
        return nullptr;

    return static_cast<Texture*>(m_data);
}

Shader* SelectionManager::GetAsShader()
{
    if (m_selectedType != type::SHADER)
        return nullptr;

    return static_cast<Shader*>(m_data);
}