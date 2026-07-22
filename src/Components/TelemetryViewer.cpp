#include "TelemetryViewer.h"
#include "Core/SceneObject.h"
#include "Components/MeshRenderer.h"

void TelemetryViewer::Update()
{
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

    mat->SetTintColor(tintColor);
}

vec4 TelemetryViewer::GetTintColorRangeMode()
{
    float range = m_rangeValueEnd - m_rangeValueStart;
    float interpValue = (m_simulatedValue - m_rangeValueStart) / range;
    interpValue = std::clamp(interpValue, 0.0f, 1.0f);
    return mix(m_rangeColorStart, m_rangeColorEnd, interpValue);
}

vec4 TelemetryViewer::GetTintColorThresholdMode()
{
    if (m_simulatedValue >= m_thresholdValue)
        return m_thresholdColorAfter;
    return m_thresholdColorBefore;
}

vec4 TelemetryViewer::GetTintColorSwitchMode()
{
    if (m_simulatedValue > 0.0f)
        return m_switchColorOn;
    return m_switchColorOff;
}

void TelemetryViewer::OnGuiDraw()
{
    DrawEnumCombo("", &m_telemetryMode, TelemetryModesOptions, 1);
    Spacing(2);
    DrawHybridFloat("valore simulato", &m_simulatedValue, 0, 200, "%.1f");
    Spacing(2);

    switch (m_telemetryMode)
    {
        case TELEMETRY_MODE::RANGE: OnGuiDrawRangeMode(); break;
        case TELEMETRY_MODE::SWITCH: OnGuiDrawSwitchMode(); break;
        case TELEMETRY_MODE::THRESHOLD: OnGuiDrawThresholdMode(); break;
    }
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

//TODO: ToJson e FromJson
json TelemetryViewer::ToJson() const
{
    json j;
    return j;
}

void TelemetryViewer::FromJson(const json& j)
{

}