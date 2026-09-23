#include "TelemetryViewer.h"
#include "Core/SceneObject.h"
#include "Components/MeshRenderer.h"
#include "Utils/Utils.h"
#include "Core/TelemetryManager.h"

void TelemetryViewer::Update()
{
    if (m_targetParameter.empty() || m_targetParameter == "") 
        return;

    m_currentValue = TelemetryManager::GetValue(m_targetParameter);

    MeshRenderer* renderer = m_sceneObject->GetComponent<MeshRenderer>();
    if (!renderer)
        return;

    Material* mat = renderer->GetMaterial();

    if (!mat)
        return;
    
    vec4 tintColor = vec4(1);
    switch (m_telemetryMode)
    {
        case TELEMETRY_MODE::RANGE: tintColor = GetTintColorRangeMode(); break;
        case TELEMETRY_MODE::SWITCH: tintColor = GetTintColorSwitchMode(); break;
        case TELEMETRY_MODE::THRESHOLD: tintColor = GetTintColorThresholdMode(); break;
    }

    if (std::isnan(m_currentValue))
        renderer->ClearOverride("material.tintColor");
    else
        renderer->SetVec4Override("material.tintColor", tintColor);
}

vec4 TelemetryViewer::GetTintColorRangeMode()
{
    float range = m_rangeValueEnd - m_rangeValueStart;
    if (range == 0.0f) return m_rangeColorStart;
    float interpValue = (m_currentValue - m_rangeValueStart) / range;
    interpValue = std::clamp(interpValue, 0.0f, 1.0f);
    return mix(m_rangeColorStart, m_rangeColorEnd, interpValue);
}

vec4 TelemetryViewer::GetTintColorThresholdMode()
{
    if (m_currentValue >= m_thresholdValue)
        return m_thresholdColorAfter;
    return m_thresholdColorBefore;
}

vec4 TelemetryViewer::GetTintColorSwitchMode()
{
    if (m_currentValue > 0.0f)
        return m_switchColorOn;
    return m_switchColorOff;
}

void TelemetryViewer::OnGuiDraw()
{
    DrawEnumCombo("", &m_telemetryMode, TelemetryModesOptions, 1);

    static ImGuiTextFilter parameterFilter;

    const auto& validParams = TelemetryManager::GetValidParameters();
    const auto& invalidParams = TelemetryManager::GetInvalidParameters();

    std::string previewText = m_targetParameter.empty() ? "No parameter" : m_targetParameter;

    if (ImGui::BeginCombo("parameter", previewText.c_str()))
    {
        parameterFilter.Draw("Search##param");
        ImGui::Separator();

        bool drewValid = false;
        for (const std::string& param : validParams)
        {
            if (!parameterFilter.PassFilter(param.c_str()))
                continue;

            drewValid = true;
            bool isSelected = (m_targetParameter == param);
            
            if (ImGui::Selectable(param.c_str(), isSelected))
                m_targetParameter = param;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        bool hasInvalidMatches = false;
        for (const std::string& param : invalidParams)
        {
            if (parameterFilter.PassFilter(param.c_str()))
            {
                hasInvalidMatches = true;
                break;
            }
        }

        if (hasInvalidMatches)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
            for (const std::string& param : invalidParams)
            {
                if (parameterFilter.PassFilter(param.c_str()))
                    TextUnformatted(param.c_str()); 
            }
            ImGui::PopStyleColor();
        }
        
        ImGui::EndCombo();
    }

    if (ImGui::IsItemDeactivated())
        parameterFilter.Clear();

    if (!m_targetParameter.empty())
        ImGui::TextDisabled("Current value (Frame %d): %.2f", TelemetryManager::GetCurrentFrame(), m_currentValue);

    switch (m_telemetryMode)
    {
        case TELEMETRY_MODE::RANGE: OnGuiDrawRangeMode(); break;
        case TELEMETRY_MODE::SWITCH: OnGuiDrawSwitchMode(); break;
        case TELEMETRY_MODE::THRESHOLD: OnGuiDrawThresholdMode(); break;
    }

    if(!m_targetParameter.empty() && m_telemetryMode != TELEMETRY_MODE::SWITCH && DrawButtonColored("Auto-Range", BUTTON_COLORS::BLUE))
        SetSuggestedValues();
}

