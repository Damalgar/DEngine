#pragma once
#include "Vendor/imgui/imgui.h"

class AppTime {
    public:

    static void Update()
    {
        m_deltaTime += ImGui::GetIO().DeltaTime;
        m_time += m_deltaTime;
    }

    static float Time() { return m_time / 1000; }
    static float DeltaTime() { return m_deltaTime / 1000; };

    static float TimeMs() { return m_time; }
    static float DeltaTimeMs() { return m_deltaTime; };

    private:
    inline static float m_time = 0;
    inline static float m_deltaTime = 0;
};