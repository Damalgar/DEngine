#pragma once
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "IO/FileSystem.h"
#include "IO/Console.h"
#include "Core/ISerializable.h"
#include "Core/AssetManager.h"

using namespace glm;

struct Shader {

    enum class SHADER_TYPE { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    Shader(std::string filename = "Default", bool readFromFile = true)
    {
        std::string shadersFolder = FileSystem::GetAssetPath("User", "Shaders");
        if (!fs::exists(shadersFolder))
            fs::create_directory(shadersFolder);

        std::string actualFilename = filename;
        fs::path finalPath = fs::path(shadersFolder) / (actualFilename + ".shader");

        if (!readFromFile)
        {
            int counter = 0;
            while (fs::exists(finalPath))
            {
                counter++;
                std::ostringstream oss;
                oss << filename << "_" << std::setw(3) << std::setfill('0') << counter;
                actualFilename = oss.str();
                finalPath = fs::path(fs::path(shadersFolder) / (actualFilename + ".shader")).string();
            }

            std::ofstream file(finalPath);
            if (file.is_open())
            {
file << R"(#pragma VERTEX
#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

out vec2 fragTextCoords;
out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
    //M.V.P
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1);
    fragTextCoords = textCoords;
    fragNormal = normalize(normalMatrix * normal);
    fragWorldPos = vec3(modelMatrix * vec4(pos, 1.0));
}
#pragma FRAGMENT
#version 330 core

struct Material {
    vec4 tintColor;
    float shininess;
    int hasColorMap;
    int hasSpecularMap;
};

out vec4 FragColor;

in vec3 fragWorldPos;
in vec2 fragTextCoords;
in vec3 fragNormal;

uniform vec3 viewPos;

uniform Material material;
uniform sampler2D colorMap;
uniform sampler2D specularMap;

void main()
{
    //===LIGHTNING===

    //variables
    vec4 albedo = material.hasColorMap == 1 ? texture(colorMap, fragTextCoords) * material.tintColor : material.tintColor;
    float specularIntensity = material.hasSpecularMap == 1 ? texture(specularMap, fragTextCoords).r : 1.0;
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float Kamb = 0.1;

    vec3 N = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.5));
    vec3 L = -lightDir;
    vec3 V = normalize(viewPos - fragWorldPos);
    vec3 H = normalize(V + L);

    //ambient
    vec3 ambientLight = Kamb * albedo.rgb;

    //diffuse
    float diff = max(0.0, dot(N,L));
    vec3 diffLight = albedo.rgb * diff;

    //specular
    float spec = pow(max(0.0, dot(H, N)), material.shininess);
    vec3 specLight = lightColor * (spec * specularIntensity);

