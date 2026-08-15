#include "UI/Panels/UITimeline.h"
#include "Core/TelemetryManager.h"
#include "Vendor/imgui/imgui.h"
#include "UI/EditorCustomizations.h"
#include "IO/Console.h"
#include "IO/InputsManager.h"

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

    ImGui::Text("%d / %d   [%d - %d]", currentFrame, maxFrames, m_viewStartFrame, m_viewEndFrame);
}