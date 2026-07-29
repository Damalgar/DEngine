#pragma once
#include "glm/glm.hpp"
#include <limits>
using namespace glm;

class BoundingBox {
    public:
    vec3 min;
    vec3 max;

    BoundingBox()
    {
        min = vec3(std::numeric_limits<float>::max());
        max = vec3(std::numeric_limits<float>::lowest());
    }

    BoundingBox(const vec3& min, const vec3& max) : min(min), max(max) {}

    void Expand(const vec3 point)
    {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);

        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }

    void Combine(const BoundingBox& other)
    {
        Expand(other.min);
        Expand(other.max);
    }

    BoundingBox GetTransformed(const mat4& transformMatrix) const
    {
        vec3 corners[8] = {
            glm::vec3(min.x, min.y, min.z),
            glm::vec3(max.x, min.y, min.z),
            glm::vec3(min.x, max.y, min.z),
            glm::vec3(max.x, max.y, min.z),
            glm::vec3(min.x, min.y, max.z),
            glm::vec3(max.x, min.y, max.z),
            glm::vec3(min.x, max.y, max.z),
            glm::vec3(max.x, max.y, max.z)
        };

        BoundingBox globalBox;

        for (int i = 0; i<8; i++)
        {
            vec4 newCorner = transformMatrix * vec4(corners[i], 1.0f);
            globalBox.Expand(newCorner);
        }

        return globalBox;
    } 
};