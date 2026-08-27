#include <glad/glad.h>
#include "UI/Panels/UITimeline.h"
#include "Core/TelemetryManager.h"
#include "Vendor/imgui/implot.h"
#include "Vendor/imgui/implot_internal.h"
#include "UI/EditorCustomizations.h"
#include "IO/Console.h"
#include "IO/InputsManager.h"
#include "UI/SelectionManager.h"
#include "Components/TelemetryViewer.h"
#include "Core/SceneObject.h"

#include <algorithm>

void UITimeline::Draw()
{
    int maxFrames = TelemetryManager::GetMaxFrames();
    int currentFrame = TelemetryManager::GetCurrentFrame();

    if (maxFrames == 0)
        return;

    ImVec2 avail = ImGui::GetContentRegionAvail();
    float usableWidth = avail.x - (timelinePref.timelineMargin * 2);

    float startCursorX = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(startCursorX + timelinePref.timelineMargin);
    ImVec2 pos = ImGui::GetCursorScreenPos(); 
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    //TIMELINE BAR
    ImVec2 timelineSize = ImVec2(usableWidth, timelinePref.timelineSliderHeigth);
    
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + timelineSize.x, pos.y + timelineSize.y),
        timelinePref.timelineSliderColor,
        5.0f
    );

    float currentRange = static_cast<float>(m_viewEndFrame - m_viewStartFrame);
    float tickStep = currentRange / static_cast<float>(timelinePref.rulerTicks);
    
    float magnitude = std::pow(10.0f, std::floor(std::log10(tickStep > 0 ? tickStep : 1)));
    float normalizedStep = tickStep / magnitude;

    int step;
    if (normalizedStep < 2.0f)
        step = 1 * magnitude;
    else if (normalizedStep < 5.0f)
        step = 2 * magnitude;
    else if (normalizedStep < 10.0f)
        step = 5 * magnitude;
    else
        step = 10 * magnitude;

    if (step < 1)
        step = 1;

    int firstTickFrame = (m_viewStartFrame / step) * step;
    if (firstTickFrame < m_viewStartFrame)
        firstTickFrame += step;

    ImU32 tickColor = IM_COL32(150, 150, 150, 255);
    ImU32 textColor = IM_COL32(200, 200, 200, 255);

    for (int f = firstTickFrame; f <= m_viewEndFrame; f += step)
    {
        float normalizedX = static_cast<float>(f - m_viewStartFrame) / currentRange;
        float tickX = pos.x + (normalizedX * timelineSize.x);

        float tickHeight = timelineSize.y * 0.3f; 
        drawList->AddLine(
            ImVec2(tickX, pos.y + timelineSize.y - tickHeight),
            ImVec2(tickX, pos.y + timelineSize.y),
            tickColor,
            1.0f
        );

        std::string frameText = std::to_string(f);
        ImVec2 textSize = ImGui::CalcTextSize(frameText.c_str());
        
        drawList->AddText(
            ImVec2(tickX - (textSize.x * 0.5f), pos.y + timelineSize.y - tickHeight - textSize.y - 2.0f),
            textColor,
            frameText.c_str()
        );
    }

    ImGui::InvisibleButton("##TimelineButton", timelineSize);

    bool isActive = ImGui::IsItemActive();
    bool isHovered = ImGui::IsItemHovered();
    float mouseWheelScrool = InputsManager::MouseWheelScroll();

    if (isHovered && InputsManager::IsMouseButtonPressed(ImGuiMouseButton_Middle))
    {
        m_isPanning = true;
        m_panAccumulator = 0;
    }

    if (m_isPanning)
    {
        if (!InputsManager::IsMouseButtonDown(ImGuiMouseButton_Middle))
            m_isPanning = false;
        else {
            float mouseDeltaX = ImGui::GetIO().MouseDelta.x;
            if (mouseDeltaX != 0)
            {
                float range = static_cast<float>(m_viewEndFrame - m_viewStartFrame);
                float framesPerPixel = range / timelineSize.x;

                m_panAccumulator -= mouseDeltaX * framesPerPixel;
                int panFrames = static_cast<int>(m_panAccumulator);

                if (panFrames != 0)
                {
                    m_viewStartFrame += panFrames;
                    m_viewEndFrame += panFrames;
                    m_panAccumulator -= panFrames;

                    if (m_viewStartFrame < 1) {
                        m_viewEndFrame += (1 - m_viewStartFrame);
                        m_viewStartFrame = 1;
                    }
                    if (m_viewEndFrame > maxFrames) {
                        m_viewStartFrame -= (m_viewEndFrame - maxFrames);
                        m_viewEndFrame = maxFrames;
                    }

                    m_viewStartFrame = std::clamp(m_viewStartFrame, 1, maxFrames);
                    m_viewEndFrame = std::clamp(m_viewEndFrame, 1, maxFrames);
                }
            }
        }
    }

    if (isHovered && mouseWheelScrool != 0.0f)
    {
        float mouseX = ImGui::GetIO().MousePos.x - pos.x;
        float posOnTimelineNormalized = mouseX / timelineSize.x;
        posOnTimelineNormalized = std::clamp(posOnTimelineNormalized, 0.0f, 1.0f);

        float range = m_viewEndFrame - m_viewStartFrame;
        float newRange = range * (1.0f - (mouseWheelScrool * timelinePref.zoomSpeed));
        newRange = std::clamp(newRange, 20.0f, static_cast<float>(maxFrames));
        float frameAtMouse = m_viewStartFrame + (posOnTimelineNormalized * range);
        m_viewStartFrame = static_cast<int>(std::round(frameAtMouse - (posOnTimelineNormalized * newRange)));
        m_viewEndFrame   = static_cast<int>(std::round(frameAtMouse + ((1.0f - posOnTimelineNormalized) * newRange)));

        if (m_viewStartFrame < 1) {
            m_viewEndFrame += (1 - m_viewStartFrame);
            m_viewStartFrame = 1;
        }
        if (m_viewEndFrame > maxFrames) {
            m_viewStartFrame -= (m_viewEndFrame - maxFrames);
            m_viewEndFrame = maxFrames;
        }

        m_viewStartFrame = std::clamp(m_viewStartFrame, 1, maxFrames);
        m_viewEndFrame = std::clamp(m_viewEndFrame, 1, maxFrames);
    }

    //PLAYHEAD
    if (isActive && InputsManager::IsMouseButtonDown(ImGuiMouseButton_Left))
    {
        float mouseX = ImGui::GetIO().MousePos.x - pos.x;
        float posOnTimelineNormalized = mouseX / timelineSize.x;
        posOnTimelineNormalized = std::clamp(posOnTimelineNormalized, 0.0f, 1.0f);

        int newFrame = m_viewStartFrame + static_cast<int>(posOnTimelineNormalized * (m_viewEndFrame - m_viewStartFrame));
        newFrame = std::clamp(newFrame, 1, maxFrames);
        TelemetryManager::SetFrame(newFrame);
        currentFrame = newFrame;
    }

    DrawGraph(ImVec2(timelineSize.x, ImGui::GetContentRegionAvail().y));

    if (currentFrame >= m_viewStartFrame && currentFrame <= m_viewEndFrame)
    {
        float currentRange = static_cast<float>(m_viewEndFrame - m_viewStartFrame);
        
        if (currentRange > 0) 
        {
            float playheadPosNormalized = (currentFrame - m_viewStartFrame) / currentRange;
            playheadPosNormalized = std::clamp(playheadPosNormalized, 0.0f, 1.0f);
            
            float playheadX = pos.x + (timelineSize.x * playheadPosNormalized);

            drawList->AddLine(
                ImVec2(playheadX, pos.y),
                ImVec2(playheadX, pos.y + avail.y),
                timelinePref.playheadColor,
                timelinePref.playheadThickness
            );
        }
    }
    //ImGui::Text("%d / %d   [%d - %d]", currentFrame, maxFrames, m_viewStartFrame, m_viewEndFrame);
}

