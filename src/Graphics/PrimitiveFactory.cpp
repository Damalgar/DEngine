#include "PrimitiveFactory.h"

Model* PrimitiveFactory::CreateCubeModel()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back({ vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) });
    vertices.push_back({ vec3( 0.5f, -0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) });
    vertices.push_back({ vec3( 0.5f,  0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f) });
    vertices.push_back({ vec3(-0.5f,  0.5f,  0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f) });

    vertices.push_back({ vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f) });
    vertices.push_back({ vec3( 0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f) });
    vertices.push_back({ vec3( 0.5f,  0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f) });
    vertices.push_back({ vec3(-0.5f,  0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f) });

    vertices.push_back({ vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f) });
    vertices.push_back({ vec3(-0.5f, -0.5f,  0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f) });
    vertices.push_back({ vec3(-0.5f,  0.5f,  0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f) });
    vertices.push_back({ vec3(-0.5f,  0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f) });

    vertices.push_back({ vec3(0.5f, -0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f) });
    vertices.push_back({ vec3(0.5f, -0.5f,  0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f) });
    vertices.push_back({ vec3(0.5f,  0.5f,  0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f) });
    vertices.push_back({ vec3(0.5f,  0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f) });

    vertices.push_back({ vec3(-0.5f, 0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f) });
    vertices.push_back({ vec3( 0.5f, 0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f) });
    vertices.push_back({ vec3( 0.5f, 0.5f,  0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f) });
    vertices.push_back({ vec3(-0.5f, 0.5f,  0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f) });

    vertices.push_back({ vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f) });
    vertices.push_back({ vec3( 0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f) });
    vertices.push_back({ vec3( 0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f) });
    vertices.push_back({ vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f) });

    unsigned int ind[] =
    {
        0, 1, 2,  2, 3, 0,
        5, 4, 7,  7, 6, 5,
        8, 9, 10, 10, 11, 8,
        13, 12, 15, 15, 14, 13,
        19, 18, 17, 17, 16, 19,
        20, 21, 22, 22, 23, 20
    };

    indices.assign(ind, ind + 36);
    Mesh cubeMesh(vertices, indices);

    BoundingBox box;
    box.min = glm::vec3(-0.5f, -0.5f, -0.5f);
    box.max = glm::vec3( 0.5f,  0.5f,  0.5f);
    cubeMesh.SetBoundingBox(box);

    std::vector<Mesh> meshes;
    meshes.push_back(cubeMesh);

    ModelNode rootNode;
    rootNode.name = "PrimitiveAssembly";
    rootNode.localTransform = glm::mat4(1.0f);
    rootNode.meshIndices.push_back(0);
    return new Model("Procedural", "Cube", meshes, rootNode);
}