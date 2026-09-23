#pragma once
#include "Vendor/imgui/imgui.h"
#include <string>

class UITimeline {
    public:
    static void Draw();

    static void SetStartFrame(int startFrame) { m_viewStartFrame = startFrame; }
    static void SetEndFrame(int endFrame) { m_viewEndFrame = endFrame; }

    private:
    inline static int m_viewStartFrame = 1;
    inline static int m_viewEndFrame = 1;
    inline static bool m_isPanning = false;
    inline static float m_panAccumulator = 0;

    inline static const ImVec4 parameterPalette[] = {
        ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
        ImVec4(0.30f, 0.70f, 0.90f, 1.0f),
        ImVec4(0.40f, 0.85f, 0.40f, 1.0f),
        ImVec4(0.95f, 0.75f, 0.20f, 1.0f),
        ImVec4(0.70f, 0.40f, 0.90f, 1.0f),
        ImVec4(0.95f, 0.55f, 0.20f, 1.0f),
        ImVec4(0.30f, 0.90f, 0.80f, 1.0f),
        ImVec4(0.90f, 0.40f, 0.70f, 1.0f),
        ImVec4(0.60f, 0.80f, 0.30f, 1.0f),
        ImVec4(0.50f, 0.60f, 0.95f, 1.0f),
    };
    static inline constexpr int parameterPaletteSize = sizeof(parameterPalette) / sizeof(parameterPalette[0]);

    static ImVec4 GetParameterColor(const std::string& name);
    static void DrawGraph(ImVec2 size);
};