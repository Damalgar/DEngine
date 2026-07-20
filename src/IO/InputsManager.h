#pragma once
#include "Vendor/imgui/imgui.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class InputsManager {
    public:
    static void Init(GLFWwindow* window);
    static bool IsKeyPressedThisFrame(ImGuiKey key);
    static bool IsKeyDown(ImGuiKey key);

    static bool IsMouseButtonDown(ImGuiMouseButton button);
    static bool IsMouseButtonPressed(ImGuiMouseButton button);
    static bool IsMouseButtonReleased(ImGuiMouseButton button);

    static glm::vec2 GetMouseDelta();
    static void SetCursorLocked(bool locked);

    private:
    inline static GLFWwindow* s_window = nullptr;
};