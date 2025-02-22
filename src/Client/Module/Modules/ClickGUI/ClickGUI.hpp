#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Elements/ClickGUIElements.hpp"
#include "Scripting/Scripting.hpp"
#include "Utils/APIUtils.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

#define colors_text FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_text")->value)
#define o_colors_text clickgui->settings.getSettingByName<float>("o_colors_text")->value
#define colors_text_rgb clickgui->settings.getSettingByName<bool>("colors_text_rgb")->value

#define colors_secondary1 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary1")->value)
#define o_colors_secondary1 clickgui->settings.getSettingByName<float>("o_colors_secondary1")->value
#define colors_secondary1_rgb clickgui->settings.getSettingByName<bool>("colors_secondary1_rgb")->value

#define colors_secondary2 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary2")->value)
#define o_colors_secondary2 clickgui->settings.getSettingByName<float>("o_colors_secondary2")->value
#define colors_secondary2_rgb clickgui->settings.getSettingByName<bool>("colors_secondary2_rgb")->value

#define colors_secondary3 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary3")->value)
#define o_colors_secondary3 clickgui->settings.getSettingByName<float>("o_colors_secondary3")->value
#define colors_secondary3_rgb clickgui->settings.getSettingByName<bool>("colors_secondary3_rgb")->value

#define colors_secondary5 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary5")->value)
#define o_colors_secondary5 clickgui->settings.getSettingByName<float>("o_colors_secondary5")->value
#define colors_secondary5_rgb clickgui->settings.getSettingByName<bool>("colors_secondary5_rgb")->value

#define colors_secondary6 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary6")->value)
#define o_colors_secondary6 clickgui->settings.getSettingByName<float>("o_colors_secondary6")->value
#define colors_secondary6_rgb clickgui->settings.getSettingByName<bool>("colors_secondary6_rgb")->value

#define colors_secondary7 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary7")->value)
#define o_colors_secondary7 clickgui->settings.getSettingByName<float>("o_colors_secondary7")->value
#define colors_secondary7_rgb clickgui->settings.getSettingByName<bool>("colors_secondary7_rgb")->value

#define colors_secondary8 FlarialGUI::HexToColorF(clickgui->settings.getSettingByName<std::string>("colors_secondary8")->value)
#define o_colors_secondary8 clickgui->settings.getSettingByName<float>("o_colors_secondary8")->value
#define colors_secondary8_rgb clickgui->settings.getSettingByName<bool>("colors_secondary8_rgb")->value

struct PageType {
    std::string type = "normal";
    std::string module;
};

class ClickGUI : public Module {
private:
    float baseHeightReal = 0.f;
    float baseHeightActual = 0.00001f;
    float realBlurAmount = 0.00001f;
    float radioPushAmount1 = 0.0000001f;
    float radioPushAmount2 = 0.0000001f;
    float width1 = 0.000001f;
    float width2 = 0.000001f;
    float width3 = 0.000001f;
    std::string searchBarString;
    Module* ghostMainModule;
public:
    static inline float modcardOpacity = 1.f;
    static inline float settingsOpacity = 0.f;
    static inline float scriptingOpacity = 0.f;
    static inline float modcardOffset = 0.f;
    static inline float settingsOffset = Constraints::RelativeConstraint(2.f, "height", true);
    static inline float scriptingOffset = Constraints::RelativeConstraint(4.f, "height", true);

    static inline PageType page;
    static inline std::string curr;
    static float inline accumilatedPos = 1;
    static float inline accumilatedBarPos = 1;
    static bool inline isAnimatingModSet = false;

    ClickGUI() : Module("ClickGUI", "What do you think it is?", IDR_CLICKGUI_PNG, "K") {
        Module::setup();
        this->ghostMainModule = new Module("main", "troll", IDR_COMBO_PNG, "");
        curr = "modules";
    };

    void onSetup() override {
        Listen(this, MouseEvent, &ClickGUI::onMouse)
        Listen(this, KeyEvent, &ClickGUI::onKey)
        ListenOrdered(this, RenderEvent, &ClickGUI::onRender, EventOrder::IMMEDIATE)
        Module::onEnable();
    }

    void onEnable() override {}

    void onDisable() override {}

    void terminate() override {
        Deafen(this, MouseEvent, &ClickGUI::onMouse)
        Deafen(this, KeyEvent, &ClickGUI::onKey)
        Deafen(this, RenderEvent, &ClickGUI::onRender)
        Module::terminate();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("editmenubind") == nullptr)
            settings.addSetting("editmenubind", (std::string) "L");

        if (settings.getSettingByName<std::string>("colors_text") == nullptr)
            settings.addSetting("colors_text", (std::string) "ffffff");
        if (settings.getSettingByName<float>("o_colors_text") == nullptr)
            settings.addSetting("o_colors_text", 1.0f);
        if (settings.getSettingByName<bool>("colors_text_rgb") == nullptr)
            settings.addSetting("colors_text_rgb", false);

        if (settings.getSettingByName<std::string>("colors_enabled") == nullptr)
            settings.addSetting("colors_enabled", (std::string) "188830");
        if (settings.getSettingByName<float>("o_colors_enabled") == nullptr)
            settings.addSetting("o_colors_enabled", 1.0f);
        if (settings.getSettingByName<bool>("colors_enabled_rgb") == nullptr)
            settings.addSetting("colors_enabled_rgb", false);

