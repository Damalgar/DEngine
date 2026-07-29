#pragma once
#include <glad/glad.h>
#include "Graphics/Vertex.h"
#include "Physics/BoundingBox.h"

#include <vector>

class Mesh {
    public:

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void Draw() const;
    void SetBoundingBox(BoundingBox& box);
    const BoundingBox& GetBoundingBox() const { return m_boundingBox; }
    

    private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;

    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    BoundingBox m_boundingBox;

    void SetupMesh();
};