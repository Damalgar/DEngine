#pragma once
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

class Texture {
    public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    std::string GetPath() const { return m_filePath; }
    std::string GetName() const { return fs::path(m_filePath).filename().stem().string(); }

    private:
    GLuint m_textureID;
    std::string m_filePath;
    int m_width, m_height, m_channels;
};