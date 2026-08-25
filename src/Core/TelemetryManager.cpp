#include "Core/TelemetryManager.h"
#include "Core/AppTime.h"
#include "IO/Console.h"
#include "IO/TelemetryParser.h"
#include <algorithm>
#include <limits>

bool TelemetryManager::LoadFromCSV(const std::string& path)
{
    TelemetryData data = TelemetryParser::LoadCSV(path);

    if (!data.success)
        return false;

    m_telemetryMap = std::move(data.map);
    m_parametersNames = std::move(data.orderedNames);
    m_maxFrames = data.maxFrames;

    std::sort(m_parametersNames.begin(), m_parametersNames.end());

    for (const std::string& parameter : m_parametersNames)
    {
        if (data.emptyParameters.find(parameter) == data.emptyParameters.end())
            m_validParameters.push_back(parameter);
        else
            m_invalidParameters.push_back(parameter);
    }

    m_currentFrame = 1;
    m_currentTime = 0;

    m_isCsvLoaded = true;

    Console::LogInfo("Loaded CSV " + path + " (" + std::to_string(m_maxFrames) + " frames)", LOG_CATEGORY::TELEMETRY);
    if (!m_invalidParameters.empty())
        Console::LogWarn("Found " + std::to_string(m_invalidParameters.size()) + " empty parameters in the CSV.", LOG_CATEGORY::TELEMETRY);

    return true;
}

void TelemetryManager::NextFrame()
{
    m_currentFrame++;
    if (m_currentFrame > m_maxFrames)
        m_currentFrame = m_maxFrames;
}

void TelemetryManager::PreviousFrame()
{
    m_currentFrame--;
    if (m_currentFrame < 1)
        m_currentFrame = 1;
}

void TelemetryManager::SetFrame(int frame)
{
    m_currentFrame = frame;
    
    if (m_currentFrame < 1)
        m_currentFrame = 1;
    else if (m_currentFrame > m_maxFrames)
        m_currentFrame = m_maxFrames;
}

void TelemetryManager::Update()
{
    float deltaTime = AppTime::DeltaTime();
    m_currentTime += deltaTime;
}

float TelemetryManager::GetValue(const std::string& parameter)
{
    if (parameter == "")
        return std::numeric_limits<float>::quiet_NaN();

    auto it = m_telemetryMap.find(parameter);
    if (it == m_telemetryMap.end())
    {
        Console::LogError("parameter " + parameter + " not valid", LOG_CATEGORY::TELEMETRY);
        return std::numeric_limits<float>::quiet_NaN();
    }

    const std::vector<float>& values = it->second;
    if (m_currentFrame - 1 >= values.size())
    {
        Console::LogError("parameter " + parameter + " not valid (less rows than frames)", LOG_CATEGORY::TELEMETRY);
        return std::numeric_limits<float>::quiet_NaN();
    }

    return values[m_currentFrame-1];
}

const std::vector<float>& TelemetryManager::GetValues(const std::string& parameter)
{
    auto it = m_telemetryMap.find(parameter);
    if (it == m_telemetryMap.end())
    {
        Console::LogError("parameter " + parameter + " not valid", LOG_CATEGORY::TELEMETRY);
        static const std::vector<float> emptyVec;
        return emptyVec;
    }

    const std::vector<float>& values = it->second;
    return values;
}

//rangeMin, rangeMax, threshold
std::vector<float> TelemetryManager::GetSuggestedRange(const std::string& parameter)
{
    if (m_telemetryMap.find(parameter) != m_telemetryMap.end())
    {
        const auto& values = m_telemetryMap[parameter];

        if (values.empty())
        {
            Console::LogWarn("Requested range for an empty parameter", LOG_CATEGORY::TELEMETRY);
            return {};
        }

        float max = std::numeric_limits<float>::lowest();
        float min = std::numeric_limits<float>::max();
        double sum = 0; 
        int validCount = 0;

        for (float value : values)
        {
            if (std::isnan(value))
                continue;
                
            sum += value;
            validCount++;

            if (value > max)
                max = value;
            if (value < min)
                min = value;
        }

        if (validCount == 0)
        {
            Console::LogWarn("Requested range for a parameter with invalid values", LOG_CATEGORY::TELEMETRY);
            return {};
        } 

        float average = static_cast<float>(sum / validCount);

        return {min, max, average};
    }

    Console::LogWarn("Requested range for an invalid parameter", LOG_CATEGORY::TELEMETRY);
    return {};
}