#include "UI/Panels/UIConsole.h"

#include <string>
#include <vector>
#include "imgui/imgui.h"
#include "Tools/drawUtils.h"
#include "Icons/IconsLoader.h"

void UIConsole::Draw()
{
    ImVec2 filterButtonSize = ImVec2(40, 40);
    ImVec2 filterButtonImageSize = ImVec2(35, 35);
    static ImGuiTextFilter textFilter;

    float width = 400.0f;
    textFilter.Draw("##ConsoleFilter", width);

    ImGui::SameLine();
    if (DrawButtonImage(IconsLoader::infoIconText, filterButtonImageSize, BUTTON_COLORS::UTILITY, "info", true, showInfo, filterButtonSize))
        showInfo = !showInfo;

    ImGui::SameLine();
    if (DrawButtonImage(IconsLoader::warningIconText, filterButtonImageSize, BUTTON_COLORS::UTILITY, "warnings", true, showWarn, filterButtonSize))
        showWarn = !showWarn;

    ImGui::SameLine();
    if (DrawButtonImage(IconsLoader::errorIconText, filterButtonImageSize, BUTTON_COLORS::UTILITY, "errors", true, showErr, filterButtonSize))
        showErr = !showErr;

    SpacingH(10);
    ImGui::SameLine();
    if (DrawButtonColored("Clear", BUTTON_COLORS::NEGATIVE))
        Console::Clear();

    ImGui::BeginChild("LogsArea", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    const std::vector<LogMessage>& logs = Console::GetLogs();

    for (const LogMessage& log : logs)
    {
        if (!showInfo && log.level == LOG_LEVEL::INFO) continue;
        if (!showWarn && log.level == LOG_LEVEL::WARNING) continue;
        if (!showErr && log.level == LOG_LEVEL::ERROR) continue;

        if (!textFilter.PassFilter(log.text.c_str())) continue;

        //info
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        GLuint iconText = IconsLoader::infoIconText;

        switch (log.level)
        {
            case LOG_LEVEL::WARNING:
                color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
                iconText = IconsLoader::warningIconText;
                break;
            case LOG_LEVEL::ERROR:
                color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                iconText = IconsLoader::errorIconText;
                break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);

        float iconSize = ImGui::GetTextLineHeight();
        ImGui::Image((void*)(intptr_t)iconText, ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1));
        ImGui::SameLine();
        ImGui::TextUnformatted(log.text.c_str());

        ImGui::PopStyleColor();
    }

    ImGui::EndChild();
}