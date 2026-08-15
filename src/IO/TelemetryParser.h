#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>


struct TelemetryData {
    std::unordered_map<std::string, std::vector<float>> map;
    std::vector<std::string> orderedNames;
    std::unordered_set<std::string> emptyParameters;
    int maxFrames = 0;
    bool success = false;
};

class TelemetryParser {
    public:
    static TelemetryData LoadCSV(const std::string& filepath);
};