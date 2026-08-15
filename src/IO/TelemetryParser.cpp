#include "IO/TelemetryParser.h"
#include "IO/Console.h"
#include <fstream>
#include <sstream>
#include <algorithm>

TelemetryData TelemetryParser::LoadCSV(const std::string& filepath)
{
    TelemetryData result;
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        Console::LogError("Failed to open CSV file: " + filepath);
        return result;
    }

    std::string line;
    std::getline(file, line);

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string columnName = "";
        while (std::getline(ss, columnName, ';'))
        { 
            result.orderedNames.push_back(columnName);
            result.map[columnName] = std::vector<float>(); 
        }
    }

    std::vector<float> lastValidValues(result.orderedNames.size(), -1);
    std::vector<bool> hasValidValue(result.orderedNames.size(), false);
    
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cellValue;
        int columnIndex = 0;

        while (std::getline(ss, cellValue, ';')) {
            if (columnIndex >= result.orderedNames.size()) break;

            float parsedValue = std::numeric_limits<float>::quiet_NaN();

            if (!cellValue.empty() && cellValue != "\r") {
                std::replace(cellValue.begin(), cellValue.end(), ',', '.');

                try {
                    parsedValue = std::stof(cellValue);
                    lastValidValues[columnIndex] = parsedValue;
                    hasValidValue[columnIndex] = true;
                } catch (...) {
                    //Ignore it, keep the value we found the previous row
                }
            }

            std::string& colName = result.orderedNames[columnIndex];
            result.map[colName].push_back(lastValidValues[columnIndex]);
            
            columnIndex++;
        }

        while (columnIndex < result.orderedNames.size()) {
            std::string& colName = result.orderedNames[columnIndex];
            result.map[colName].push_back(lastValidValues[columnIndex]);
            columnIndex++;
        }

        result.maxFrames++;
    }

    for (size_t i = 0; i < result.orderedNames.size(); i++)
    {
        if (!hasValidValue[i])
            result.emptyParameters.insert(result.orderedNames[i]);
    }

    file.close();
    result.success = true;
    return result;
}