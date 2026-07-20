#pragma once
#include "imgui.h"
#include "../Vendor/imgui/backends/imgui_impl_glfw.h"
#include "../Vendor/imgui/backends/imgui_impl_opengl3.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <imgui_internal.h>

#define TOAST_SUCCESS ImVec4(0.2f, 0.7f, 0.2f, 1.0f)
#define TOAST_WARNING ImVec4(1.0f, 0.7f, 0.0f, 1.0f)
#define TOAST_ERROR   ImVec4(0.8f, 0.2f, 0.2f, 1.0f)
#define TOAST_INFO    ImVec4(0.2f, 0.4f, 0.8f, 1.0f)

template <typename T>
struct ComboEntry {
    const char* name;
    T value;
};

enum class BUTTON_COLORS
{
    POSITIVE = 0,
    NEGATIVE,
    WARNING,
    UTILITY,
    GREY,
    INVISIBLE,
    NONE
};

inline void ElideString(std::string& stringToElide, int charactersInclusive = 24)
{
    if (stringToElide.size() > charactersInclusive+3)
        stringToElide = stringToElide.substr(0, charactersInclusive) + "...";
}

inline bool DrawHybridInt(const char* label, int* variable, int v_min, int v_max, bool returnEdit = false, bool spacing = true, float invSize = 1,
    bool sameLineText = true,
    int warning_treshold = 9999999, std::string warning_title = "", std::string warning_text = "") {
    bool edited = false;
    bool valueChanged = false;
    ImGui::PushID(label);
    if (!sameLineText)
        ImGui::Text("%s", label);
    float totalWidth = ImGui::GetContentRegionAvail().x / invSize;
    float inputWidth = 80.0f;
    float sliderWidth = totalWidth - inputWidth - 10.0f;

    ImGui::SetNextItemWidth(sliderWidth);
    if (ImGui::SliderInt("##slider", variable, v_min, v_max, ""))
        valueChanged = true; 
    
    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;
    
    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    if (ImGui::InputInt("##input", variable, 0)) 
    {
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        valueChanged = true;
    }

    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;

    if (sameLineText) {
        ImGui::SameLine();
        ImGui::Text("%s", label);
    }

    if (warning_treshold != 9999999 && *variable > warning_treshold)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "(!)");
        
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "%s", warning_title.c_str());
            ImGui::Separator();
            ImGui::Text("%s", warning_text.c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::PopID();

    if (spacing)
        ImGui::Spacing();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawHybridIntPerc(const char* label, int* variable, int v_min, int v_max, bool returnEdit = false, bool spacing = true, float invSize = 1,
    bool sameLineText = true,
    int warning_treshold = 9999999, std::string warning_title = "", std::string warning_text = "") {
    
    bool edited = false;
    bool valueChanged = false;
    
    ImGui::PushID(label);
    if (!sameLineText)
        ImGui::Text("%s", label);
        
    float totalWidth = ImGui::GetContentRegionAvail().x / invSize;
    float inputWidth = 80.0f;
    float sliderWidth = totalWidth - inputWidth - 10.0f;

    float range = (float)(v_max - v_min);
    if (range <= 0.0f) range = 1.0f;
    int perc = (int)std::round((*variable - v_min) / range * 100.0f);

    ImGui::SetNextItemWidth(sliderWidth);
    if (ImGui::SliderInt("##slider", &perc, 0, 100, ""))
    {
        if (perc < 0) perc = 0;
        if (perc > 100) perc = 100;
        
        *variable = v_min + (int)std::round((perc / 100.0f) * range);
        
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        
        valueChanged = true;
    }
    
    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;
    
    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    int step = 0;
    
    if (ImGui::InputScalar("##input", ImGuiDataType_S32, &perc, &step, NULL, "%d%%")) 
    {
        if (perc < 0) perc = 0;
        if (perc > 100) perc = 100;
        
        *variable = v_min + (int)std::round((perc / 100.0f) * range);
        
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        
        valueChanged = true;
    }

    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;

    if (sameLineText) {
        ImGui::SameLine();
        ImGui::Text("%s", label);
    }

    if (warning_treshold != 9999999 && *variable > warning_treshold)
    {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "(!)");
        
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "%s", warning_title.c_str());
            ImGui::Separator();
            ImGui::Text("%s", warning_text.c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::PopID();

    if (spacing)
        ImGui::Spacing();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawHybridFloatPerc(const char* label, float* variable, float v_min, float v_max, const char* format = "%.1f%%", bool returnEdit = false, bool spacing = true, float invSize = 1,
    bool sameLineText = true, bool* outActivated = nullptr, bool* outDeactivated = nullptr,
    float warning_treshold = 9999999.0f, std::string warning_title = "", std::string warning_text = "") {
    
    bool edited = false;
    bool valueChanged = false;

    if (outActivated) *outActivated = false;
    if (outDeactivated) *outDeactivated = false;
    
    ImGui::PushID(label);
    if (!sameLineText)
        ImGui::Text("%s", label);
        
    float totalWidth = ImGui::GetContentRegionAvail().x / invSize;
    float inputWidth = 80.0f;
    float sliderWidth = totalWidth - inputWidth - 10.0f;

    float range = v_max - v_min;
    if (range <= 0.0f) range = 1.0f;
    
    float perc = ((*variable - v_min) / range) * 100.0f;

    ImGui::SetNextItemWidth(sliderWidth);
    if (ImGui::SliderFloat("##slider", &perc, 0.0f, 100.0f, format))
    {
        if (perc < 0.0f) perc = 0.0f;
        if (perc > 100.0f) perc = 100.0f;
        
        *variable = v_min + (perc / 100.0f) * range;
        
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        
        valueChanged = true;
    }
    
    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;
        
    if (outActivated && ImGui::IsItemActivated()) *outActivated = true;
    if (outDeactivated && ImGui::IsItemDeactivatedAfterEdit()) *outDeactivated = true;
    
    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    
    if (ImGui::InputFloat("##input", &perc, 0.0f, 0.0f, format)) 
    {
        if (perc < 0.0f) perc = 0.0f;
        if (perc > 100.0f) perc = 100.0f;
        
        *variable = v_min + (perc / 100.0f) * range;
        
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        
        valueChanged = true;
    }

    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;

    if (outActivated && ImGui::IsItemActivated()) *outActivated = true;
    if (outDeactivated && ImGui::IsItemDeactivatedAfterEdit()) *outDeactivated = true;

    if (sameLineText) {
        ImGui::SameLine();
        ImGui::Text("%s", label);
    }
 
    if (warning_treshold != 9999999.0f && *variable > warning_treshold)
    {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "(!)");
        
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "%s", warning_title.c_str());
            ImGui::Separator();
            ImGui::Text("%s", warning_text.c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::PopID();

    if (spacing)
        ImGui::Spacing();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawFieldString(const char* label, std::string& str, int maxLength, bool returnEdit = false, bool sameLine = true, int invSize = 1, 
    bool multiline = false, float multilineHeight = 0.0f, bool* outActivated = nullptr , bool* outDeactivatedAfterEdit = nullptr)
{
    if (outActivated) *outActivated = false;
    if (outDeactivatedAfterEdit) *outDeactivatedAfterEdit = false;
    bool edited = false;
    bool valueChanged = false;
    
    ImGui::PushID(label);
    if (!sameLine) ImGui::Text("%s", label);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / invSize);
    
    char buffer[maxLength];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s", str.c_str());
    
    if (multiline)
    {
        ImVec2 size = ImVec2(0.0f, multilineHeight);
        if (ImGui::InputTextMultiline("##input", buffer, sizeof(buffer), size))
        {
            str = buffer;
            valueChanged = true;
        }

        if (ImGui::IsItemActivated() && outActivated)
            *outActivated = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) 
        {
            if (outDeactivatedAfterEdit)
                *outDeactivatedAfterEdit = true;
            edited = true;
        }
    }
    else if (ImGui::InputText("##input", buffer, sizeof(buffer))) 
    {
        str = buffer;
        valueChanged = true;
    }

    if (ImGui::IsItemActivated() && outActivated)
            *outActivated = true;
    if (ImGui::IsItemDeactivatedAfterEdit()) 
    {
        if (outDeactivatedAfterEdit)
            *outDeactivatedAfterEdit = true;
        edited = true;
    }

    if (sameLine)
    {
        ImGui::SameLine();
        ImGui::Text("%s", label);
    }

    ImGui::PopID();

    return returnEdit ? edited : valueChanged;
};

inline bool DrawFieldStringPlaceHolder(const char* label, std::string& str, int maxLength, bool returnEdit = false, int invSize = 1) {
    bool edited = false;
    bool valueChanged = false;
    
    ImGui::PushID(label);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / invSize);
    
    char buffer[maxLength];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s", str.c_str());
    
    if (ImGui::InputTextWithHint("##input", label, buffer, sizeof(buffer))) 
    {
        str = buffer;
        valueChanged = true;
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) 
    {
        edited = true;
    }

    ImGui::PopID();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawFieldInt(const char* label, int* variable, int v_min, int v_max, bool returnEdit = false,
    int warning_treshold = 9999999, std::string warning_title = "", std::string warning_text = "", bool spacing = true) {
    bool edited = false;
    bool valueChanged = false;
    ImGui::PushID(label);
    ImGui::Text("%s", label);
    float inputWidth = 60.0f;

    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    if (ImGui::InputInt("##input", variable, 0)) 
    {
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        valueChanged = true;
    }

    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;

    if (warning_treshold != 9999999 && *variable > warning_treshold)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "(!)");
        
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "%s", warning_title.c_str());
            ImGui::Separator();
            ImGui::Text("%s", warning_text.c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::PopID();

    if (spacing)
        ImGui::Spacing();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawFieldFloat(const char* label, float* variable, float v_min = std::numeric_limits<float>::lowest(), float v_max = std::numeric_limits<float>::max(), const char* format = "%.1f", bool returnEdit = false, 
    float invSize = 1, bool* outActivated = nullptr, bool* outDeactivated = nullptr) 
{
    bool edited = false;
    bool valueChanged = false;
    if (outActivated) *outActivated = false;
    if (outDeactivated) *outDeactivated = false;

    ImGui::PushID(label);
    
    if (label != nullptr && label[0] != '\0')
        ImGui::Text("%s", label);

    float inputWidth = ImGui::GetContentRegionAvail().x / invSize;
    ImGui::SetNextItemWidth(inputWidth);

    if (ImGui::InputFloat("##input", variable, 0.0f, 0.0f, format)) 
    {
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        valueChanged = true;
    }

    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;

    if (outActivated && ImGui::IsItemActivated()) *outActivated = true;
    if (outDeactivated && ImGui::IsItemDeactivatedAfterEdit()) *outDeactivated = true;

    ImGui::PopID();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawHybridFloat(const char* label, float* variable, float v_min, float v_max, const char* format = "%.1f", bool returnEdit = false, 
    float invSize = 1, ImGuiSliderFlags flags = 0, bool* outActivated = nullptr, bool* outDeactivated = nullptr) {
    bool edited = false;
    bool valueChanged = false;
    if (outActivated) *outActivated = false;
    if (outDeactivated) *outDeactivated = false;

    ImGui::PushID(label);
    if (label != "")
        ImGui::Text("%s", label);

    float totalWidth = ImGui::GetContentRegionAvail().x / invSize;
    float inputWidth = 100.0f;
    float sliderWidth = totalWidth - inputWidth - 10.0f;

    ImGui::SetNextItemWidth(sliderWidth);
    if (ImGui::SliderFloat("##slider", variable, v_min, v_max, "", flags))
        valueChanged = true; 

    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;

    if (outActivated && ImGui::IsItemActivated()) *outActivated = true;
    if (outDeactivated && ImGui::IsItemDeactivatedAfterEdit()) *outDeactivated = true;

    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    if (ImGui::InputFloat("##input", variable, 0.0f, 0.0f, format)) 
    {
        if (*variable < v_min) *variable = v_min;
        if (*variable > v_max) *variable = v_max;
        valueChanged = true;
    };

    if (ImGui::IsItemDeactivatedAfterEdit())
        edited = true;

    if (outActivated && ImGui::IsItemActivated()) *outActivated = true;
    if (outDeactivated && ImGui::IsItemDeactivatedAfterEdit()) *outDeactivated = true;

    ImGui::PopID();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawFieldFloat2(const char* label, float* v1, float* v2, int v_min, int v_max, bool returnEdit = false, const char* format = "%.1f",
    float warning_treshold = 9999999, std::string warning_title = "", std::string warning_text = "", bool spacing = true) {
    
    bool edited = false;
    bool valueChanged = false;
    
    ImGui::PushID(label);
    ImGui::Text("%s", label);
    float inputWidth = 60.0f;

    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    if (ImGui::InputFloat("##input1", v1, 0, 0, format)) 
    {
        if (*v1 < v_min) *v1 = v_min;
        if (*v1 > v_max) *v1 = v_max;
        valueChanged = true;
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

    ImGui::SameLine();

    ImGui::SetNextItemWidth(inputWidth);
    if (ImGui::InputFloat("##input2", v2, 0, 0, format)) 
    {
        if (*v2 < v_min) *v2 = v_min;
        if (*v2 > v_max) *v2 = v_max;
        valueChanged = true;
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

    if (warning_treshold != 9999999 && (*v1 > warning_treshold || *v2 > warning_treshold))
    {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "(!)");
        
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "%s", warning_title.c_str());
            ImGui::Separator();
            ImGui::Text("%s", warning_text.c_str());
            ImGui::EndTooltip();
        }
    }

    ImGui::PopID();

    if (spacing)
        ImGui::Spacing();

    return returnEdit ? edited : valueChanged;
}

inline bool DrawFloatCoords3(const char* mainLabel, float* v, float increment = 1.0f, float v_min = std::numeric_limits<float>::lowest(), float v_max = std::numeric_limits<float>::max(),
    const char* format = "%.2f", bool returnEdit = false, float margin = 20 ,const char* labX = "X", const char* labY = "Y", const char* labZ = "Z") 
{
    bool edited = false;
    bool valueChanged = false;
    
    ImGui::PushID(mainLabel);
    
    if (mainLabel && mainLabel[0] != '\0' && mainLabel[0] != '#') {
        ImGui::Text("%s", mainLabel);
    }
    
    float availWidth = ImGui::GetContentRegionAvail().x - margin;
    float spaceX = ImGui::GetStyle().ItemInnerSpacing.x;
    
    const char* labels[] = { labX, labY, labZ };
    float totalLabelsWidth = 0.0f;
    for (int i = 0; i < 3; ++i) {
        totalLabelsWidth += ImGui::CalcTextSize(labels[i]).x;
    }
    
    float inputWidth = (availWidth - totalLabelsWidth - (spaceX * 5.0f)) / 3.0f;
    
    for (int i = 0; i < 3; ++i) {
        if (i > 0) ImGui::SameLine(0, spaceX);
        
        ImGui::AlignTextToFramePadding(); 
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", labels[i]);
        ImGui::SameLine(0, spaceX);
        
        ImGui::PushID(i);
        ImGui::SetNextItemWidth(inputWidth);
        
        if (ImGui::DragFloat("##v", &v[i], 0.01f * increment, v_min, v_max, format)) {
            valueChanged = true;
        }
        
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            edited = true;
        }
        ImGui::PopID();
    }
    
    ImGui::PopID();

    return returnEdit ? edited : valueChanged;
}

inline float calculateTime(int hours, int minutes, int seconds, float milliseconds)
{
    return hours*3600 + minutes*60 + seconds + milliseconds/1000;
}

inline void SpacingH(float px)
{
    ImGui::SameLine();
    ImGui::Dummy({px, 0});
}

inline void SpacingV(float px)
{
    ImGui::SameLine();
    ImGui::Dummy({0, px});
}

inline void TextElided(const std::string& text, float maxWidth)
{
    if (ImGui::CalcTextSize(text.c_str()).x <= maxWidth)
    {
        ImGui::Text("%s", text.c_str());
        return;
    }

    std::string ellipsis = "...";
    float ellipsisWidth = ImGui::CalcTextSize(ellipsis.c_str()).x;

    if (maxWidth <= ellipsisWidth)
    {
        ImGui::Text("...");
        return;
    }

    std::string elided = text;
    while (!elided.empty() && ImGui::CalcTextSize((elided + ellipsis).c_str()).x > maxWidth)
        elided.pop_back();

    ImGui::Text("%s", (elided + ellipsis).c_str());
}

inline bool DrawFieldTime(float* variable, float v_min, float v_max, int invSize = 1, bool returnEdit = false)
{
    bool valueChanged = false;
    bool edited = false;
    float originalValue = *variable;
    float time = *variable;
    int timeInt = (int)time;
    
    int h = timeInt / 3600;
    int m = (timeInt / 60) % 60;
    int s = timeInt % 60;
    int ms = (int)((time - (float)timeInt) * 1000.0f + 0.5f);

    float totalWidth = ImGui::GetContentRegionAvail().x / invSize;
    float fieldWidth = (totalWidth / 4.0f) - 6.0f;

    ImGui::SetNextItemWidth(fieldWidth);
    if (ImGui::InputInt("hours", &h, 0)) valueChanged = true;
    if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

    SpacingH(5);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(fieldWidth);
    if (ImGui::InputInt("minutes", &m, 0)) valueChanged = true;
    if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;
    
    SpacingH(5);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(fieldWidth);
    if (ImGui::InputInt("seconds", &s, 0)) valueChanged = true;
    if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

    SpacingH(5);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(fieldWidth);
    if (ImGui::InputInt("ms", &ms, 0)) valueChanged = true;
    if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

    if (valueChanged)
    {
        if (h < 0) h = 0;
        
        if (m < 0) m = 0;
        if (m > 59) m = 59;
        
        if (s < 0) s = 0;
        if (s > 59) s = 59;
        
        if (ms < 0) ms = 0;
        if (ms > 999) ms = 999;

        float newTime = calculateTime(h, m, s, ms);
        
        if (newTime < v_min) newTime = v_min;
        if (newTime > v_max) newTime = v_max;
        
        *variable = newTime;
    }
    if (returnEdit) return edited;
    return originalValue != *variable;
}

inline void Indent(int n = 1)
{
    for (int i = 0; i<n; i++)
        ImGui::Indent();
}

inline void Unindent(int n = 1)
{
    for (int i = 0; i<n; i++)
        ImGui::Unindent();
}

inline void Spacing(int n = 1)
{
    for (int i = 0; i<n; i++)
        ImGui::Spacing();
}

inline void Separator(int n = 1)
{
    for (int i = 0; i<n; i++)
        ImGui::Separator();
}

template <typename T>
inline bool DrawEnumCombo(const char* label, T* currentVar, const std::vector<ComboEntry<T>>& options, float invSize = 1)
{
    bool valueChanged = false;

    const char* previewValue = "Unknown";
    for (const auto& option : options) {
        if (option.value == *currentVar) {
            previewValue = option.name;
            break;
        }
    }

    float totalWidth = ImGui::GetContentRegionAvail().x / invSize;
    ImGui::SetNextItemWidth(totalWidth);
    if (ImGui::BeginCombo(label, previewValue))
    {
        for (const auto& option : options)
        {
            bool isSelected = (*currentVar == option.value);

            if (ImGui::Selectable(option.name, isSelected))
            {
                *currentVar = option.value;
                valueChanged = true;
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    
    return valueChanged;
}

inline void TextUnformatted(const char* text, float size = 1)
{
    ImGui::SetWindowFontScale(size);
    ImGui::TextUnformatted(text);
    ImGui::SetWindowFontScale(1.0f);
}

inline ImVec4 ColorFromHash(const std::string& name) 
{
    std::hash<std::string> hasher;
    size_t hash = hasher(name);
    
    float r = (hash & 0xFF) / 255.0f;
    float g = ((hash >> 8) & 0xFF) / 255.0f;
    float b = ((hash >> 16) & 0xFF) / 255.0f;
    
    r = (r + 1.0f) / 2.0f;
    g = (g + 1.0f) / 2.0f;
    b = (b + 1.0f) / 2.0f;
    
    return ImVec4(r, g, b, 1.0f);
}

inline void DrawCenteredIcon(ImTextureID tex, const char* fallbackText, ImVec2 iconSize)
{
    float availWidth = ImGui::GetContentRegionAvail().x;

    if (tex != (ImTextureID)0)
    {
        float offsetX = (availWidth - iconSize.x) * 0.5f;
        if (offsetX > 0.0f) 
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        
        ImGui::Image(tex, iconSize);
    }
    else
    {
        float textWidth = ImGui::CalcTextSize(fallbackText).x;
        float offsetX = (availWidth - textWidth) * 0.5f;
        if (offsetX > 0.0f) 
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        
        ImGui::TextUnformatted(fallbackText);
    }
}

inline void DrawMasterVUMeter(ImVec2 pos, ImVec2 size, float currentMasterLevel) 
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    static float smoothedLevel = 0.0f;
    float deltaTime = ImGui::GetIO().DeltaTime;
    
    if (currentMasterLevel > smoothedLevel) {
        smoothedLevel = currentMasterLevel;
    } else {
        smoothedLevel = ImLerp(smoothedLevel, currentMasterLevel, deltaTime * 5.0f); 
    }

    drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(20, 20, 20, 255));

    float displayLevel = std::min(smoothedLevel, 1.05f) / 1.05f;
    float fillHeight = size.y * displayLevel;
    float fillStartY = (pos.y + size.y) - fillHeight;

    drawList->PushClipRect(ImVec2(pos.x, fillStartY), ImVec2(pos.x + size.x, pos.y + size.y), true);
    
    ImU32 colBottom = IM_COL32(0, 255, 0, 255);
    ImU32 colMid    = IM_COL32(255, 255, 0, 255);
    ImU32 colTop    = IM_COL32(255, 0, 0, 255);
    
    float midY = pos.y + (size.y * 0.3f);
    drawList->AddRectFilledMultiColor(pos, ImVec2(pos.x + size.x, midY), colTop, colTop, colMid, colMid);
    drawList->AddRectFilledMultiColor(ImVec2(pos.x, midY), ImVec2(pos.x + size.x, pos.y + size.y), colMid, colMid, colBottom, colBottom);
    
    drawList->PopClipRect();
    drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(50, 50, 50, 255));
}

inline void SetTooltip(const char* text, bool onHovered = true)
{
    if (!onHovered || ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::SetTooltip("Automation view");
        ImGui::EndTooltip();
    }
}

inline void SetButtonColorsGrey()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.22f, 0.24f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.30f, 0.32f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.38f, 0.40f, 0.42f, 1.0f));
}

