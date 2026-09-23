#include "Utils/Utils.h"
#include "Core/Transform.h"
#include "Core/AssetManager.h"
#include "Render/Shader.h"
#include "Physics/BoundingBox.h"
#include "Core/SceneManager.h"
#include "Core/SceneObject.h"
#include <random>
#include <filesystem>
namespace fs = std::filesystem;

namespace Utils
{
    mat4 GetModelMatrix(vec3& translation, vec3& rotationAxis, float rotationAngle, vec3& scale)
    {
        //S.R.T
        mat4 modelMat = mat4(1.0);

        modelMat = translate(modelMat, translation);

        if (rotationAngle != 0)
            modelMat = rotate(modelMat, rotationAngle, rotationAxis);
            
        modelMat = glm::scale(modelMat, scale);

        return modelMat;
    }

    mat4 GetModelMatrix(vec3& translation, vec3& rotationEuler, vec3& scale)
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

    mat4 GetViewMatrix(const vec3& eye, const vec3& center, const vec3& up)
    {
        return lookAt(eye, center, up);
    }

    mat4 GetProjectionMatrix(float FOV, float aspectRatio, float nearPlane, float farPlane)
    {
        FOV = radians(FOV);
        return perspective(FOV, aspectRatio, nearPlane, farPlane);
    }

    mat4 GetProjectionMatrix(float FOV, int screenWidth, int screenHeight, float nearPlane, float farPlane)
    {
        return GetProjectionMatrix(FOV, (float)screenWidth/(float)screenHeight, nearPlane, farPlane);
    }

    mat3 GetNormalMatrix(mat4& modelMatrix)
    {
        return transpose(inverse(mat3(modelMatrix)));
    }

    void LocalFromGlobal(const mat4& oldGlobalMatrix, const mat4& newParentGlobalMatrix, vec3& outPosition, quat& outRotation, vec3& outScale)
    {
        mat4 newLocalMatrix = glm::inverse(newParentGlobalMatrix) * oldGlobalMatrix;
        
        vec3 skew;
        vec4 perspective;
        
        glm::decompose(newLocalMatrix, outScale, outRotation, outPosition, skew, perspective);
    }

    void DrawDebugBox(const BoundingBox& box, const mat4& viewMatrix, const mat4& projectionMatrix)
    {
        static unsigned int vao = 0;
        static unsigned int vbo = 0;
        
        if (vao == 0)
        {
            float vertices[] = {
                //Front
                -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,
                0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,
                //Back
                -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,
                //Mid
                -0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,   0.5f, -0.5f, -0.5f,
                0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,
                -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
        }

        glm::vec3 center = (box.max + box.min) * 0.5f;
        glm::vec3 size = box.max - box.min;            

        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), center);
        modelMatrix = glm::scale(modelMatrix, size);

        Shader* debugShader = AssetManager::GetShader("DefaultShader");
        debugShader->Use();
        debugShader->SetMat4("modelMatrix", modelMatrix);
        debugShader->SetMat4("viewMatrix", viewMatrix);
        debugShader->SetMat4("projectionMatrix", projectionMatrix);

        debugShader->SetInt("usePlainColor", 1);
        debugShader->SetVec4("material.tintColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        glLineWidth(7.0f);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 24); 
        glBindVertexArray(0);

        glLineWidth(1.0f);
    }

    void DrawWireSphere(const vec3& position, float radius, const vec4& color, const mat4& viewMatrix, const mat4& projectionMatrix)
    {
        static unsigned int vao = 0;
        static unsigned int vbo = 0;
        static int numVertices = 0;
        
        if (vao == 0)
        {
            std::vector<float> vertices;
            const int segments = 32;
            const float angleStep = (2.0f * 3.14159265359f) / segments;

            for (int i = 0; i < segments; i++)
            {
                float angle1 = i * angleStep;
                float angle2 = (i + 1) * angleStep;
                
                vertices.push_back(std::cos(angle1));
                vertices.push_back(std::sin(angle1));
                vertices.push_back(0.0f);
                
                vertices.push_back(std::cos(angle2));
                vertices.push_back(std::sin(angle2));
                vertices.push_back(0.0f);
            }

            for (int i = 0; i < segments; i++)
            {
                float angle1 = i * angleStep;
                float angle2 = (i + 1) * angleStep;
                
                vertices.push_back(std::cos(angle1));
                vertices.push_back(0.0f);
                vertices.push_back(std::sin(angle1));
                
                vertices.push_back(std::cos(angle2));
                vertices.push_back(0.0f);
                vertices.push_back(std::sin(angle2));
            }

            for (int i = 0; i < segments; i++)
            {
                float angle1 = i * angleStep;
                float angle2 = (i + 1) * angleStep;
                
                vertices.push_back(0.0f);
                vertices.push_back(std::cos(angle1));
                vertices.push_back(std::sin(angle1));
                
                vertices.push_back(0.0f);
                vertices.push_back(std::cos(angle2));
                vertices.push_back(std::sin(angle2));
            }

            numVertices = vertices.size() / 3;

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
        }

        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(radius));

