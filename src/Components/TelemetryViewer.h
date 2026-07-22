#pragma once
#include "Components/Component.h"
#include <glm/glm.hpp>
#include "Tools/drawUtils.h"
using namespace glm;

enum class TELEMETRY_MODE {
    RANGE = 0,
    THRESHOLD,
    SWITCH
};

class TelemetryViewer : public Component {
    public:
    TelemetryViewer(SceneObject* sceneObject) : Component(sceneObject, COMPONENT_TYPE::TELEMETRY_VIEWER) {}

    void Update() override;
    void OnGuiDraw() override;

    json ToJson() const override;
    void FromJson(const json& j) override;

    private:
    TELEMETRY_MODE m_telemetryMode = TELEMETRY_MODE::RANGE;
    std::string m_selectedChannel;

    //Range mode
    vec4 m_rangeColorStart = vec4(0.0f, 0.0f, 0.7f, 1.0f);
    vec4 m_rangeColorEnd = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    float m_rangeValueStart = 0;
    float m_rangeValueEnd = 200;

    //Threshold mod
    vec4 m_thresholdColorBefore = vec4(0.0f, 0.0f, 0.7f, 1.0f);;
    vec4 m_thresholdColorAfter = vec4(1.0f, 0.0f, 0.0f, 1.0f);;
    float m_thresholdValue = 100;

    //Switch mode
    vec4 m_switchColorOff = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 m_switchColorOn = vec4(0.0f, 0.8f, 0.0f, 1.0f);

    float m_simulatedValue = 0;

    vec4 GetTintColorRangeMode();
    vec4 GetTintColorThresholdMode();
    vec4 GetTintColorSwitchMode();

    void OnGuiDrawRangeMode();
    void OnGuiDrawThresholdMode();
    void OnGuiDrawSwitchMode();

    inline static const std::vector<ComboEntry<TELEMETRY_MODE>> TelemetryModesOptions =
    {
        {"Range", TELEMETRY_MODE::RANGE},
        {"Threshold", TELEMETRY_MODE::THRESHOLD},
        {"Switch", TELEMETRY_MODE::SWITCH}
    };
};