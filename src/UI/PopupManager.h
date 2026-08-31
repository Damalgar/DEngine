#pragma once
#include <string>
#include <functional>

class SceneObject;

struct ConfirmPopupInfos
{
    std::string title;
    std::string message;
    std::function<void()> onConfirm;
    std::function<void()> onCancel;
    bool shouldOpen = false;    
};

struct AddModelPopupInfos
{
    std::string modelName;
    std::function<void(SceneObject*)> onMountSelected;
    SceneObject* selectedMountPoint = nullptr;
    bool shouldOpen = false;
};

class PopupManager {
    public:
    static void Draw();

    static void OpenConfirmPopup(const std::string& title, const std::string& message, std::function<void()> onConfirm, std::function<void()> onDeny);
    static void OpenAddModelPopup(const std::string& modelName, std::function<void(SceneObject*)> onMountSelected);

    private:
    inline static ConfirmPopupInfos m_confirmPopupInfos;
    inline static AddModelPopupInfos m_addModelPopupInfos;

    static void DrawConfirmPopup();
    static void DrawAddModelPopup();
};