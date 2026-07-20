#include "Material.h"
#include "Core/AssetManager.h"

Material::Material(Shader* shader, Texture* colorMap, Texture* specularMap, float shininess, vec4 tintColor, std::string name)
{
    m_shader = shader;
    m_colorMap = colorMap;
    m_specularMap = specularMap;
    m_shininess = shininess;
    m_tintColor = tintColor;
    m_name = name;
}

void Material::Apply()
{
    if (!m_shader) return;

    m_shader->Use();

    m_shader->SetFloat("material.shininess", m_shininess);
    m_shader->SetVec4("material.tintColor", m_tintColor);

    if (m_colorMap)
    {
        m_colorMap->Bind(0);
        m_shader->SetInt("material.hasColorMap", 1);
        m_shader->SetInt("colorMap", 0);
    } else 
    {
        m_shader->SetInt("material.hasColorMap", 0);
    }

    if (m_specularMap)
    {
        m_specularMap->Bind(1);
        m_shader->SetInt("material.hasSpecularMap", 1);
        m_shader->SetInt("specularMap", 1);
    } else 
    {
        m_shader->SetInt("material.hasSpecularMap", 0);
    }
}

void Material::FromJson(const json& j)
{
    m_name = j.contains("name") ? j["name"].get<std::string>() : "Unknown Material";
    m_shininess = j.contains("shininess") ? j["shininess"].get<float>() : 0.0f;
    m_tintColor = j.contains("tintColor") ? j["tintColor"].get<vec4>() : vec4(1.0f);

    std::string colorMapName = j.value("colorMap", "");
    if (!colorMapName.empty())
        m_colorMap = new Texture(j["colorMap"].get<std::string>());
    else
        m_colorMap = nullptr;

    std::string specularMapName = j.value("specularMap", "");
    if (!specularMapName.empty())
        m_specularMap = new Texture(j["specularMap"].get<std::string>());
    else
        m_specularMap = nullptr;

    std::string shaderName = j.value("shader", "DefaultShader");
    m_shader = AssetManager::GetShader(shaderName);
}

json Material::ToJson() const
{
    json j;
    j["name"] = m_name;
    j["colorMap"] = m_colorMap ? m_colorMap->GetPath() : "";
    j["specularMap"] = m_specularMap ? m_specularMap->GetPath() : "";
    j["shininess"] = m_shininess;
    j["tintColor"] = m_tintColor;
    j["shader"] = m_shader ? m_shader->GetName() : "DefaultShader";
    return j;
}