void TelemetryViewer::SetSuggestedValues()
{
    std::vector<float> suggestedValues = TelemetryManager::GetSuggestedRange(m_targetParameter);
    if (suggestedValues.empty())
        return;

    m_rangeValueStart = suggestedValues[0];
    m_rangeValueEnd = suggestedValues[1];
    
    m_thresholdValue = suggestedValues[2];
}

void TelemetryViewer::OnGuiDrawRangeMode()
{
    DrawFieldFloat("Start", &m_rangeValueStart, 5);
    ImGui::SameLine();
    ImGui::ColorEdit4("##StartColor", &m_rangeColorStart.x, ImGuiColorEditFlags_NoInputs);

    Spacing(2);

    DrawFieldFloat("End", &m_rangeValueEnd, 5);
    ImGui::SameLine();
    ImGui::ColorEdit4("##EndColor", &m_rangeColorEnd.x, ImGuiColorEditFlags_NoInputs);
}

void TelemetryViewer::OnGuiDrawSwitchMode()
{
    ImGui::ColorEdit4("Off", &m_switchColorOff.x, ImGuiColorEditFlags_NoInputs);
    Spacing(2);
    ImGui::ColorEdit4("On", &m_switchColorOn.x, ImGuiColorEditFlags_NoInputs);
}

void TelemetryViewer::OnGuiDrawThresholdMode()
{
    DrawFieldFloat("Threshold", &m_thresholdValue, 5);
    Spacing(2);

    ImGui::ColorEdit4("Before", &m_thresholdColorBefore.x, ImGuiColorEditFlags_NoInputs);
    Spacing(2);
    ImGui::ColorEdit4("After", &m_thresholdColorAfter.x, ImGuiColorEditFlags_NoInputs);
}

json TelemetryViewer::ToJson() const
{
    json j = Component::ToJson();

    j["telemetryMode"] = static_cast<int>(m_telemetryMode);
    j["targetParameter"] = m_targetParameter;
    
    j["rangeValueStart"] = m_rangeValueStart;
    j["rangeValueEnd"] = m_rangeValueEnd;
    j["rangeColorStart"] = m_rangeColorStart;
    j["rangeColorEnd"] = m_rangeColorEnd;

    j["thresholdValue"] = m_thresholdValue;
    j["thresholdColorBefore"] = m_thresholdColorBefore;
    j["thresholdColorAfter"] = m_thresholdColorAfter;

    j["switchColorOff"] = m_switchColorOff;
    j["switchColorOn"] = m_switchColorOn;
    return j;
}

void TelemetryViewer::FromJson(const json& j)
{
    m_telemetryMode = static_cast<TELEMETRY_MODE>(j.value("telemetryMode", 0));
    m_targetParameter = j.contains("targetParameter") ? j["targetParameter"].get<std::string>() : "";

    m_rangeValueStart = j.value("rangeValueStart", 0.0f);
    m_rangeValueEnd = j.value("rangeValueEnd", 100.0f);
    m_rangeColorStart = j["rangeColorStart"], vec4(1);
    m_rangeColorEnd = j["rangeColorEnd"], vec4(1);

    m_thresholdValue = j.value("thresholdValue", 50.0f);
    m_thresholdColorBefore = j["thresholdColorBefore"], vec4(1);
    m_thresholdColorAfter = j["thresholdColorAfter"], vec4(1);

    m_switchColorOff = j["switchColorOff"], vec4(1);
    m_switchColorOn = j["switchColorOn"], vec4(1);
}