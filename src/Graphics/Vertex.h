#pragma once
#include "glm/glm.hpp"
#include <cstddef>
using namespace glm;

struct Vertex
{
    vec3 pos;
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
    vec3 bitangent;
    vec4 color;

    Vertex() : pos(0.0f), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f), color(1.0f) {}

    Vertex(vec3 pos, vec4 color = vec4(1.0f))
        : pos(pos), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f), color(color) {}

    Vertex(vec3 pos, vec3 normal, vec2 texCoords)
        : pos(pos), normal(normal), texCoords(texCoords), tangent(0.0f), bitangent(0.0f), color(1.0f) {}
};