    vec3 finalColor = diffLight + ambientLight + specLight;
    FragColor = vec4(finalColor, 1.0);
})";
                file.close();
                Console::LogInfo("Created new Shader " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
                }
                else
                    Console::LogError("Cannot create Shader at " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
            }

        std::ifstream stream(finalPath.string());
        if (!stream.is_open())
            return;

        std::string line;
        std::stringstream ss[2];
        SHADER_TYPE type = SHADER_TYPE::NONE;
        m_shaderCode = "";

        while (std::getline(stream, line))
        {
            if (line.find("#pragma VERTEX") != std::string::npos)
                type = SHADER_TYPE::VERTEX;
            else if (line.find("#pragma FRAGMENT") != std::string::npos)
                type = SHADER_TYPE::FRAGMENT;
            else {
                if (type == SHADER_TYPE::VERTEX)
                    ss[0] << line << "\n";
                else if (type == SHADER_TYPE::FRAGMENT)
                    ss[1] << line << "\n";
                m_shaderCode += line +=  "\n";
            }
        }

        m_vertexCode = ss[0].str();
        m_fragmentCode = ss[1].str();
        m_name = actualFilename;

        Compile();
    }

    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }

    std::string GetVertexCode() const { return m_vertexCode; }
    std::string GetFragmentCode() const { return m_fragmentCode; }

    void SetVertexCode(const std::string& vertexCode) { m_vertexCode = vertexCode; }
    void SetFragmentCode(const std::string& fragmentCode) { m_fragmentCode = fragmentCode; }

    void Use() const { glUseProgram(m_ID); }

    void SetVec4(std::string uniformName, vec4 value) { SetVec4(uniformName, value.x, value.y, value.z, value.w); }
    void SetVec4(std::string unifromName, vec3 value, float w = 1.0) { SetVec4(unifromName, value.x, value.y, value.z, w); }
    void SetVec4(std::string uniformName, float x, float y, float z, float w)
    {
        GLuint uniformID = glGetUniformLocation(m_ID, uniformName.c_str());
        glUniform4f(uniformID, x, y, z, w);
    }

    void SetVec3(std::string uniformName, vec3 value) { SetVec3(uniformName, value.x, value.y, value.z); } 
    void SetVec3(std::string uniformName, float x, float y, float z)
    {
        GLuint uniformID = glGetUniformLocation(m_ID, uniformName.c_str());
        glUniform3f(uniformID, x, y, z);
    }

    void SetMat4(std::string uniformName, glm::mat4 mat)
    {
        GLuint uniformID = glGetUniformLocation(m_ID, uniformName.c_str());
        glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void SetMat3(std::string uniformName, glm::mat3 mat)
    {
        GLuint uniformID = glGetUniformLocation(m_ID, uniformName.c_str());
        glUniformMatrix3fv(uniformID, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void SetInt(std::string uniformName, int value)
    {
        GLuint uniformID = glGetUniformLocation(m_ID, uniformName.c_str());
        glUniform1i(uniformID, value);
    }

    void SetFloat(std::string uniformName, float value)
    {
        GLuint uniformID = glGetUniformLocation(m_ID, uniformName.c_str());
        glUniform1f(uniformID, value);
    }

    bool TryRecompileAndSave()
    {
        if (!Compile())
            return false;

        std::string shadersFolder = FileSystem::GetAssetPath("User", "Shaders");
        fs::path finalPath = fs::path(shadersFolder) / (m_name + ".shader");

        std::ofstream file(finalPath.string());
        if (file.is_open())
        {
            file << "#pragma VERTEX\n";
            file << m_vertexCode;
            
            if (!m_vertexCode.empty() && m_vertexCode.back() != '\n')
                file << "\n";
                
            file << "#pragma FRAGMENT\n";
            file << m_fragmentCode;
            
            file.close();
            Console::LogInfo("Shader compiled and saved successfully: " + m_name, LOG_CATEGORY::ASSETMANAGER);
            return true;
        }
        
        Console::LogError("Failed to write Shader to disk: " + finalPath.string(), LOG_CATEGORY::ASSETMANAGER);
        return false;
    }

    private:
    GLuint m_ID;
    std::string m_name;

    std::string m_vertexCode;
    std::string m_fragmentCode;
    std::string m_shaderCode;

    bool Compile()
    {
        const char* vertexShaderCode = m_vertexCode.c_str();
        const char* fragmentShaderCode = m_fragmentCode.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
        glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);

        GLint success;
        GLchar infoLog[1024];

        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            Console::LogWarn("error loading Vertex Shader:\n\t" + std::string(infoLog));
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            Console::LogWarn("error loading Fragment Shader:\n\t" + std::string(infoLog));
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        GLuint tempProgramID = glCreateProgram();
        glAttachShader(tempProgramID, vertexShader);
        glAttachShader(tempProgramID, fragmentShader);

        glLinkProgram(tempProgramID);
        glGetProgramiv(tempProgramID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(tempProgramID, 512, nullptr, infoLog);
            Console::LogWarn(std::string("error linking shader program:\n\t") + infoLog);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(tempProgramID);
            return false;
        }

        if (m_ID != 0)
            glDeleteProgram(m_ID);

        m_ID = tempProgramID;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        return true;
    }
};