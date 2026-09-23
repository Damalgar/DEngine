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
    ImVec2 squareButtonSize = ImVec2(75, 75);
    ImVec2 switchButtonSize = ImVec2(40, 40);
};
inline ScenePref scenePref;

struct TimelinePref
{
    float timelineSliderHeigth = 50;
    float timelineMargin = 20;
    ImU32 timelineSliderColor = IM_COL32(30, 30, 30, 255);
    
    float playheadThickness = 2;
    ImU32 playheadColor = IM_COL32(255, 255, 255, 255);

    float zoomSpeed = 0.15f;
    int rulerTicks = 10;
};
inline TimelinePref timelinePref;