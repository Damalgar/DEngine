#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Components/Transform.h"
#include "Vendor/json.hpp"

using namespace glm;

namespace Utils
{
    /// @brief creates the model matrix (M.V.P. model)
    /// @param translation how much is the object transposed from the center of the world
    /// @param rotationAxis on which axis is it rotated
    /// @param rotationAngle how much is it rotated on each axis (IN DEGREES)
    /// @param scale object's scale
    /// @return mat4 model matrix
    static mat4 GetModelMatrix(vec3& translation, vec3& rotationAxis, float rotationAngle, vec3& scale)
    {
        //S.R.T
        mat4 modelMat = mat4(1.0);

        modelMat = translate(modelMat, translation);

        if (rotationAngle != 0)
            modelMat = rotate(modelMat, rotationAngle, rotationAxis);
            
        modelMat = glm::scale(modelMat, scale);

        return modelMat;
    }

    /// @brief creates the model matrix using Euler Angles (Pitch, Yaw, Roll)
    /// @param translation object position
    /// @param rotationEuler rotation on X, Y, and Z axes (IN DEGREES)
    /// @param scale object scale
    /// @return mat4 model matrix
    static mat4 GetModelMatrix(vec3& translation, vec3& rotationEuler, vec3& scale)
    {
        mat4 modelMat = mat4(1.0f);

        modelMat = glm::translate(modelMat, translation);

        //Y -> X -> Z to avoid gimbal lock as much as we can
        modelMat = glm::rotate(modelMat, glm::radians(rotationEuler.y), vec3(0.0f, 1.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(rotationEuler.x), vec3(1.0f, 0.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(rotationEuler.z), vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::scale(modelMat, scale);

        return modelMat;
    }

    /// @brief creates the view matrix (M.V.P. model)
    /// @param eye the position of the camera
    /// @param center the target we are looking at
    /// @param up a vector representing the up of the world
    /// @return mat4 view matrix
    static mat4 GetViewMatrix(const vec3& eye, const vec3& center, const vec3& up)
    {
        return lookAt(eye, center, up);
    }

    /// @brief creates the projection matrix (M.V.P. model)
    /// @param FOV Field Of View expressed in degress
    /// @param aspectRatio the window's aspect ratio
    /// @param nearPlane the near plane distance
    /// @param farPlane the far plane distance
    /// @return mat4 projection matrix
    static mat4 GetProjectionMatrix(float FOV, float aspectRatio, float nearPlane, float farPlane)
    {
        FOV = radians(FOV);
        return perspective(FOV, aspectRatio, nearPlane, farPlane);
    }

    /// @brief creates the projection matrix (M.V.P. model)
    /// @param FOV Field Of View expressed in degress
    /// @param screenWidth the window's width
    /// @param screenHeight the window's height
    /// @param nearPlane the near plane distance
    /// @param farPlane the far plane distance
    /// @return mat4 projection matrix
    static mat4 GetProjectionMatrix(float FOV, int screenWidth, int screenHeight, float nearPlane, float farPlane)
    {
        return GetProjectionMatrix(FOV, (float)screenWidth/(float)screenHeight, nearPlane, farPlane);
    }

    /// @brief creates the normal matrix for shaders (model^-1)^t
    /// @param modelMatrix the model matrix
    /// @return mat3 normalMatrix
    static mat3 GetNormalMatrix(mat4& modelMatrix)
    {
        return transpose(inverse(mat3(modelMatrix)));
    }
    
    /// @brief Computes the new local Transform (Position, Rotation, Scale) to keep the object fixed in world space when changing parent
    /// @param oldGlobalMatrix The world matrix of the object before changing parent
    /// @param newParentGlobalMatrix The world matrix of the new parent (use Identity matrix if unparenting/reparenting to root)
    /// @param outPosition Output parameter for the new local position
    /// @param outRotation Output parameter for the new local rotation (IN DEGREES)
    /// @param outScale Output parameter for the new local scale
    static void LocalFromGlobal(const mat4& oldGlobalMatrix, const mat4& newParentGlobalMatrix, vec3& outPosition, vec3& outRotation, vec3& outScale)
    {
        mat4 newLocalMatrix = glm::inverse(newParentGlobalMatrix) * oldGlobalMatrix;
        quat rotationQuat;
        vec3 skew;
        vec4 perspective;

        glm::decompose(newLocalMatrix, outScale, rotationQuat, outPosition, skew, perspective);
        
        mat4 rotationMatrix = glm::mat4_cast(rotationQuat);
        float radY, radX, radZ;
        glm::extractEulerAngleYXZ(rotationMatrix, radY, radX, radZ);

        outRotation = vec3(glm::degrees(radX), glm::degrees(radY), glm::degrees(radZ));
    }
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
}