        if (settings.getSettingByName<std::string>("colors_disabled") == nullptr)
            settings.addSetting("colors_disabled", (std::string) "7d1820");
        if (settings.getSettingByName<float>("o_colors_disabled") == nullptr)
            settings.addSetting("o_colors_disabled", 1.0f);
        if (settings.getSettingByName<bool>("colors_disabled_rgb") == nullptr)
            settings.addSetting("colors_disabled_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary1") == nullptr)
            settings.addSetting("colors_primary1", (std::string) "ff233a");
        if (settings.getSettingByName<float>("o_colors_primary1") == nullptr)
            settings.addSetting("o_colors_primary1", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary1_rgb") == nullptr)
            settings.addSetting("colors_primary1_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary2") == nullptr)
            settings.addSetting("colors_primary2", (std::string) "ffffff");
        if (settings.getSettingByName<float>("o_colors_primary2") == nullptr)
            settings.addSetting("o_colors_primary2", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary2_rgb") == nullptr)
            settings.addSetting("colors_primary2_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary3") == nullptr)
            settings.addSetting("colors_primary3", (std::string) "9a6b72");
        if (settings.getSettingByName<float>("o_colors_primary3") == nullptr)
            settings.addSetting("o_colors_primary3", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary3_rgb") == nullptr)
            settings.addSetting("colors_primary3_rgb", false);

        if (settings.getSettingByName<std::string>("colors_primary4") == nullptr)
            settings.addSetting("colors_primary4", (std::string) "704b52");
        if (settings.getSettingByName<float>("o_colors_primary4") == nullptr)
            settings.addSetting("o_colors_primary4", 1.0f);
        if (settings.getSettingByName<bool>("colors_primary4_rgb") == nullptr)
            settings.addSetting("colors_primary4_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary1") == nullptr)
            settings.addSetting("colors_secondary1", (std::string) "3f2a2d");
        if (settings.getSettingByName<float>("o_colors_secondary1") == nullptr)
            settings.addSetting("o_colors_secondary1", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary1_rgb") == nullptr)
            settings.addSetting("colors_secondary1_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary2") == nullptr)
            settings.addSetting("colors_secondary2", (std::string) "201a1b");
        if (settings.getSettingByName<float>("o_colors_secondary2") == nullptr)
            settings.addSetting("o_colors_secondary2", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary2_rgb") == nullptr)
            settings.addSetting("colors_secondary2_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary3") == nullptr)
            settings.addSetting("colors_secondary3", (std::string) "120e0f");
        if (settings.getSettingByName<float>("o_colors_secondary3") == nullptr)
            settings.addSetting("o_colors_secondary3", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary3_rgb") == nullptr)
            settings.addSetting("colors_secondary3_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary4") == nullptr)
            settings.addSetting("colors_secondary4", (std::string) "1c1616");
        if (settings.getSettingByName<float>("o_colors_secondary4") == nullptr)
            settings.addSetting("o_colors_secondary4", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary4_rgb") == nullptr)
            settings.addSetting("colors_secondary4_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary5") == nullptr)
            settings.addSetting("colors_secondary5", (std::string) "8b1b25");
        if (settings.getSettingByName<float>("o_colors_secondary5") == nullptr)
            settings.addSetting("o_colors_secondary5", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary5_rgb") == nullptr)
            settings.addSetting("colors_secondary5_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary6") == nullptr)
            settings.addSetting("colors_secondary6", (std::string) "ff2438");
        if (settings.getSettingByName<float>("o_colors_secondary6") == nullptr)
            settings.addSetting("o_colors_secondary6", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary6_rgb") == nullptr)
            settings.addSetting("colors_secondary6_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary7") == nullptr)
            settings.addSetting("colors_secondary7", (std::string) "943c3c");
        if (settings.getSettingByName<float>("o_colors_secondary7") == nullptr)
            settings.addSetting("o_colors_secondary7", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary7_rgb") == nullptr)
            settings.addSetting("colors_secondary7_rgb", false);

        if (settings.getSettingByName<std::string>("colors_secondary8") == nullptr)
            settings.addSetting("colors_secondary8", (std::string) "302728");
        if (settings.getSettingByName<float>("o_colors_secondary8") == nullptr)
            settings.addSetting("o_colors_secondary8", 1.0f);
        if (settings.getSettingByName<bool>("colors_secondary8_rgb") == nullptr)
            settings.addSetting("colors_secondary8_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod1") == nullptr)
            settings.addSetting("colors_mod1", (std::string) "201a1b");
        if (settings.getSettingByName<float>("o_colors_mod1") == nullptr)
            settings.addSetting("o_colors_mod1", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod1_rgb") == nullptr)
            settings.addSetting("colors_mod1_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod2") == nullptr)
            settings.addSetting("colors_mod2", (std::string) "2f2022");
        if (settings.getSettingByName<float>("o_colors_mod2") == nullptr)
            settings.addSetting("o_colors_mod2", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod2_rgb") == nullptr)
            settings.addSetting("colors_mod2_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod3") == nullptr)
            settings.addSetting("colors_mod3", (std::string) "3f2a2d");
        if (settings.getSettingByName<float>("o_colors_mod3") == nullptr)
            settings.addSetting("o_colors_mod3", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod3_rgb") == nullptr)
            settings.addSetting("colors_mod3_rgb", false);

        if (settings.getSettingByName<std::string>("colors_mod4") == nullptr)
            settings.addSetting("colors_mod4", (std::string) "705d60");
        if (settings.getSettingByName<float>("o_colors_mod4") == nullptr)
            settings.addSetting("o_colors_mod4", 1.0f);
        if (settings.getSettingByName<bool>("colors_mod4_rgb") == nullptr)
            settings.addSetting("colors_mod4_rgb", false);
    }

    void settingsRender(float settingsOffset) override {

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(7.5, textWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0, "height"));

        FlarialGUI::KeybindSelector(0, x, y, getKeybind());

        y += Constraints::SpacingConstraint(0.35, textWidth);

        /*FlarialGUI::FlarialTextWithFont(x, y, L"Edit Menu", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);*/

        FlarialGUI::KeybindSelector(1, x + Constraints::SpacingConstraint(0.8, textWidth), y,
                                    settings.getSettingByName<std::string>("editmenubind")->value);

        if (settings.getSettingByName<std::string>("editmenubind")->value.empty())
            settings.getSettingByName<std::string>("editmenubind")->value = "L";

        float rectY = Constraints::PercentageConstraint(0.35, "top");
        float rectX = x;

        float tooltipV = Constraints::RelativeConstraint(0.0285, "height", true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Text Color",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(10, rectX + FlarialGUI::SettingsTextWidth("Text Color "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_text")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_text_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Text Color  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_text_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Text Color  ") + tooltipV * 4.125f, rectY,
                            "The color of the text in the Click GUI.", FlarialGUI::SettingsTextWidth("(i)"), tooltipV,
                            true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Enabled Color",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(11, rectX + FlarialGUI::SettingsTextWidth("Enabled Color "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_enabled")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_enabled_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Enabled Color  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_enabled_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Enabled Color  ") + tooltipV * 4.125f, rectY,
                            "The color of the enabled button in the Click GUI.", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Disabled Color",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(12, rectX + FlarialGUI::SettingsTextWidth("Disabled Color "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_disabled")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_disabled_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Disabled Color  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_disabled_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Disabled Color  ") + tooltipV * 4.125f, rectY,
                            "The color of the disabled button in the Click GUI.", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 1",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(13, rectX + FlarialGUI::SettingsTextWidth("Primary 1 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary1")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary1_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 1  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary1_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 1  ") + tooltipV * 4.125f, rectY,
                            "Color of active elements\nMain color of sliders\nBackground color of enabled toggles",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 2",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(14, rectX + FlarialGUI::SettingsTextWidth("Primary 2 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary2")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary2_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 2  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary2_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 2  ") + tooltipV * 4.125f, rectY,
                            "Minor color of toggles and sliders\nText Indicator color",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 3",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(15, rectX + FlarialGUI::SettingsTextWidth("Primary 3 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary3")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary3_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 3  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary3_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 3  ") + tooltipV * 4.125f, rectY,
                            "Color of inactive elements\nUnfilled slider bar color\nInner color of color pickers\nBackground color of disabled toggles",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Primary 4",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(16, rectX + FlarialGUI::SettingsTextWidth("Primary 4 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_primary4")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_primary4_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Primary 4  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_primary4_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Primary 4  ") + tooltipV * 4.125f, rectY,
                            "Base color of color pickers\nHover color of dropdown childrens",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 1",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(17, rectX + FlarialGUI::SettingsTextWidth("Secondary 1 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary1")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary1_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 1  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary1_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 1  ") + tooltipV * 4.125f, rectY,
                            "Background color of settings", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 2",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(18, rectX + FlarialGUI::SettingsTextWidth("Secondary 2 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary2")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary2_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 2  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary2_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 2  ") + tooltipV * 4.125f, rectY,
                            "Background color of navigation bar, tooltips, and buttons",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 3",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(19, rectX + FlarialGUI::SettingsTextWidth("Secondary 3 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary3")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary3_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 3  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary3_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 3  ") + tooltipV * 4.125f, rectY,
                            "Background color of base Click GUI rectangle", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 4",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(20, rectX + FlarialGUI::SettingsTextWidth("Secondary 4 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary4")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary4_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 4  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary4_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 4  ") + tooltipV * 4.125f, rectY,
                            "Background color of active Modules search bar", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 5",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(21, rectX + FlarialGUI::SettingsTextWidth("Secondary 5 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary5")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary5_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 5  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary5_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 5  ") + tooltipV * 4.125f, rectY,
                            "Background color of navigation bar icons", FlarialGUI::SettingsTextWidth("(i)"), tooltipV,
                            true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 6",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(26, rectX + FlarialGUI::SettingsTextWidth("Secondary 6 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary6")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary6_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 6  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary6_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 6  ") + tooltipV * 4.125f, rectY,
                            "Background color of active navigation bar buttons", FlarialGUI::SettingsTextWidth("(i)"),
                            tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"Secondary 7",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(27, rectX + FlarialGUI::SettingsTextWidth("Secondary 7 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_secondary7")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_secondary7_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("Secondary 7  ") + tooltipV * 4.125f,
                                        rectY, L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_secondary7_setting",
                            rectX + FlarialGUI::SettingsTextWidth("Secondary 7  ") + tooltipV * 4.125f, rectY,
                            "Outline color of tooltips", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 1",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(22, rectX + FlarialGUI::SettingsTextWidth("ModCard 1 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod1")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod1_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 1  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard1_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 1  ") + tooltipV * 4.125f, rectY,
                            "Color of top rounded rectangle", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true,
                            true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 2",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(23, rectX + FlarialGUI::SettingsTextWidth("ModCard 2 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod2")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod2_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 2  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard2_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 2  ") + tooltipV * 4.125f, rectY,
                            "Color of bottom rounded rectangle", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true,
                            true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 3",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(24, rectX + FlarialGUI::SettingsTextWidth("ModCard 3 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod3")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod3_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 3  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard3_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 3  ") + tooltipV * 4.125f, rectY,
                            "Background color of module icons\nOuter color of module settings icon",
                            FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true, true);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, L"ModCard 4",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::ColorPicker(25, rectX + FlarialGUI::SettingsTextWidth("ModCard 4 "),
                                rectY - Constraints::SpacingConstraint(0.017, textWidth),
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                        "colors_mod4")->value,
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                                        "colors_mod4_rgb")->value);
        FlarialGUI::FlarialTextWithFont(rectX + FlarialGUI::SettingsTextWidth("ModCard 4  ") + tooltipV * 4.125f, rectY,
                                        L"(i)",
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        FlarialGUI::Tooltip("colors_modcard4_setting",
                            rectX + FlarialGUI::SettingsTextWidth("ModCard 4  ") + tooltipV * 4.125f, rectY,
                            "Inner color of module settings icon", FlarialGUI::SettingsTextWidth("(i)"), tooltipV, true,
                            true);

        // next element

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(10, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_text")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_text")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_text_rgb")->value);
        FlarialGUI::ColorPickerWindow(11, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_enabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_enabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_enabled_rgb")->value);
        FlarialGUI::ColorPickerWindow(12, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_disabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_disabled")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_disabled_rgb")->value);
        FlarialGUI::ColorPickerWindow(13, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary1_rgb")->value);
        FlarialGUI::ColorPickerWindow(14, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary2_rgb")->value);
        FlarialGUI::ColorPickerWindow(15, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary3_rgb")->value);
        FlarialGUI::ColorPickerWindow(16, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_primary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_primary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_primary4_rgb")->value);
        FlarialGUI::ColorPickerWindow(17, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary1_rgb")->value);
        FlarialGUI::ColorPickerWindow(18, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary2_rgb")->value);
        FlarialGUI::ColorPickerWindow(19, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary3_rgb")->value);
        FlarialGUI::ColorPickerWindow(20, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary4_rgb")->value);
        FlarialGUI::ColorPickerWindow(21, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary5")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary5")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary5_rgb")->value);
        FlarialGUI::ColorPickerWindow(26, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary6")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary6")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary6_rgb")->value);
        FlarialGUI::ColorPickerWindow(27, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_secondary7")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_secondary7")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_secondary7_rgb")->value);
        FlarialGUI::ColorPickerWindow(22, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod1")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod1_rgb")->value);
        FlarialGUI::ColorPickerWindow(23, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod2")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod2_rgb")->value);
        FlarialGUI::ColorPickerWindow(24, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod3")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod3_rgb")->value);
        FlarialGUI::ColorPickerWindow(25, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                "colors_mod4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                "o_colors_mod4")->value, ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>(
                "colors_mod4_rgb")->value);
    }

    static inline bool editmenu = false;

    static std::pair<float, float>
    centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
        return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
    }

    void onRender(RenderEvent &event) {
        float allahu = Constraints::RelativeConstraint(0.65);
        float akbar = Constraints::RelativeConstraint(0.25);

        if(editmenu) {
            D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::Black);
            c.a = 0.75f;
            FlarialGUI::RoundedRect(0, 0, c, MC::windowSize.x, MC::windowSize.y, 0, 0);
        }
        Vec2<float> allahuakbar = Constraints::CenterConstraint(allahu, akbar, "y", 1.175, 1.175);
        // TODO: add inventory screen to onRender?
        // watermark
        if (SDK::getCurrentScreen() == "inventory_screen" || SDK::getCurrentScreen().find("chest") != std::string::npos)
            if (Client::settings.getSettingByName<bool>("watermark")->value)
                FlarialGUI::image(IDR_FLARIAL_TITLE_PNG, D2D1::RectF(allahuakbar.x, allahuakbar.y, allahuakbar.x + allahu, allahuakbar.y + akbar));


        if (FlarialGUI::scrollposmodifier == 0) {
            FlarialGUI::scrollposmodifier = Constraints::RelativeConstraint(0.1f);
        }

        if (this->active) {

            FlarialGUI::lerp(baseHeightActual, 0.64f, 0.18f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(realBlurAmount, Client::settings.getSettingByName<float>("blurintensity")->value,
                             0.15f * FlarialGUI::frameFactor);

        } else {

            FlarialGUI::lerp(baseHeightReal, 0.0001f, 0.22f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(baseHeightActual, 0.00001f, 0.30f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);
            FlarialGUI::lerp(realBlurAmount, 0.00001f, 0.15f * FlarialGUI::frameFactor);

            for (auto &box: FlarialGUI::TextBoxes) box.isActive = false;

        }

        if (SwapchainHook::init && baseHeightActual > 0.1f) {

            setEnabled(true);

            /* Base Rectangle Start */

            float baseWidth = Constraints::RelativeConstraint(0.81);

            if (this->active) {

                FlarialGUI::lerp(baseHeightReal, 0.64f, 0.28f * floorf(FlarialGUI::frameFactor * 100.0f) / 100.0f);

            }

            if(Client::settings.getSettingByName<float>("blurintensity")->value > 0.01f)
                Blur::RenderBlur(event.RTV, 3, realBlurAmount / 2);

            float baseHeight = Constraints::RelativeConstraint(baseHeightReal);

            Vec2<float> center = Constraints::CenterConstraint(baseWidth,
                                                               Constraints::RelativeConstraint(baseHeightReal), "r",
                                                               1, 1);
            Vec2<float> round = Constraints::RoundingConstraint(43, 43);
            Vec2<float> baseRound = round;

            D2D1_COLOR_F basebaseRectangleColor = colors_secondary3_rgb ? FlarialGUI::rgbColor : colors_secondary3;
            basebaseRectangleColor.a = o_colors_secondary3;

            FlarialGUI::RoundedRect(center.x, center.y,
                                    basebaseRectangleColor, baseWidth,
                                    Constraints::RelativeConstraint(baseHeightReal), round.x, round.x);


            FlarialGUI::PushSize(center.x, center.y, baseWidth, Constraints::RelativeConstraint(baseHeightActual));

            /* Base Rectangle End */


            /* Nav Bar Start */

            float navigationBarWidth = Constraints::RelativeConstraint(1.235f);
            float navigationBarHeight = Constraints::RelativeConstraint(0.124f);
            float navx = Constraints::PercentageConstraint(0.013f, "left");
            float navy = Constraints::PercentageConstraint(0.019f, "top");
            round = Constraints::RoundingConstraint(28, 28);

            D2D1_COLOR_F navColor = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
            navColor.a = o_colors_secondary2;

            FlarialGUI::RoundedRect(navx, navy, navColor,
                                    navigationBarWidth, navigationBarHeight, round.x, round.x);


            FlarialGUI::PushSize(navx, navy, navigationBarWidth, navigationBarHeight);

            /* Nav Bar End */

            /* Logo Start */


            float logoWidth = Constraints::RelativeConstraint(1.21);

            float logoX = navx - Constraints::SpacingConstraint(0.05, logoWidth);
            float logoY = (navy + navigationBarHeight / 2.0f - logoWidth / 2.0f);

            if (!Client::settings.getSettingByName<bool>("noicons")->value)
                FlarialGUI::image(IDR_LOGO_PNG,
                                  D2D1::RectF(logoX, logoY, logoX + logoWidth, logoY + logoWidth));

            FlarialGUI::Tooltip("easter egg", logoX, logoY, "Never gonna give you up", logoWidth, logoWidth);

            /* Logo End */

            /* tab buttons start */

            float shit = Constraints::RelativeConstraint(0.448f);

            float RadioButtonWidth = Constraints::RelativeConstraint(0.134, "width");
            float RadioButtonHeight = shit;

            if(curr == "modules") FlarialGUI::lerp(width1, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width1, shit, 0.15f * FlarialGUI::frameFactor);

            if(curr == "settings") FlarialGUI::lerp(width2, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width2, shit, 0.15f * FlarialGUI::frameFactor);

            if(curr == "scripting") FlarialGUI::lerp(width3, RadioButtonWidth, 0.15f * FlarialGUI::frameFactor);
            else FlarialGUI::lerp(width3, shit, 0.15f * FlarialGUI::frameFactor);

            float radioX = navx - Constraints::SpacingConstraint(-0.85, logoWidth);
            float radioY(navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            D2D1_COLOR_F modTextCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
            modTextCol.a = o_colors_text;

            D2D1_COLOR_F mod6Col = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            mod6Col.a = o_colors_secondary6;
            static D2D1_COLOR_F tabBgCol = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            static D2D1_COLOR_F tabBgCol2 = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            static D2D1_COLOR_F tabBgCol3 = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;
            static D2D1_COLOR_F tabBgCol4 = colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6;


            tabBgCol.a = o_colors_secondary6;
            tabBgCol2.a = o_colors_secondary6;
            tabBgCol3.a = o_colors_secondary6;
            tabBgCol4.a = o_colors_secondary6;


            //radiobutton of modules

            if(curr != "modules") {
                tabBgCol = FlarialGUI::LerpColor(tabBgCol, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol = FlarialGUI::LerpColor(tabBgCol, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            FlarialGUI::ShadowRect(Vec2(radioX, radioY + Constraints::SpacingConstraint(0.015f, logoWidth)), Vec2{width1, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);

            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(1, radioX, radioY, tabBgCol,
                                                                                        modTextCol, L"Modules",
                                                                                        width1,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "modules", curr)) {
                curr = "modules";
                page.type = "normal";
                FlarialGUI::ResetShit();

                FlarialGUI::scrollpos = 0;
                FlarialGUI::barscrollpos = 0;
                accumilatedPos = 0;
                accumilatedBarPos = 0;
            }


            logoWidth = shit * 0.625f;

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(curr == "modules") FlarialGUI::image(IDR_FOLDER_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
                else FlarialGUI::image(IDR_FOLDER_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
            }

            // radiobutton of settings

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            if(curr != "settings") {
                tabBgCol2 = FlarialGUI::LerpColor(tabBgCol2, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol2 = FlarialGUI::LerpColor(tabBgCol2, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            radioX += Constraints::SpacingConstraint(3.24f, logoWidth);
            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            radioPushAmount1 = Constraints::SpacingConstraint(0.9f, logoWidth) + width1;
            radioX += radioPushAmount1;

            FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)}, Vec2{width2, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);

            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(0, radioX, radioY,
                                                                                        tabBgCol2,
                                                                                        modTextCol, L"Settings",
                                                                                        width2,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "settings",
                                                                                        curr)) {
                FlarialGUI::TextBoxes[0].isActive = false;
                curr = "settings";
            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(curr == "settings") FlarialGUI::image(IDR_SETTINGS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
                else FlarialGUI::image(IDR_SETTINGS_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
            }

            const float h = Constraints::RelativeConstraint(0.42, "height");
            const float allahY = (navy + navigationBarHeight / 2.0f - h / 2.0f);
            ClickGUIElements::SearchBar(0, searchBarString, 12, Constraints::PercentageConstraint(0.022, "right"),
                                        allahY);



            // radiobutton of scripting

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            radioPushAmount2 = Constraints::SpacingConstraint(0.9f * 5.69f, logoWidth) + width1 + width2;
            radioX += radioPushAmount2;
            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            if(curr != "scripting") {
                tabBgCol4 = FlarialGUI::LerpColor(tabBgCol4, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol4 = FlarialGUI::LerpColor(tabBgCol4, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)}, Vec2{width3, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);
            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(3, radioX, radioY,
                                                                                        tabBgCol4,
                                                                                        modTextCol, L"Scripts",
                                                                                        width3,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "scripting",
                                                                                        curr)) {

                curr = "scripting";

            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(curr == "scripting") FlarialGUI::image(IDR_SCRIPTING_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
                else FlarialGUI::image(IDR_SCRIPTING_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));

            }

 // radiobutton of editmenu

            radioX = navx - Constraints::SpacingConstraint(-0.36f, logoWidth);
            radioY = (navy + navigationBarHeight / 2.0f - RadioButtonHeight / 2.0f);

            radioPushAmount2 = Constraints::SpacingConstraint(0.9f * 6.45f, logoWidth) + width1 + width2 + width3;
            radioX += radioPushAmount2;
            round = Constraints::RoundingConstraint(17.5f, 17.5f);

            if(curr != "editmenu") {
                tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, colors_secondary8_rgb ? FlarialGUI::rgbColor : colors_secondary8, 0.15f * FlarialGUI::frameFactor);
            } else {
                tabBgCol3 = FlarialGUI::LerpColor(tabBgCol3, colors_secondary6_rgb ? FlarialGUI::rgbColor : colors_secondary6, 0.15f * FlarialGUI::frameFactor);
            }

            FlarialGUI::ShadowRect(Vec2{radioX, radioY + Constraints::SpacingConstraint(0.115f, logoWidth)}, Vec2{shit, RadioButtonHeight + Constraints::SpacingConstraint(0.015f, logoWidth)}, D2D1::ColorF(D2D1::ColorF::Black), round.x, 3);
            if (!FlarialGUI::activeColorPickerWindows && FlarialGUI::RoundedRadioButton(2, radioX, radioY,
                                                                                        tabBgCol3,
                                                                                        modTextCol, L"",
                                                                                        shit,
                                                                                        RadioButtonHeight, round.x,
                                                                                        round.x, "editmenu",
                                                                                        curr)) {
                this->active = false;
                FlarialGUI::Notify("To disable this menu press ESC or " +
                                           this->settings.getSettingByName<std::string>("editmenubind")->value);
                editmenu = true;
            }

            radioX += Constraints::SpacingConstraint(0.29f, logoWidth);
            radioY += Constraints::SpacingConstraint(0.29f, logoWidth);

            if (!Client::settings.getSettingByName<bool>("noicons")->value) {
                if(curr == "editmenu") FlarialGUI::image(IDR_STYLUS_WHITE_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));
                else FlarialGUI::image(IDR_STYLUS_PNG, D2D1::RectF(radioX, radioY, radioX + logoWidth, radioY + logoWidth));

            }

            /* tab buttons end */

            FlarialGUI::PopSize(); // Pops nav bar
            /* Mod Card Start */

            std::string e = curr;

            if(page.type == "normal" && e == "modules") {
                FlarialGUI::lerp(modcardOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(modcardOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
            } else {
                FlarialGUI::lerp(modcardOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(modcardOffset, -Constraints::RelativeConstraint(2.f, "height", true), 0.08f * FlarialGUI::frameFactor);
            }

            if(page.type == "normal" && e == "scripting") {
                FlarialGUI::lerp(scriptingOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(scriptingOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
            } else {
                FlarialGUI::lerp(scriptingOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(scriptingOffset, -Constraints::RelativeConstraint(4.f, "height", true), 0.08f * FlarialGUI::frameFactor);
            }


            if(e == "settings" || page.type == "settings") {
                FlarialGUI::lerp(settingsOpacity, 1.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(settingsOffset, 0.0f, 0.245f * FlarialGUI::frameFactor);
            } else {
                FlarialGUI::lerp(settingsOpacity, 0.0f, 0.1f * FlarialGUI::frameFactor);
                FlarialGUI::lerp(settingsOffset, Constraints::RelativeConstraint(2.f, "height", true), 0.08f * FlarialGUI::frameFactor);
            }


            if (page.type == "normal") {


                /* MODCARD RENDER */

                if(modcardOpacity > 0.05f){

                    float modWidth = Constraints::RelativeConstraint(0.19f, "height", true);
                    float modHeight = Constraints::RelativeConstraint(0.1369f, "height", true);

                    Vec2<float> modcenter = Constraints::CenterConstraint(modWidth, modHeight, "both", -0.58,
                                                                          -0.52);

                    FlarialGUI::PushSize(center.x, center.y, baseWidth,
                                         Constraints::RelativeConstraint(baseHeightReal, "height", true));

                    float scrollWidth = Constraints::RelativeConstraint(1.12);
                    float scrollHeight = Constraints::RelativeConstraint(0.84);
                    Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0,
                                                                             1);

                    FlarialGUI::PopSize();

                    int i3 = 0;
                    float i2 = 0;

                    for (const auto& pair : ModuleManager::moduleMap) {
                        if ((++i3 % 3) == 0) {
                            i2 += Constraints::SpacingConstraint(0.8, modWidth);
                        }
                    }

                    FlarialGUI::ScrollBar(120, scrollcenter.y, 10, modHeight + i2, 2);

                    FlarialGUI::SetScrollView(scrollcenter.x, scrollcenter.y, scrollWidth, scrollHeight);

                    float xModifier = 0.0f;
                    float yModifier = 0.0f;

                    int i = 0;

                    static auto modules = ModuleManager::getModules();
                    if(ModuleManager::cguiRefresh) {
modules = ModuleManager::getModules();
                        ModuleManager::cguiRefresh = false;
}



                    if (Client::settings.getSettingByName<bool>("enabledModulesOnTop")->value)
                        std::sort(modules.begin(), modules.end(), compareEnabled);
                    else
                        std::sort(modules.begin(), modules.end(), compareNames);

                    for (const auto& pModule: modules) {
                        bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos + 55 > center.y) &&
                                       (modcenter.y + yModifier + FlarialGUI::scrollpos - 200) <
                                       center.y + Constraints::RelativeConstraint(baseHeightReal);

                        if (!searchBarString.empty()) {
                            std::string name = pModule->name;

                            for (char &c: name) {
                                c = (char)std::tolower(c);
                            }

                            std::string search = searchBarString;

                            for (char &c: search) {
                                c = (char)std::tolower(c);
                            }

                            if (name.starts_with(search) ||
                                name.find(search) != std::string::npos) {
                                ClickGUIElements::ModCard(modcenter.x + xModifier + modcardOffset, modcenter.y + yModifier, pModule.get(),
                                                          pModule->icon, i, visible);
                                xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                                if ((++i % 3) == 0) {
                                    yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                    xModifier = 0.0f;
                                }
                            }
                        } else {
                            ClickGUIElements::ModCard(modcenter.x + xModifier + modcardOffset, modcenter.y + yModifier, pModule.get(),
                                                      pModule->icon, i, visible);

                            xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }

                        }
                    }

                    FlarialGUI::UnsetScrollView();

                    D2D1_COLOR_F shadowCol = D2D1::ColorF(D2D1::ColorF::Black);
                    shadowCol.a = modcardOpacity;
                    //FlarialGUI::RoundedRect(center.x, center.y + navigationBarHeight, D2D1::ColorF(D2D1::ColorF::White), baseWidth, navigationBarHeight);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + navigationBarHeight * 1.178f, center.x + baseWidth, center.y + navigationBarHeight * 2.15f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + navigationBarHeight}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::RoundedRect(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, D2D1::ColorF(D2D1::ColorF::White), baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, center.x + baseWidth, center.y + (Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.651f) + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true)}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::ShadowRect(Vec2{center.x, center.y}, Vec2{baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true)}, FlarialGUI::HexToColorF("120e0f"), baseRound.x, 100);
                    }

                /* MODCARD RENDER END */

                /* SETTINGS RENDER */

                if (settingsOpacity > 0.05f) {


                    FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                    float rectXNoOff = Constraints::PercentageConstraint(0.015, "left");
                    float rectX = Constraints::PercentageConstraint(0.015, "left") + settingsOffset;
                    float rectY = Constraints::PercentageConstraint(0.167, "top");
                    float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                    float rectHeight = Constraints::RelativeConstraint(0.85);
                    round = Constraints::RoundingConstraint(38.f, 38.5f);

                    float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
                    float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

                    D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
                    colorThing.a = settingsOpacity;

                    D2D1_RECT_F rect = { rectXNoOff, rectY, rectXNoOff + anotherRectWidth, rectY + anotherRectHeight};
                    FlarialGUI::PushImClipRect(rect);

                    FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                            anotherRectWidth, anotherRectHeight, round.x, round.x);

                    FlarialGUI::PopImClipRect();

                    D2D1_COLOR_F bruv = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                    bruv.a = o_colors_secondary1;

                    D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
                    textCol.a = o_colors_text;

                    FlarialGUI::PopSize();

                    FlarialGUI::PushSize(rectXNoOff + Constraints::SpacingConstraint(0.0085, rectWidth),
                                         rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                         rectHeight);


                    float toggleX = Constraints::PercentageConstraint(0.019, "left");
                    float toggleY = Constraints::PercentageConstraint(0.10, "top");

                    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


                    FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(20.5, scrollviewWidth), 2);
                    FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                              Constraints::RelativeConstraint(1.0, "width"),
                                              Constraints::RelativeConstraint(0.88f, "height"));

                    FlarialGUI::PopSize();

                    FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                     rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                     rectHeight);

                    Module* c = this->ghostMainModule;
                    c->addHeader("Keybinds");
                    c->addKeybind("Eject Keybind", "When setting, hold the new bind for 2 seconds", Client::settings.getSettingByName<std::string>("ejectKeybind")->value);

                    c->extraPadding();

                    c->addHeader("Fonts");
                    c->addTextBox("ClickGUI", "", Client::settings.getSettingByName<std::string>("fontname")->value);
                    c->addSlider("Universal Font Scale", "", Client::settings.getSettingByName<float>("gui_font_scale")->value, 2.f, 0.f, true);
                    c->addTextBox("Modules", "", Client::settings.getSettingByName<std::string>("mod_fontname")->value);
                    c->addSlider("Universal Font Scale", "", Client::settings.getSettingByName<float>("modules_font_scale")->value, 2.f, 0.f, true);
                    c->addToggle("Override Font Weight", "", Client::settings.getSettingByName<bool>("overrideFontWeight")->value);
                    c->addDropdown("Font Weight", "Bold, Thin, etc.", {"Bold", "Normal", "SemiBold", "ExtraBold", "Medium", "Light", "ExtraLight"}, Client::settings.getSettingByName<std::string>("fontWeight")->value);
                    c->extraPadding();

                    c->addHeader("Rendering");
                    c->addButton("Force Reload Minecraft", "Forces the swapchain to recreate.", "RELOAD", [] () {

                        SwapchainHook::queueReset = true;

                    });

                    c->addButton("Reload Scripts", "", "RELOAD", [] () {

    ModuleManager::restartModules = true;

});

                    c->addElementText("Following Requires Restart");
                    c->extraPadding();

                    c->addToggle("Better Frames", "RTX Disabled, Restart Required.", Client::settings.getSettingByName<bool>("killdx")->value);
                    c->addToggle("V-SYNC Disabler", "Works on all devices.", Client::settings.getSettingByName<bool>("vsync")->value);
                    c->addToggle("Do Not Wait Flag", "May improve latency or performance.", Client::settings.getSettingByName<bool>("donotwait")->value);
                    c->addDropdown("Buffering Mode", "May improve latency or performance.", {"Double Buffering", "Triple Buffering"}, Client::settings.getSettingByName<std::string>("bufferingmode")->value);
                    c->addDropdown("Swap Effect", "May improve latency or performance.", {"FLIP_SEQUENTIAL", "FLIP_DISCARD"}, Client::settings.getSettingByName<std::string>("swapeffect")->value);

                    c->extraPadding();

                    c->addElementText("Following Does Not Require Restart");
                    c->extraPadding();

                    c->addToggle("Disable Animations", "",  Client::settings.getSettingByName<bool>("disableanims")->value);
                    c->addSlider("UI Blur Intensity", "", Client::settings.getSettingByName<float>("blurintensity")->value, 25.f);
                    c->addSlider("Chroma / RGB Speed", "", Client::settings.getSettingByName<float>("rgb_speed")->value, 10.f);
                    c->addSlider("Chroma / RGB Saturation", "", Client::settings.getSettingByName<float>("rgb_saturation")->value, 1.f);
                    c->addSlider("Chroma / RGB Value", "", Client::settings.getSettingByName<float>("rgb_value")->value, 1.f);
                    c->addToggle("Inventory Watermark", "", Client::settings.getSettingByName<bool>("watermark")->value);
                    c->addToggle("Enabled Modules on Top", "", Client::settings.getSettingByName<bool>("enabledModulesOnTop")->value);
                    c->addToggle("No UI Icons / Images", "Flarial only", Client::settings.getSettingByName<bool>("noicons")->value);
                    c->addToggle("Disable UI Shadows", "Flarial only",  Client::settings.getSettingByName<bool>("noshadows")->value);

                    c->extraPadding();

                    c->addHeader("Misc");
                    c->addToggle("Center Cursor", "Centers your cursor everytime you open your inventory, etc.", Client::settings.getSettingByName<bool>("centreCursor")->value);
                    c->addToggle("Anonymous on API", "Stay anonymous on Flarial's API.", Client::settings.getSettingByName<bool>("anonymousApi")->value);
                    c->addToggle("Promotions", "To donate & join discord. (pls keep on)", Client::settings.getSettingByName<bool>("promotions")->value);

                    c->resetPadding();

                    FlarialGUI::UnsetScrollView();
                    FlarialGUI::PopSize();

                }

                   if(scriptingOpacity > 0.05f){

                    float modWidth = Constraints::RelativeConstraint(0.19f, "height", true);
                    float modHeight = Constraints::RelativeConstraint(0.1369f, "height", true);

                    Vec2<float> modcenter = Constraints::CenterConstraint(modWidth, modHeight, "both", -0.58,
                                                                          -0.52);

                    FlarialGUI::PushSize(center.x, center.y, baseWidth,
                                         Constraints::RelativeConstraint(baseHeightReal, "height", true));

                    float scrollWidth = Constraints::RelativeConstraint(1.12);
                    float scrollHeight = Constraints::RelativeConstraint(0.84);
                    Vec2<float> scrollcenter = Constraints::CenterConstraint(scrollWidth, scrollHeight, "y", 0.0,
                                                                             1);

                    FlarialGUI::PopSize();

                    int i3 = 0;
                    float i2 = 0;

                    for (const auto& pair : ModuleManager::moduleMap) {
                        if ((++i3 % 3) == 0) {
                            i2 += Constraints::SpacingConstraint(0.8, modWidth);
                        }
                    }

                    FlarialGUI::ScrollBar(120, scrollcenter.y, 10, modHeight + i2, 2);

                    FlarialGUI::SetScrollView(scrollcenter.x, scrollcenter.y, scrollWidth, scrollHeight);

                    float xModifier = 0.0f;
                    float yModifier = 0.0f;

                    int i = 0;

                    static auto modules = Scripting::luaScriptModules;

                    for (const auto& pModule: modules) {
                        bool visible = (modcenter.y + yModifier + FlarialGUI::scrollpos + 55 > center.y) &&
                                       (modcenter.y + yModifier + FlarialGUI::scrollpos - 200) <
                                       center.y + Constraints::RelativeConstraint(baseHeightReal);

                        if (!searchBarString.empty()) {
                            std::string name = pModule.second->name;

                            for (char &c: name) {
                                c = (char)std::tolower(c);
                            }

                            std::string search = searchBarString;

                            for (char &c: search) {
                                c = (char)std::tolower(c);
                            }

                            if (name.starts_with(search) ||
                                name.find(search) != std::string::npos) {
                                ClickGUIElements::ModCard(modcenter.x + xModifier + scriptingOffset, modcenter.y + yModifier, pModule.second.get(),
                                                          pModule.second->icon, i, visible, ClickGUI::scriptingOpacity);
                                xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                                if ((++i % 3) == 0) {
                                    yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                    xModifier = 0.0f;
                                }
                            }
                        } else {
                            ClickGUIElements::ModCard(modcenter.x + xModifier + scriptingOffset, modcenter.y + yModifier, pModule.second.get(),
                                                      pModule.second->icon, i, visible, ClickGUI::scriptingOpacity);

                            xModifier += Constraints::SpacingConstraint(1.02f, modWidth);
                            if ((++i % 3) == 0) {
                                yModifier += Constraints::SpacingConstraint(0.8, modWidth);
                                xModifier = 0.0f;
                            }

                        }
                    }

                    FlarialGUI::UnsetScrollView();

                    D2D1_COLOR_F shadowCol = D2D1::ColorF(D2D1::ColorF::Black);
                    shadowCol.a = scriptingOpacity;
                    //FlarialGUI::RoundedRect(center.x, center.y + navigationBarHeight, D2D1::ColorF(D2D1::ColorF::White), baseWidth, navigationBarHeight);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + navigationBarHeight * 1.178f, center.x + baseWidth, center.y + navigationBarHeight * 2.15f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + navigationBarHeight}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::RoundedRect(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, D2D1::ColorF(D2D1::ColorF::White), baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f);
                    FlarialGUI::PushImClipRect(D2D1::RectF(center.x, center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.85f, center.x + baseWidth, center.y + (Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.651f) + Constraints::RelativeConstraint(baseHeightReal, "height", true) * 0.35f));
                    FlarialGUI::ShadowRect(Vec2{center.x + Constraints::SpacingConstraint(0.15f, baseWidth), center.y + Constraints::RelativeConstraint(baseHeightReal, "height", true)}, Vec2{baseWidth * 0.74f, Constraints::SpacingConstraint(0.25f, baseHeightReal)}, shadowCol, 50, 100);
                    FlarialGUI::PopImClipRect();

                    //FlarialGUI::ShadowRect(Vec2{center.x, center.y}, Vec2{baseWidth, Constraints::RelativeConstraint(baseHeightReal, "height", true)}, FlarialGUI::HexToColorF("120e0f"), baseRound.x, 100);
                    }
                /* SETTINGS RENDER END */
                /* Mod Card End */
            }

            if (ModuleManager::getModule(page.module) && page.type != "normal" && settingsOpacity > 0.05f) {


                FlarialGUI::PushSize(center.x, center.y, baseWidth, baseHeight);

                float rectXNoOff = Constraints::PercentageConstraint(0.015, "left");
                float rectX = Constraints::PercentageConstraint(0.015, "left") + settingsOffset;
                float rectY = Constraints::PercentageConstraint(0.167, "top");
                float rectWidth = Constraints::RelativeConstraint(0.965, "width");
                float rectHeight = Constraints::RelativeConstraint(0.85);
                round = Constraints::RoundingConstraint(38.f, 38.5f);

                float anotherRectHeight = Constraints::RelativeConstraint(0.8105);
                float anotherRectWidth = Constraints::RelativeConstraint(0.972, "width");

                D2D1_COLOR_F colorThing = colors_secondary2_rgb ? FlarialGUI::rgbColor : colors_secondary2;
                colorThing.a = settingsOpacity;

                D2D1_RECT_F rect = { rectXNoOff, rectY, rectXNoOff + anotherRectWidth, rectY + anotherRectHeight};
                FlarialGUI::PushImClipRect(rect);

                FlarialGUI::RoundedRect(rectX, rectY, colorThing,
                                        anotherRectWidth, anotherRectHeight, round.x, round.x);

                FlarialGUI::PopImClipRect();

                D2D1_COLOR_F bruv = colors_secondary1_rgb ? FlarialGUI::rgbColor : colors_secondary1;
                bruv.a = o_colors_secondary1;

                D2D1_COLOR_F textCol = colors_text_rgb ? FlarialGUI::rgbColor : colors_text;
                textCol.a = o_colors_text;

                FlarialGUI::PopSize();

                FlarialGUI::PushSize(rectX + Constraints::SpacingConstraint(0.0085, rectWidth),
                                     rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth,
                                     rectHeight);


                    FlarialGUI::SetScrollView(
                            rectXNoOff + Constraints::SpacingConstraint(0.0085, rectWidth),
                            rectY + Constraints::SpacingConstraint(0.01, rectWidth), rectWidth, rectHeight);


                ModuleManager::getModule(page.module)->settingsRender(settingsOffset);

                FlarialGUI::UnsetScrollView();

                FlarialGUI::PopSize();

                if (!FlarialGUI::activeColorPickerWindows) {

                    float childHeight = Constraints::SpacingConstraint(0.3, rectHeight);
                    float childWidth = Constraints::SpacingConstraint(0.45, rectWidth);
                    std::pair<float, float> centered = centerChildRectangle(rectWidth, rectHeight, childWidth,
                                                                            childHeight);

                    round = Constraints::RoundingConstraint(25, 25);

                    float buttonWidth = Constraints::RelativeConstraint(0.19f, "width");
                    float buttonHeight = Constraints::RelativeConstraint(0.1f, "height");
                    float spacingX = Constraints::RelativeConstraint(0.03);

                    std::pair<float, float> thingYes = centerChildRectangle(childWidth, childHeight, buttonWidth,
                                                                            buttonHeight);


                    textCol.a = settingsOpacity;
                    if (FlarialGUI::RoundedButton(0, spacingX + centered.first + rectX - settingsOffset,
                                                  thingYes.second + rectHeight + rectY - Constraints::RelativeConstraint(0.06f), colorThing, textCol, L"Reset",
                                                  buttonWidth, buttonHeight, round.x, round.x)) {
                        auto currentModule = ModuleManager::getModule(page.module);
                        bool wasEnabled = currentModule->isEnabled();
                        currentModule->enabledState = false; // THIS MIGHT BE WITH DELAY !!!
                        currentModule->loadDefaults();
                        FlarialGUI::ResetShit();
                        if(wasEnabled)
                            currentModule->enabledState = true;
                    }


                    if (FlarialGUI::RoundedButton(1, -spacingX + centered.first + rectX + childWidth - buttonWidth - settingsOffset,
                                                  thingYes.second + rectHeight + rectY - Constraints::RelativeConstraint(0.06f), colorThing, textCol,
                                                  L"Copy From", buttonWidth, buttonHeight, round.x, round.x)) {

                    }
                }
            }
            FlarialGUI::PopSize(); // Pops base rect
        }

        if(realBlurAmount < 0.5f) setEnabled(false);

        FlarialGUI::displayToolTips();
        FlarialGUI::NotifyHeartbeat();
    }

    void onKey(KeyEvent &event) {

        //TODO: MAKE module->setActive() module->isActive() module->isRestricted()

        if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key) && event.getAction() == ActionType::Pressed) {
            if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen")
                this->active = false;
            else {
            keybindActions[0]({});
            }

            if (this->active) {
                accumilatedPos = 0;
                accumilatedBarPos = 0;

                page.type = "normal";
                curr = "modules";
            }
            else {
                FlarialGUI::ResetShit();
                ModuleManager::SaveModulesConfig();
                Client::SaveSettings();
            }
        }

        // if clicked esc
        if ((event.getKey() == VK_ESCAPE && event.getAction() == ActionType::Released)) {
            if (!editmenu) {
                if (this->active) {
                    // exit ClickGUI
                    SDK::clientInstance->grabMouse(); // let mouse control the view

                    this->active = false;
                    page.type = "normal";
                    curr = "modules";

                    FlarialGUI::ResetShit();
                    ModuleManager::SaveModulesConfig();
                    Client::SaveSettings();
                }
            } else {
                // switch from edit mode back to ClickGUI
                editmenu = false;
                this->active = true;
            }
        }

        if (this->active) {
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

            // auto search? TODO: make it optional
            if (page.type == "normal" && curr == "modules" &&
                event.getAction() == ActionType::Pressed) {

                //FlarialGUI::TextBoxes[0].isActive = true;
                // you searching
                if (FlarialGUI::TextBoxes[0].isActive) {
                    FlarialGUI::scrollpos = 0;
                    FlarialGUI::barscrollpos = 0;
                    accumilatedPos = 0;
                    accumilatedBarPos = 0;

                }
            }

            if (this->isAdditionalKeybind(event.keys,
                                          this->settings.getSettingByName<std::string>("editmenubind")->value) &&
                Module::isKeyPartOfAdditionalKeybind(event.key, this->settings.getSettingByName<std::string>(
                        "editmenubind")->value)) {

                if (!editmenu) {
                    this->active = false;
                    FlarialGUI::Notify("To disable this menu press ESC or " +
                                               this->settings.getSettingByName<std::string>("editmenubind")->value);
                    editmenu = true;
                }

            }
            // switch back to ClickGUI if clicked on Edit Mode bind
        } else if (editmenu && this->isAdditionalKeybind(event.keys, this->settings.getSettingByName<std::string>(
                "editmenubind")->value) && Module::isKeyPartOfAdditionalKeybind(event.key,
                                                                                this->settings.getSettingByName<std::string>(
                                                                                        "editmenubind")->value) ||
                   editmenu && this->isKeybind(event.keys) && Module::isKeyPartOfAdditionalKeybind(event.key,
                                                                                                     this->settings.getSettingByName<std::string>(
                                                                                                             "editmenubind")->value)) {

            editmenu = false;
            this->active = true;

        }

        if (this->active || editmenu && SDK::getCurrentScreen() == "hud_screen")
            SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

        if (this->active || editmenu)
            event.cancel(); // do not pass key event to the game

        FlarialGUI::inMenu = this->active;
    }

    void onMouse(MouseEvent &event) {
        MC::mousePos.x = event.getMouseX();
        MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();

        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) MC::held = true;
        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) MC::held = false;

        if (event.getButton() == MouseButton::Scroll) {
            accumilatedPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::scrollposmodifier
                                                                           : -FlarialGUI::scrollposmodifier;
            accumilatedBarPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::barscrollposmodifier
                                                                              : -FlarialGUI::barscrollposmodifier;
        }

        if (this->active) event.cancel();

        if(SDK::getCurrentScreen() != "hud_screen"){
            if(this->active)
                this->active = false;
        }

        if ((this->active || editmenu) && SDK::getCurrentScreen() == "hud_screen")
            event.cancel(); // TODO: modules dont listen for canceled state!!!

    }

    static bool compareEnabled(std::shared_ptr<Module>& obj1, std::shared_ptr<Module>& obj2) {
        return obj1->isEnabled() >
               obj2->isEnabled();
    }

    static bool compareNames(std::shared_ptr<Module>& obj1, std::shared_ptr<Module>& obj2) {
        return obj1->name < obj2->name;
    }
};