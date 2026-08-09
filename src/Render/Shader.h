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

using namespace glm;

struct Shader : public ISerializable{

    Shader(std::string vertexShaderFilename = "", std::string fragmentShaderFilename = "", std::string name = "Shader")
    {
        if (vertexShaderFilename == "")
            vertexShaderFilename = "DefaultVertex";

        if (fragmentShaderFilename == "")
            fragmentShaderFilename = "DefaultFragment";

        m_vertexShaderName = vertexShaderFilename;
        m_fragmentShaderName = fragmentShaderFilename;
        m_name = name;
        m_ID = 0;

        Compile();
    }

    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }

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

    json ToJson() const override
    {
        json j;
        j["vertexShader"] = m_vertexShaderName;
        j["fragmentShader"] = m_fragmentShaderName;
        return j;
    }

    void FromJson(const json& j) override
    {
        m_vertexShaderName = j.value("vertexShader", "DefaultVertex");
        m_fragmentShaderName = j.value("fragmentShader", "DefaultFragment");
        Compile();
    }

    private:
    GLuint m_ID;
    std::string m_name;
    std::string m_vertexShaderName;
    std::string m_fragmentShaderName;

    void Compile()
    {
        if (m_ID != 0)
        {
            glDeleteProgram(m_ID);
            m_ID = 0;
        }

        std::string finalVertexPath = FileSystem::GetVertexShaderPath(m_vertexShaderName);
        if (finalVertexPath == "") 
        {
            m_vertexShaderName = "DefaultVertex";
            finalVertexPath = FileSystem::GetVertexShaderPath(m_vertexShaderName);
        }

        std::string finalFragmentPath = FileSystem::GetFragmentShaderPath(m_fragmentShaderName);
        if (finalFragmentPath == "") 
        {
            m_fragmentShaderName = "DefaultFragment";
            finalFragmentPath = FileSystem::GetFragmentShaderPath(m_fragmentShaderName);
        }

        m_ID = glCreateProgram();

        std::string defaultVertexShaderStr = LoadShaderAsString(finalVertexPath.c_str());
        std::string defaultFragmentShaderStr = LoadShaderAsString(finalFragmentPath.c_str());

        const char* defaultVertexShaderCode = defaultVertexShaderStr.c_str();
        const char* defaultFragmentShaderCode = defaultFragmentShaderStr.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(vertexShader, 1, &defaultVertexShaderCode, nullptr);
        glShaderSource(fragmentShader, 1, &defaultFragmentShaderCode, nullptr);

        GLint success;
        GLchar infoLog[512];

        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            Console::LogWarn("error loading Vertex Shader (" + m_vertexShaderName + "):\n\t" + infoLog);
        }

        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            Console::LogWarn("error loading Fragment Shader (" + m_fragmentShaderName + "):\n\t" + infoLog);
        }

        glAttachShader(m_ID, vertexShader);
        glAttachShader(m_ID, fragmentShader);

        glLinkProgram(m_ID);
        glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
            Console::LogWarn(std::string("error linking shader program:\n\t") + infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    std::string LoadShaderAsString(const char* filepath)
    {
        std::ifstream stream(filepath);

        if (!stream.is_open())
        {
            Console::LogWarn(std::string("file not found ") + filepath);
            return "";
        }

        std::stringstream buffer;
        buffer << stream.rdbuf();
        stream.close();
        return buffer.str();
    }
};