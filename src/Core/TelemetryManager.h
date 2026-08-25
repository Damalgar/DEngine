#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>

class TelemetryManager {
    public:
    static bool LoadFromCSV(const std::string& path);
    
    static int GetMaxFrames() { return m_maxFrames; }
    static int GetCurrentFrame() { return m_currentFrame; }
    static float GetCurrentTime() { return m_currentTime; }

    static void NextFrame();
    static void PreviousFrame(); 
    static void SetFrame(int frame);
    static void Update();

    static float GetValue(const std::string& parameter);
    static const std::vector<float>& GetValues(const std::string& parameter);
    static const std::vector<std::string>& GetParameters() { return m_parametersNames; }
    static const std::vector<std::string>& GetValidParameters() { return m_validParameters; }
    static const std::vector<std::string>& GetInvalidParameters() { return m_invalidParameters; }

    //rangeMin, rangeMax, threshold
    static std::vector<float> GetSuggestedRange(const std::string& parameter);

    static bool IsCsvLoaded() { return m_isCsvLoaded; }

    private:
    inline static int m_currentFrame = 1;
    inline static int m_maxFrames = 1;
    inline static float m_currentTime = 0;

    inline static bool m_isCsvLoaded = false;

    //[column, values]
    inline static std::unordered_map<std::string, std::vector<float>> m_telemetryMap;

    inline static std::vector<std::string> m_parametersNames{};
    inline static std::vector<std::string> m_invalidParameters{};
    inline static std::vector<std::string> m_validParameters{};
};