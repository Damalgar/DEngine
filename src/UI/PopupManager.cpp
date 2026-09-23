#include "UI/PopupManager.h"
#include "Vendor/imgui/imgui.h"
#include "Fonts/FontsLoader.h"
#include "Tools/drawUtils.h"
#include "Core/SceneManager.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Components/include_components.h"

void PopupManager::Draw()
{
    DrawConfirmPopup();
    DrawAddModelPopup();
}

void PopupManager::OpenConfirmPopup(const std::string& title, const std::string& message, std::function<void()> onConfirm, std::function<void()> onCancel)
{
    m_confirmPopupInfos.title = title;
    m_confirmPopupInfos.message = message;
    m_confirmPopupInfos.onConfirm = onConfirm;
    m_confirmPopupInfos.onCancel = onCancel;
    m_confirmPopupInfos.shouldOpen = true;
}

void PopupManager::OpenAddModelPopup(const std::string& modelName, std::function<void(SceneObject*)> onMountSelected)
{
    m_addModelPopupInfos.modelName = modelName;
    m_addModelPopupInfos.onMountSelected = onMountSelected;
    m_addModelPopupInfos.selectedMountPoint = nullptr;
    m_addModelPopupInfos.shouldOpen = true;
}

void PopupManager::DrawConfirmPopup()
{
    if (m_confirmPopupInfos.shouldOpen)
    {
        m_confirmPopupInfos.shouldOpen = false;
        ImGui::OpenPopup(m_confirmPopupInfos.title.c_str());
    }

    if (ImGui::BeginPopupModal(m_confirmPopupInfos.title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        TextUnformatted(m_confirmPopupInfos.message);
        Separator();

        if (DrawButtonColored("Confirm", BUTTON_COLORS::GREEN))
        {
            if (m_confirmPopupInfos.onConfirm) m_confirmPopupInfos.onConfirm();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (DrawButtonColored("Cancel", BUTTON_COLORS::GREEN))
        {
            if (m_confirmPopupInfos.onCancel) m_confirmPopupInfos.onCancel();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void PopupManager::DrawAddModelPopup()
{
    if (m_addModelPopupInfos.shouldOpen)
    {
        m_addModelPopupInfos.shouldOpen = false;
        ImGui::OpenPopup("Add Model to Scene");
    }

    if (ImGui::BeginPopupModal("Add Model to Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        TextUnformatted("Importing " + m_addModelPopupInfos.modelName);
        TextUnformatted("Select mount point for your model");
        Spacing(2);
        Separator();
        Spacing(2);

        auto& sceneObjects = SceneManager::GetActiveScene()->GetSceneObjects();
        std::vector<SceneObject*> mountPoints{};
        for (SceneObject* obj : sceneObjects)
        {
            MountPoint* mountPoint = obj->GetComponent<MountPoint>();
            if (mountPoint)
                mountPoints.push_back(obj);
        }

        if (ImGui::BeginChild("MountPointsList", ImVec2(400, 500), true))
        {
            if (ImGui::Selectable("None (Place at origin)", m_addModelPopupInfos.selectedMountPoint == nullptr))
                m_addModelPopupInfos.selectedMountPoint = nullptr;

            for (SceneObject* obj : mountPoints)
            {
                ImGui::PushID(obj);
                
                glm::vec4 color = obj->GetComponent<MountPoint>()->GetGizmosTintColor();
                ImVec4 colorIm = ImVec4(color.x, color.y, color.z, color.w);
                ImGui::PushStyleColor(ImGuiCol_Text, colorIm);
                if (ImGui::Selectable(obj->name.c_str(), m_addModelPopupInfos.selectedMountPoint == obj))
                    m_addModelPopupInfos.selectedMountPoint = obj;
                ImGui::PopStyleColor();

                ImGui::PopID();
            }

            ImGui::EndChild();
        }

        if (DrawButtonColored("Import", BUTTON_COLORS::GREEN))
        {
            if (m_addModelPopupInfos.onMountSelected) m_addModelPopupInfos.onMountSelected(m_addModelPopupInfos.selectedMountPoint);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (DrawButtonColored("Cancel", BUTTON_COLORS::RED))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}