#pragma once
#include <glad/glad.h>

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;
    void Rescale(int width, int height);
    unsigned int GetTextureID() const { return m_textureID; }

private:
    GLuint m_fbo;
    GLuint m_textureID;
    GLuint m_rbo;
    int m_width;
    int m_height;
};