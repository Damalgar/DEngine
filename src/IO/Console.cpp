#include "Console.h"
#include <iostream>

void Console::Log(const std::string& message, LOG_LEVEL level, LOG_CATEGORY category)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string finalMessage = GetCategoryString(category) + " " + message;
    m_logs.push_back({ finalMessage, level, category });

    if (m_logs.size() > MAX_LOGS)
        m_logs.erase(m_logs.begin());

    std::cout << finalMessage << std::endl;
}

void Console::Clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logs.clear();
}

std::string Console::GetCategoryString(LOG_CATEGORY category)
{
    using cat = LOG_CATEGORY;
    switch (category)
    {
        case cat::ASSETMANAGER: return "[ASSET MANAGER]";
        case cat::SCENE: return "[SCENE]";
        case cat::SYSTEM: return "[SYSTEM]";
        case cat::TELEMETRY: return "[TELEMETRY]";
        default: return "[INFO]";
    }
}