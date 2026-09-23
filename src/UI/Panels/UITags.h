#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

class UITags {
    public:
    static void Draw();
    static void OnCsvLoaded();
    static std::unordered_set<std::string> GetTrackedParameters();

    private:
    inline static bool m_focusOnSelectedObj = true;
    inline static bool m_focusOnTrackedParameters = false;
    inline static std::unordered_map<std::string, bool> m_trackedParametersMap;
    inline static std::unordered_set<std::string> m_trackedParameters;

    static void DrawTags();
    static void DrawParameters();
};