#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include <iostream>

#include "Components/Mesh.h"

struct ModelNode {
    std::string name;
    glm::mat4 localTransform;
    std::vector<unsigned int> meshIndices;
    std::vector<ModelNode> children;
};

class Model {
    public:
    Model(const std::string& dir, const std::string& name, const std::vector<Mesh>& m, const ModelNode& root)
        : m_dir(dir), m_name(name), m_meshes(m), m_rootNode(root) {}

    std::string GetDir() const { return m_dir; }
    std::string GetName() const { return m_name; }
    const ModelNode& GetRootNode() const { return m_rootNode; }
    const Mesh* GetMesh(unsigned int index) const { return &m_meshes[index]; }

    private:
    ModelNode m_rootNode;
    std::vector<Mesh> m_meshes;
    std::string m_dir;
    std::string m_name;
};