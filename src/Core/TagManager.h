#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

class TagManager {
    public:
    static void Init();

    static bool AddTag(const std::string& tag);
    static bool RemoveTag(const std::string& tag);
    static bool ToggleTagVisibility(const std::string& tag);
    static bool SetTagVisibility(const std::string& tag, bool visible);
    static bool IsTagVisible(const std::string& tag);
    static const std::vector<std::string>& GetTags();

    private:
    static inline std::vector<std::string> m_tags;
    static inline std::unordered_map<std::string, bool> m_tagVisibility;

    static inline auto m_sortFunc = [](const std::string& a, const std::string& b) {
        if (a == "Default") return true;
        if (b == "Default") return false;

        return a < b;
    };
};