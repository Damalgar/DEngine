#pragma once
#include "RobotoFont.h"
#include "RobotoBlack.h"
#include "imgui.h"

class FontsLoader
{
    private:
    inline static ImFont* fontRoboto_Small = nullptr;
    inline static ImFont* fontRoboto_Standard = nullptr;
    inline static ImFont* fontRoboto_Medium = nullptr;
    inline static ImFont* fontRoboto_Big = nullptr;
    inline static ImFont* fontRoboto_Huge = nullptr;

    inline static ImFont* fontRobotoBlack_Huge = nullptr;

    public:
    enum class FONTS {
        ROBOTO = 0,
        ROBOTO_MEDIUM,
        ROBOTO_BIG,
        ROBOTO_HUGE,
        ROBOTO_BLACK_HUGE,
        ROBOTO_SMALL
    };

    static void InitializeFonts(ImGuiIO& io) 
    {
        //Standard needs to be first
        ImFontConfig fontConfigStandard;
        fontConfigStandard.OversampleH = 3; 
        fontConfigStandard.OversampleV = 3;
        fontConfigStandard.RasterizerMultiply = 1.2f;
        fontConfigStandard.FontDataOwnedByAtlas = false;

        fontRoboto_Standard = io.Fonts->AddFontFromMemoryTTF(
            (void*)Roboto_Medium_ttf, Roboto_Medium_ttf_len, 24.0f, &fontConfigStandard
        );

        ImFontConfig fontConfigSmall;
        fontConfigSmall.OversampleH = 3; 
        fontConfigSmall.OversampleV = 3;
        fontConfigSmall.RasterizerMultiply = 1.2f;
        fontConfigSmall.FontDataOwnedByAtlas = false;

        fontRoboto_Small = io.Fonts->AddFontFromMemoryTTF(
            (void*)Roboto_Medium_ttf, Roboto_Medium_ttf_len, 16.0f, &fontConfigSmall
        );

        ImFontConfig fontConfigMedium;
        fontConfigMedium.OversampleH = 2; 
        fontConfigMedium.OversampleV = 2;
        fontConfigMedium.RasterizerMultiply = 1.2f;
        fontConfigMedium.FontDataOwnedByAtlas = false;

        fontRoboto_Medium = io.Fonts->AddFontFromMemoryTTF(
            (void*)Roboto_Medium_ttf, Roboto_Medium_ttf_len, 32.0f, &fontConfigMedium
        ); 

        ImFontConfig fontConfigBig;
        fontConfigBig.OversampleH = 1; 
        fontConfigBig.OversampleV = 1;
        fontConfigBig.RasterizerMultiply = 1.2f;
        fontConfigBig.FontDataOwnedByAtlas = false;

        fontRoboto_Big = io.Fonts->AddFontFromMemoryTTF(
            (void*)Roboto_Medium_ttf, Roboto_Medium_ttf_len, 64.0f, &fontConfigBig
        );

        ImFontConfig fontConfigHuge;
        fontConfigHuge.OversampleH = 1; 
        fontConfigHuge.OversampleV = 1;
        fontConfigHuge.RasterizerMultiply = 1.2f;
        fontConfigHuge.FontDataOwnedByAtlas = false;

        fontRoboto_Huge = io.Fonts->AddFontFromMemoryTTF(
            (void*)Roboto_Medium_ttf, Roboto_Medium_ttf_len, 128.0f, &fontConfigHuge
        ); 

        ImFontConfig fontConfigRobotoBlackHuge;
        fontConfigRobotoBlackHuge.OversampleH = 1; 
        fontConfigRobotoBlackHuge.OversampleV = 1;
        fontConfigRobotoBlackHuge.RasterizerMultiply = 1.2f;
        fontConfigRobotoBlackHuge.FontDataOwnedByAtlas = false;

        fontRobotoBlack_Huge = io.Fonts->AddFontFromMemoryTTF(
            (void*)Roboto_Black_ttf, Roboto_Black_ttf_len, 128.0f, &fontConfigRobotoBlackHuge
        );
    }

    static void PushFont(FONTS size)
    {
        using S = FONTS;
        switch (size)
        {
        case S::ROBOTO_MEDIUM: ImGui::PushFont(fontRoboto_Medium); return;
        case S::ROBOTO_BIG: ImGui::PushFont(fontRoboto_Big); return;
        case S::ROBOTO_HUGE: ImGui::PushFont(fontRoboto_Huge); return;
        case S::ROBOTO_BLACK_HUGE: ImGui::PushFont(fontRobotoBlack_Huge); return;
        case S::ROBOTO_SMALL: ImGui::PushFont(fontRoboto_Small); return;
        default: ImGui::PushFont(fontRoboto_Standard); return;
        }
    }

    static void PopFont()
    {
        ImGui::PopFont();
    }
};