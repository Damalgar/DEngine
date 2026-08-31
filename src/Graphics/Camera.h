#pragma once
#include "IO/InputsManager.h"
#include "Core/AppTime.h"
#include <glm/glm.hpp>
using namespace glm;

class Camera {
    public:
    Camera() = default;
    void Update();

    vec3 GetPos() { return m_pos; }
    vec3 GetFront() { return m_front; }

    void SetCanInteract(const bool value) { m_canInteract = value; }
    bool GetCanInteract() const { return m_canInteract; }

    private:
    vec3 m_pos = vec3(0, 0, 0);
    vec3 m_front = vec3(0, 0, -1);
    vec3 m_right = vec3(1, 0, 0);
    vec3 m_up = vec3(0,1,0);

    float m_yaw = -90; // right/left
    float m_pitch = 0; // up/down
    float m_movementSpeed = 15.0f;
    float m_sensitivity = 0.1f;

    bool m_isDragging = false;
    bool m_canInteract = false;

    ImVec2 m_lockedMousePos;

    glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    void UpdateCameraVectors();
};