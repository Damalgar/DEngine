#include "Core/TagManager.h"
#include "Core/SceneManager.h"
#include "Core/Scene.h"

void TagManager::Init()
{
    AddTag("Default");
}

bool TagManager::AddTag(const std::string& tag)
{
    if (std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end())
        return false;

    m_tags.push_back(tag);
    m_tagVisibility[tag] = true;

    std::sort(m_tags.begin(), m_tags.end(), m_sortFunc);
    return true;
}

bool TagManager::RemoveTag(const std::string& tag)
{
    if (tag == "Default")
        return false;

    auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it == m_tags.end())
        return false;

    m_tags.erase(it);
    m_tagVisibility.erase(tag);

    if (SceneManager::GetActiveScene())
        SceneManager::GetActiveScene()->OnTagDeleted(tag);

    return true;
}

bool TagManager::ToggleTagVisibility(const std::string& tag)
{
    if (m_tagVisibility.find(tag) == m_tagVisibility.end())
        return false;

    m_tagVisibility[tag] = !m_tagVisibility[tag];
    return true;
}

bool TagManager::SetTagVisibility(const std::string& tag, bool visible)
{
    if (m_tagVisibility.find(tag) == m_tagVisibility.end())
        return false;

    m_tagVisibility[tag] = visible;
    return true;
}

bool TagManager::IsTagVisible(const std::string& tag)
{
    if (m_tagVisibility.find(tag) == m_tagVisibility.end())
        return false;

    return m_tagVisibility[tag];
}

const std::vector<std::string>& TagManager::GetTags() { return m_tags; }