void UITimeline::DrawGraph(ImVec2 size)
{
    SceneObject* selectedObj = SelectionManager::GetAsSceneObject();
    if (selectedObj == nullptr)
        return;

    TelemetryViewer* telemetryViewer = selectedObj->GetComponent<TelemetryViewer>();
    if (telemetryViewer == nullptr)
        return;

    std::string parameter = telemetryViewer->GetTargetParameter();
    const auto& values = TelemetryManager::GetValues(parameter);
    if (values.empty())
        return;
    
    double plotMinX = static_cast<double>(m_viewStartFrame);
    double plotMaxX = static_cast<double>(m_viewEndFrame);
    
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0.0f, 0.0f));
    ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0.0f, 0.1f));
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotBorderSize, 0.0f);

    float startCursorX = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(startCursorX + timelinePref.timelineMargin);
    ImPlotFlags plotFlags = ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoLegend;
    
    if (ImPlot::BeginPlot("##TimelineGraph", size, plotFlags))
    {
        ImPlot::SetupAxisLimits(ImAxis_X1, plotMinX, plotMaxX, ImGuiCond_Always);
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoDecorations);

        ImPlotPlot* currentPlot = ImPlot::GetCurrentPlot();
        if (currentPlot) 
        {
            plotMinX = currentPlot->Axes[ImAxis_X1].Range.Min;
            plotMaxX = currentPlot->Axes[ImAxis_X1].Range.Max;
        }

        int range = m_viewEndFrame - m_viewStartFrame;
        if (range < 100)
        {
            ImPlotSpec spec;
            spec.Marker = ImPlotMarker_Circle;
            spec.MarkerSize = 4.0f;
            spec.MarkerFillColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
            spec.MarkerLineColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            spec.LineWeight = 1.5f;

            ImPlot::PlotLine(parameter.c_str(), values.data(), static_cast<int>(values.size()), 1.0, 1.0, spec);
        } else
            ImPlot::PlotLine(parameter.c_str(), values.data(), static_cast<int>(values.size()), 1.0, 1.0);

        if (ImPlot::IsPlotHovered())
        {
            ImPlotPoint mousePos = ImPlot::GetPlotMousePos();
            int hoveredFrame = static_cast<int>(std::round(mousePos.x));
            int valIndex = hoveredFrame - 1;

            if (valIndex >= 0 && valIndex < static_cast<int>(values.size()))
            {
                double ptX = static_cast<double>(hoveredFrame);
                double ptY = static_cast<double>(values[valIndex]);
                ImPlotSpec spec;
                spec.Marker = ImPlotMarker_Circle;
                spec.MarkerSize = 4.0f;
                spec.MarkerFillColor = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
                spec.MarkerLineColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                spec.LineWeight = 1.5f;

                ImPlot::PlotScatter("##HoverHighlight", &ptX, &ptY, 1, spec);

                ImGui::BeginTooltip();
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", parameter.c_str());
                ImGui::Separator();
                ImGui::Text("Value: %.4f", ptY);
                ImGui::Text("Frame: %d", hoveredFrame);
                ImGui::EndTooltip();
            }
        }
        ImPlot::EndPlot();
    }

    ImPlot::PopStyleVar(3);

    m_viewStartFrame = static_cast<int>(plotMinX);
    m_viewEndFrame = static_cast<int>(plotMaxX);

    if (m_viewStartFrame < 0)
        m_viewStartFrame = 0;
    
    if (m_viewEndFrame > TelemetryManager::GetMaxFrames())
        m_viewEndFrame = TelemetryManager::GetMaxFrames();
}