#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Vendor/json.hpp"

class BoundingBox;
class SceneObject;

using namespace glm;

namespace Utils
{
    /// @brief creates the model matrix (M.V.P. model)
    /// @param translation how much is the object transposed from the center of the world
    /// @param rotationAxis on which axis is it rotated
    /// @param rotationAngle how much is it rotated on each axis (IN DEGREES)
    /// @param scale object's scale
    /// @return mat4 model matrix
    mat4 GetModelMatrix(vec3& translation, vec3& rotationAxis, float rotationAngle, vec3& scale);

    /// @brief creates the model matrix using Euler Angles (Pitch, Yaw, Roll)
    /// @param translation object position
    /// @param rotationEuler rotation on X, Y, and Z axes (IN DEGREES)
    /// @param scale object scale
    /// @return mat4 model matrix
    mat4 GetModelMatrix(vec3& translation, vec3& rotationEuler, vec3& scale);

    /// @brief creates the view matrix (M.V.P. model)
    /// @param eye the position of the camera
    /// @param center the target we are looking at
    /// @param up a vector representing the up of the world
    /// @return mat4 view matrix
    mat4 GetViewMatrix(const vec3& eye, const vec3& center, const vec3& up);
    
    /// @brief creates the projection matrix (M.V.P. model)
    /// @param FOV Field Of View expressed in degress
    /// @param aspectRatio the window's aspect ratio
    /// @param nearPlane the near plane distance
    /// @param farPlane the far plane distance
    /// @return mat4 projection matrix
    mat4 GetProjectionMatrix(float FOV, float aspectRatio, float nearPlane, float farPlane);

    /// @brief creates the projection matrix (M.V.P. model)
    /// @param FOV Field Of View expressed in degress
    /// @param screenWidth the window's width
    /// @param screenHeight the window's height
    /// @param nearPlane the near plane distance
    /// @param farPlane the far plane distance
    /// @return mat4 projection matrix
    mat4 GetProjectionMatrix(float FOV, int screenWidth, int screenHeight, float nearPlane, float farPlane);

    /// @brief creates the normal matrix for shaders (model^-1)^t
    /// @param modelMatrix the model matrix
    /// @return mat3 normalMatrix
    mat3 GetNormalMatrix(mat4& modelMatrix);
    
    /// @brief Computes the new local Transform (Position, Rotation, Scale) to keep the object fixed in world space when changing parent
    /// @param oldGlobalMatrix The world matrix of the object before changing parent
    /// @param newParentGlobalMatrix The world matrix of the new parent (use Identity matrix if unparenting/reparenting to root)
    /// @param outPosition Output parameter for the new local position
    /// @param outRotation Output parameter for the new local rotation (IN DEGREES)
    /// @param outScale Output parameter for the new local scale
    void LocalFromGlobal(const mat4& oldGlobalMatrix, const mat4& newParentGlobalMatrix, vec3& outPosition, quat& outRotation, vec3& outScale);

    /// @brief Draws a Bounding Box for debug
    /// @param viewMatrix view matrix (M.V.P. model) 
    /// @param projectionMatrix projection matrix (M.V.P. model)
    void DrawDebugBox(const BoundingBox& box, const mat4& viewMatrix, const mat4& projectionMatrix);

    /// @brief checks if a ray intersects a specific boundingBox
    /// @param origin the origin from where the ray will begin
    /// @param direction the direction that the ray will follow
    /// @param box the box to check
    /// @param outDistance how far is the intersected box from the origin
    bool RayIntersectsBoundingBox(const vec3& origin, const vec3& direction, const BoundingBox& box, float& outDistance);

    /// @brief checks if a ray intersects a SceneObject, with exclusion of specific tags
    /// @param origin the origin from where the ray will begin
    /// @param direction the direction that the ray will follow
    /// @param ignoreTags tags to exclude from the raycast
    SceneObject* Raycast(vec3 origin, vec3 direction, const std::vector<std::string>& ignoreTags = {});

    /// @brief return a ray from the mouse position
    /// @param mouseX mouse's positon on the x axis
    /// @param mouseY mouse's positon on the y axis
    /// @param screenWidth screen or panel width
    /// @param viewMatrix view matrix (M.V.P. model) 
    /// @param projectionMatrix projection matrix (M.V.P. model)
    vec3 GetMouseRayDirection(float mouseX, float mouseY, float screenWidth, float screenHeight, const mat4& viewMatrix, const mat4& projectionMatrix);
}

namespace glm 
{
    //vec3 to json
    inline void to_json(nlohmann::json& j, const vec3& v) 
    {
        j = nlohmann::json{v.x, v.y, v.z};
    }

    //json to vec3
    inline void from_json(const nlohmann::json& j, vec3& v) 
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    //vec4 to json
    inline void to_json(nlohmann::json& j, const vec4& v) 
    {
        j = nlohmann::json{v.x, v.y, v.z, v.w};
    }

    //json to vec4
    inline void from_json(const nlohmann::json& j, vec4& v) 
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
        v.w = j.at(3).get<float>();
    }

    //quaternion to json
    inline void to_json(nlohmann::json& j, const quat& quaternion)
    {
        j = nlohmann::json{quaternion.w, quaternion.x, quaternion.y, quaternion.z};
    }

    //json to quaternion
    inline void from_json(const nlohmann::json& j, quat& quaternion)
    {
        quaternion.w = j.at(0).get<float>();
        quaternion.x = j.at(1).get<float>();
        quaternion.y = j.at(2).get<float>();
        quaternion.z = j.at(3).get<float>();
    }
}