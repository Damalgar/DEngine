#include "IO/InputsManager.h"

void InputsManager::Init(GLFWwindow* window)
{
    s_window = window;
}

bool InputsManager::IsKeyPressedThisFrame(ImGuiKey key)
{
    return ImGui::IsKeyPressed(key, false);
}

bool InputsManager::IsKeyDown(ImGuiKey key)
{
    return ImGui::IsKeyDown(key);
}

glm::vec2 InputsManager::GetMouseDelta()
{
    ImVec2 delta = ImGui::GetIO().MouseDelta;
    return glm::vec2(delta.x, delta.y);
}

void InputsManager::SetCursorLocked(bool locked)
{
    if (!s_window) return;

    if (locked)
    {
        glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    } else {
        glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }
}

bool InputsManager::IsMouseButtonDown(ImGuiMouseButton button)
{
    return ImGui::IsMouseDown(button);
}

bool InputsManager::IsMouseButtonPressed(ImGuiMouseButton button)
{
    return ImGui::IsMouseClicked(button);
}

bool InputsManager::IsMouseButtonReleased(ImGuiMouseButton button)
{
    return ImGui::IsMouseReleased(button);
}

float InputsManager::MouseWheelScroll()
{
    return ImGui::GetIO().MouseWheel;
}