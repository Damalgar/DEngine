#pragma once
#include <string>
#include <vector>
#include <mutex>

enum class LOG_LEVEL {
    INFO = 0,
    WARNING,
    ERROR
};

enum class LOG_CATEGORY {
    DEFAULT = 0,
    ASSETMANAGER,
    SCENE,
    SYSTEM,
    TELEMETRY
};

struct LogMessage {
    std::string text;
    LOG_LEVEL level;
    LOG_CATEGORY category;
};

class Console {
    public:
    static void Log(const std::string& message, LOG_LEVEL level = LOG_LEVEL::INFO, LOG_CATEGORY category = LOG_CATEGORY::DEFAULT);
    static void LogInfo(const std::string& message, LOG_CATEGORY category = LOG_CATEGORY::DEFAULT) { Log(message, LOG_LEVEL::INFO, category); }
    static void LogWarn(const std::string& message, LOG_CATEGORY category = LOG_CATEGORY::DEFAULT) { Log(message, LOG_LEVEL::WARNING, category); }
    static void LogError(const std::string& message, LOG_CATEGORY category = LOG_CATEGORY::DEFAULT) { Log(message, LOG_LEVEL::ERROR, category); }
    static void Clear();
    static const std::vector<LogMessage>& GetLogs() { return m_logs; }
    
    private:
    inline static std::vector<LogMessage> m_logs;
    inline static std::mutex m_mutex;
    inline static const int MAX_LOGS = 1000;

    static std::string GetCategoryString(LOG_CATEGORY category);
};