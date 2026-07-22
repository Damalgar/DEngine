#include "Camera.h"

#include <iostream>
void Camera::Update()
{
    if (InputsManager::IsMouseButtonPressed(ImGuiMouseButton_Right) && m_canInteract)
    {
        m_isDragging = true;
        InputsManager::SetCursorLocked(true);
        m_lockedMousePos = ImGui::GetMousePos();
    }

    if (InputsManager::IsMouseButtonReleased(ImGuiMouseButton_Right) && m_isDragging)
    {
        InputsManager::SetCursorLocked(false);
        m_isDragging = false;

        ImGui::GetIO().WantSetMousePos = true;
        ImGui::GetIO().MousePos = m_lockedMousePos;
    }

    if (!m_isDragging)
        return;

    vec2 mouseDelta = InputsManager::GetMouseDelta();
    
    m_yaw   += mouseDelta.x * m_sensitivity;
    m_pitch -= mouseDelta.y * m_sensitivity;

    if (m_pitch > 89.0f)  m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    UpdateCameraVectors();

    vec3 inputDir = glm::vec3(0.0f);
    if (InputsManager::IsKeyDown(ImGuiKey_W)) inputDir += m_front;
    if (InputsManager::IsKeyDown(ImGuiKey_S)) inputDir -= m_front;
    if (InputsManager::IsKeyDown(ImGuiKey_D)) inputDir += m_right;
    if (InputsManager::IsKeyDown(ImGuiKey_A)) inputDir -= m_right;

    if (glm::length(inputDir) != 0.0f)
    {
        inputDir = glm::normalize(inputDir);
        float speed = m_movementSpeed * AppTime::DeltaTime();

        if (InputsManager::IsKeyDown(ImGuiKey_LeftShift))
            speed *= 5;
        m_pos += inputDir * speed;
    }
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}