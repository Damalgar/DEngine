#include "UI/Panels/UITags.h"
#include "Vendor/imgui/imgui.h"
#include "Tools/drawUtils.h"
#include "Core/Application.h"
#include "Core/TagManager.h"
#include "Core/SceneObject.h"
#include "Core/TelemetryManager.h"
#include "UI/SelectionManager.h"

void UITags::Draw()
{
    if (ImGui::BeginTabBar("TagsTabBar"))
    {
        if (ImGui::BeginTabItem("Tags"))
        {
            DrawTags();
            ImGui::EndTabItem();
        }

        if (!TelemetryManager::IsCsvLoaded())
            ImGui::BeginDisabled();

        if (ImGui::BeginTabItem("Parameters"))
        {
            DrawParameters();
            ImGui::EndTabItem();
        }

        if (!TelemetryManager::IsCsvLoaded())
            ImGui::EndDisabled();

        ImGui::EndTabBar();
    }
}

void UITags::OnCsvLoaded()
{
    if (!TelemetryManager::IsCsvLoaded())
        return;

    std::unordered_map<std::string, bool> oldMap = std::move(m_trackedParametersMap);
    m_trackedParametersMap.clear();
    m_trackedParameters.clear();

    const std::vector<std::string>& parameters = TelemetryManager::GetValidParameters();
    for (const std::string& param : parameters)
    {
        if (oldMap.find(param) != oldMap.end())
        {
            m_trackedParametersMap[param] = oldMap[param];
            m_trackedParameters.insert(param);
        } else
            m_trackedParametersMap[param] = false;
    }
}

void UITags::DrawTags()
{
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::Indent(10.0f);
    ImGui::TextDisabled("Tags");
    ImGui::Unindent(10.0f);
    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    std::vector<std::string> uniqueTags;
    const std::vector<SceneObject*> sceneObjects = Application::Instance->GetSceneObjects();

    static char newTagBuf[64] = "";
    ImGui::InputText("##NewTag", newTagBuf, sizeof(newTagBuf));
    ImGui::SameLine();
    if (DrawButtonColored("+", BUTTON_COLORS::GREY))
    {
        if (strlen(newTagBuf) > 0)
        {
            bool added = TagManager::AddTag(newTagBuf);
            if (added)
            {
                std::string notify = "Tag " + std::string(newTagBuf) + " added"; 
                NotificationSystem::Show(notify.c_str(), TOAST_INFO);
                newTagBuf[0] = '\0';
            } else
            {
                std::string notify = "Tag " + std::string(newTagBuf) + " already exists"; 
                NotificationSystem::Show(notify.c_str(), TOAST_WARNING);
            }
        }
    }

    const std::vector<std::string>& tags = TagManager::GetTags();
    std::string tagToRemove = "";
    for (const std::string& tag : tags)
    {
        if (tag.empty() || tag == "")
        {
            TagManager::RemoveTag("");
            continue;
        }

        ImGui::PushID(tag.c_str());
        bool isVisible = TagManager::IsTagVisible(tag);
        if (ImGui::Checkbox(tag.c_str(), &isVisible))
            TagManager::SetTagVisibility(tag, isVisible);

        if (tag != "Default")
        {
            ImGui::SameLine(ImGui::GetWindowWidth() - 50);
            if (DrawButtonColored("X", BUTTON_COLORS::RED))
                tagToRemove = tag;
        }
        ImGui::PopID();
    }
    if (tagToRemove != "")
        TagManager::RemoveTag(tagToRemove);

    Spacing(3);
    ImGui::PopStyleVar();
}

void UITags::DrawParameters()
{
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::Indent(10.0f);
    ImGui::TextDisabled("Parameters");
    ImGui::Unindent(10.0f);
    ImGui::Separator();

    DrawToggleSwitch("Show Selected Object In Graph", &m_focusOnSelectedObj);
    DrawToggleSwitch("Show Parameters In Graph", &m_focusOnTrackedParameters);

    static ImGuiTextFilter parameterFilter;
    parameterFilter.Draw("Search...##ParamFilter", ImGui::GetContentRegionAvail().x);

    if (ImGui::BeginChild("ParametersList", ImVec2(0,0), true))
    {
        for (auto& it : m_trackedParametersMap)
        {
            const std::string& param = it.first;
            if (parameterFilter.PassFilter(param.c_str()))
            {
                if (ImGui::Checkbox(param.c_str(), &it.second))
                {
                    if (it.second)
                        m_trackedParameters.insert(param);
                    else
                        m_trackedParameters.erase(param);
                }
            }
        }
    }

    ImGui::EndChild(); 
}

std::unordered_set<std::string> UITags::GetTrackedParameters()
{
    //TODO: find a better way to avoid creating a new set every frame (maybe a class set that contains all the tracked parameters?)
    std::unordered_set<std::string> set;

    if (m_focusOnTrackedParameters)
        set = m_trackedParameters;

    if (m_focusOnSelectedObj)
    {
        SceneObject* obj = SelectionManager::GetAsSceneObject();
        if (obj != nullptr)
        {
            TelemetryViewer* telemetryViewer = obj->GetComponent<TelemetryViewer>();
            if (telemetryViewer != nullptr)
                set.insert(telemetryViewer->GetTargetParameter());
        }
    }
    return set;
}