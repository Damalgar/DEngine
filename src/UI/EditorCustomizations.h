#pragma once
#include "Vendor/imgui/imgui.h"

struct FileSystemPref
{
    float borderMargins = 30;
    float cellSize = 100;
    float cellSpacing = 30;
};
inline FileSystemPref fileSystemPref;

struct ScenePref
{
    ImVec2 buttonSize = ImVec2(75, 75);
};
inline ScenePref scenePref;