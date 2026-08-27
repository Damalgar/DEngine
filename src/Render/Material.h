#pragma once

#include "Render/Texture.h"
#include "Render/Shader.h"
#include <glm/glm.hpp>
#include "Core/ISerializable.h"

class Material : public ISerializable {
    public:
    Material(Shader* shader, Texture* colorMap, Texture* specularMap, float shininess, vec4 tintColor = vec4(1), std::string name = "New Material");
    Material(Material* other);

    Shader* GetShader() const { return m_shader; }
    Texture* GetColorMap() const { return m_colorMap; }
    Texture* GetSpecularMap() const { return m_specularMap; }
    float GetShininess() const { return m_shininess; }
    vec4 GetTintColor() const { return m_tintColor; }
    std::string GetName() const { return m_name; }

    void SetColorMap(Texture* texture) { m_colorMap = texture; }
    void SetSpecularMap(Texture* texture) { m_specularMap = texture; }
    void SetName(const std::string& name) { m_name = name; }
    void SetTintColor(const vec4 tintColor) { m_tintColor = tintColor; }
    void SetShininess(const float value) { m_shininess = value; }
    void SetShader(Shader* shader) { m_shader = shader; }

    void Apply();
    json ToJson() const override;
    void FromJson(const json& j) override;

    Material* Clone() { return new Material(*this); }

    private:
    Shader* m_shader;
    Texture* m_colorMap; //Albedo/Diffuse
    Texture* m_specularMap;
    bool m_isDirty = false;
    std::string m_name;

    float m_shininess = 0;
    vec4 m_tintColor;
};