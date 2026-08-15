#pragma once

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
};