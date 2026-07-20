#pragma once

#include <glad/glad.h>
#include "Graphics/Vertex.h"
#include <array>

struct Cube
{
    Cube() : Cube(GetDefaultVertices()) {}

    Cube(const std::array<Vertex, 24>& vertices)
        : m_vertices{vertices}
    {
        // 36 Indici (6 facce * 2 triangoli * 3 vertici)
        m_indices = {
            0,  1,  2,  2,  3,  0,  // Front
            4,  5,  6,  6,  7,  4,  // Back
            8,  9,  10, 10, 11, 8,  // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20  // Bottom
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(5);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
    }

    void Draw()
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Cube()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

private:
    std::array<Vertex, 24> m_vertices;
    std::array<GLuint, 36> m_indices;

    GLuint vbo = 0;
    GLuint vao = 0;
    GLuint ebo = 0;

    static std::array<Vertex, 24> GetDefaultVertices()
    {
        using namespace glm;
        return {
            // FACCIA FRONTALE (Normale +Z)
            Vertex(vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)), // Basso Sinistra
            Vertex(vec3( 0.5f, -0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)), // Basso Destra
            Vertex(vec3( 0.5f,  0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)), // Alto Destra
            Vertex(vec3(-0.5f,  0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)), // Alto Sinistra

            // FACCIA POSTERIORE (Normale -Z)
            Vertex(vec3( 0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)), 
            Vertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)), 
            Vertex(vec3(-0.5f,  0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)), 
            Vertex(vec3( 0.5f,  0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

            // FACCIA SINISTRA (Normale -X)
            Vertex(vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
            Vertex(vec3(-0.5f, -0.5f,  0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),
            Vertex(vec3(-0.5f,  0.5f,  0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
            Vertex(vec3(-0.5f,  0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

            // FACCIA DESTRA (Normale +X)
            Vertex(vec3( 0.5f, -0.5f,  0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
            Vertex(vec3( 0.5f, -0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),
            Vertex(vec3( 0.5f,  0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
            Vertex(vec3( 0.5f,  0.5f,  0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

            // FACCIA SUPERIORE (Normale +Y)
            Vertex(vec3(-0.5f,  0.5f,  0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
            Vertex(vec3( 0.5f,  0.5f,  0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
            Vertex(vec3( 0.5f,  0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
            Vertex(vec3(-0.5f,  0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)),

            // FACCIA INFERIORE (Normale -Y)
            Vertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
            Vertex(vec3( 0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),
            Vertex(vec3( 0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
            Vertex(vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f))
        };
    }
};