        Shader* debugShader = AssetManager::GetShader("DefaultShader"); 
        debugShader->Use();
        debugShader->SetMat4("modelMatrix", modelMatrix);
        debugShader->SetMat4("viewMatrix", viewMatrix);
        debugShader->SetMat4("projectionMatrix", projectionMatrix);
        
        debugShader->SetInt("usePlainColor", 1);
        debugShader->SetVec4("material.tintColor", color);
        
        glLineWidth(7.0f);
        
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, numVertices); 
        glBindVertexArray(0);

        glLineWidth(1.0f);
    }

    bool RayIntersectsBoundingBox(const vec3& origin, const vec3& direction, const BoundingBox& box, float& outDistance)
    {
        //Slab method
        vec3 invDir = 1.0f / direction;
        
        vec3 t0 = (box.min - origin) * invDir;
        vec3 t1 = (box.max - origin) * invDir;

        vec3 tmin = glm::min(t0, t1);
        vec3 tmax = glm::max(t0, t1);

        float enterDist = std::max(std::max(tmin.x, tmin.y), tmin.z);
        float exitDist = std::min(std::min(tmax.x, tmax.y), tmax.z);

        if (exitDist < enterDist)
            return false;

        if (exitDist < 0.0f)
            return false;

        outDistance = enterDist;
        return true;
    }

    SceneObject* Raycast(vec3 origin, vec3 direction, const std::vector<std::string>& ignoreTags)
    {
        if (!SceneManager::GetActiveScene())
            return nullptr;

        SceneObject* closestObject = nullptr;
        float minHitDistance = std::numeric_limits<float>::max();

        for (SceneObject* obj : SceneManager::GetActiveScene()->GetSceneObjects())
        {
            if (!obj->IsActiveInHierarchy())
                continue;

            if (!ignoreTags.empty())
            {
                if (std::find(ignoreTags.begin(), ignoreTags.end(), obj->GetTag()) != ignoreTags.end())
                    continue;
            }

            MeshRenderer* renderer = obj->GetComponent<MeshRenderer>();
            if (!renderer)
                continue;

            BoundingBox box = renderer->GetGlobalBoundingBox();

            float hitDistance;
            vec3 normDir = glm::normalize(direction);

            if (RayIntersectsBoundingBox(origin, normDir, box, hitDistance) && hitDistance < minHitDistance)
            {
                closestObject = obj;
                minHitDistance = hitDistance;
            }
        }

        return closestObject;
    }

    vec3 GetMouseRayDirection(float mouseX, float mouseY, float screenWidth, float screenHeight, const mat4& viewMatrix, const mat4& projectionMatrix)
    {
        //Normalized device coordinate
        float x = (2.0f * mouseX) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / screenHeight; //OpenGL grows Y towards down

        vec4 rayClip = vec4(x, y, -1.0f, 1.0f);
        vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;

        rayEye = vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        vec4 rayWorld = glm::inverse(viewMatrix) * rayEye;

        return glm::normalize(glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z));
    }

    uint64_t GetRandomID()
    {
        std::random_device rd;
        std::mt19937_64 eng(rd());
        std::uniform_int_distribution<uint64_t> distr;

        return distr(eng);
    }

    std::string GetFileIncrementalName(const std::string& originalName, const std::string& extension, const std::string& folder)
    {
        fs::path userFolder = fs::path(FileSystem::GetAssetPath("User")) / folder;
        fs::path finalPath = userFolder / (originalName + extension);
        int counter = 0;
        while (fs::exists(finalPath))
        {
            counter++;
            std::ostringstream oss;
            oss << originalName << "_" << std::setw(3) << std::setfill('0') << counter;
            
            finalPath = userFolder / (oss.str() + extension);
        }
        return finalPath.filename().stem().string();
    }
} //Namespace Utils