inline void PopButtonColorsGrey()
{
    ImGui::PopStyleColor(3);
}

inline void SetButtonColorsPositive()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.9f, 0.4f, 1.0f));
}

inline void PopButtonColorsPositive()
{
    ImGui::PopStyleColor(3);
}

inline void SetButtonColorsNegative()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.0f, 0.0f, 1.0f));
}

inline void PopButtonColorsNegative()
{
    ImGui::PopStyleColor(3);
}

inline void SetButtonColorsWarning()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.6f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.7f, 0.0f, 1.0f));
}

inline void PopButtonColorsWarning()
{
    ImGui::PopStyleColor(3);
}

inline void SetButtonColorsUtility()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.4f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.5f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.6f, 1.0f, 1.0f));
}

inline void PopButtonColorsUtility()
{
    ImGui::PopStyleColor(3);
}

inline void SetButtonColorsTransparent()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0,0,0,0));
}

inline void PopButtonColorsTransparent()
{
    ImGui::PopStyleColor(3);
}

inline bool AreImVec4Different(ImVec4 a, ImVec4 b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

inline void PushButtonColor(BUTTON_COLORS color)
{
    using BC = BUTTON_COLORS;
    switch (color)
    {
        case BC::POSITIVE: SetButtonColorsPositive(); break;
        case BC::NEGATIVE: SetButtonColorsNegative(); break;
        case BC::WARNING: SetButtonColorsWarning(); break;
        case BC::UTILITY: SetButtonColorsUtility(); break;
        case BC::GREY: SetButtonColorsGrey(); break;
        case BC::INVISIBLE: SetButtonColorsTransparent(); break;
        case BC::NONE: return;
    }
}

inline void PopButtonColor(BUTTON_COLORS color)
{
    if (color == BUTTON_COLORS::NONE)
        return;
    ImGui::PopStyleColor(3);
}

inline bool DrawButtonColored(const char* label, BUTTON_COLORS color, bool colorOnlyOnActive = false, bool value = false,  ImVec2 size = ImVec2(0,0), 
    bool autoScaleText = true)
{
    bool shouldScale = autoScaleText && (size.x != 0.0f && size.y != 0.0f); 
    
    if (shouldScale)
    {
        ImVec2 actualSize = size;
        if (actualSize.x <= 0.0f) actualSize.x = ImGui::GetContentRegionAvail().x + actualSize.x;
        if (actualSize.y <= 0.0f) actualSize.y = ImGui::GetFrameHeight();

        ImVec2 padding = ImGui::GetStyle().FramePadding;
        ImVec2 availableSpace = ImVec2(actualSize.x - (padding.x * 2.0f), actualSize.y - (padding.y * 2.0f));
        ImVec2 textSize = ImGui::CalcTextSize(label);

        if (textSize.x > 0 && textSize.y > 0)
        {
            float scaleX = availableSpace.x / textSize.x;
            float scaleY = availableSpace.y / textSize.y;

            float targetScale = std::min(scaleX, scaleY);
            if (targetScale > 1.0f) targetScale = 1.0f;
            if (targetScale <= 0) targetScale = 0.1f;
            ImGui::SetWindowFontScale(targetScale);
        }
    }

    bool pressed = false;
    if (colorOnlyOnActive)
    {
        bool originalValue = value;
        if (originalValue) PushButtonColor(color);
        pressed = ImGui::Button(label, size);
        if (originalValue) PopButtonColor(color);
    } else {
        PushButtonColor(color);
        pressed = ImGui::Button(label, size);
        PopButtonColor(color);
    }
    if (shouldScale)
        ImGui::SetWindowFontScale(1.0f);

    return pressed;
}

inline bool DrawButtonImage(ImTextureID tex, ImVec2 iconSize, BUTTON_COLORS color = BUTTON_COLORS::NONE, const char* fallback = "", bool colorOnlyOnActive = false, bool value = false, ImVec2 size = ImVec2(0,0))
{
    if (tex == (ImTextureID)0)
        return DrawButtonColored(fallback, color, colorOnlyOnActive, value, size, true);

    if (size.x == 0.0f && size.y == 0.0f)
        size = ImVec2(iconSize.x + 8.0f, iconSize.y + 8.0f);

    bool pressed = false;

    if (colorOnlyOnActive) {
        if (value) 
            PushButtonColor(color);
    }
    else
        PushButtonColor(color);

    ImGui::PushID(fallback);
    pressed = ImGui::Button("", size);
    
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    ImVec2 actualSize = ImVec2(rectMax.x - rectMin.x, rectMax.y - rectMin.y);

    float offsetX = (actualSize.x - iconSize.x) * 0.5f;
    float offsetY = (actualSize.y - iconSize.y) * 0.5f;

    ImGui::GetWindowDrawList()->AddImage(
        tex, 
        ImVec2(rectMin.x + offsetX, rectMin.y + offsetY), 
        ImVec2(rectMin.x + offsetX + iconSize.x, rectMin.y + offsetY + iconSize.y)
    );
    
    ImGui::PopID();

    if (colorOnlyOnActive)
    {
        if (value)
            PopButtonColor(color);
    } 
    else 
        PopButtonColor(color);

    return pressed;
}

namespace NotificationSystem {

    struct Notification {
        std::string message;
        ImVec4 color;
        float duration;
        float timer;
        float alpha;
    };

    inline std::vector<Notification> activeNotifications;

    inline void Show(const char* label, ImVec4 color = TOAST_SUCCESS, float duration = 3.0f) {
        Notification n;
        n.message = label;
        n.color = color;
        n.duration = duration;
        n.timer = duration;
        n.alpha = 0.0f;
        activeNotifications.push_back(n);
    }

    inline void Show(float num, ImVec4 color = TOAST_SUCCESS, float duration = 3.0f)
    {
        std::string text = std::to_string(num);
        Show(text.c_str(), color, duration);
    }

    inline void Render() {
        if (activeNotifications.empty()) return;

        float startY = 40.0f;
        float padding = 10.0f;
        
        ImGuiIO& io = ImGui::GetIO();
        float deltaTime = io.DeltaTime;
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                                 ImGuiWindowFlags_AlwaysAutoResize | 
                                 ImGuiWindowFlags_NoSavedSettings | 
                                 ImGuiWindowFlags_NoFocusOnAppearing | 
                                 ImGuiWindowFlags_NoNav |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoInputs;

        for (size_t i = 0; i < activeNotifications.size(); i++) {
            Notification& n = activeNotifications[i];

            if (n.duration - n.timer < 0.5f) {
                n.alpha = (n.duration - n.timer) / 0.5f;
            } 
            else if (n.timer < 1.0f) {
                n.alpha = n.timer / 1.0f;
            } 
            else {
                n.alpha = 1.0f;
            }
            n.timer -= deltaTime;

            ImGui::SetNextWindowBgAlpha(0.9f * n.alpha); 
            std::string id = "##NOTIFY" + std::to_string(i);
            
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, startY), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
            
            ImGui::PushStyleColor(ImGuiCol_WindowBg, n.color);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,1,1));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1,1,1,0.5f));
            
            if (ImGui::Begin(id.c_str(), nullptr, flags)) {
                ImGui::Text("%s", n.message.c_str());
                float height = ImGui::GetWindowHeight();
                startY += height + padding;
            }
            ImGui::End();
            
            ImGui::PopStyleColor(3);
        }
        activeNotifications.erase(
            std::remove_if(activeNotifications.begin(), activeNotifications.end(),
                [](const Notification& n) { return n.timer <= 0.0f; }),
            activeNotifications.end()
        );
    }
}