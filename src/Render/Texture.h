#pragma once
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>
#include <string>

class Texture {
    public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    std::string GetPath() const { return m_filePath; }

    private:
    GLuint m_textureID;
    std::string m_filePath;
    int m_width, m_height, m_channels;
};