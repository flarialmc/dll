#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ClickGUIRenderer.hpp"
#include "GUIMouseListener.hpp"

class ClickGUI : public Module {

public:

    ClickGUI() : Module("ClickGUI", "What do you think it is?", R"(\Flarial\assets\clickgui.png)", "K") {
        Module::setup();
    };

    void onSetup() override {
        EventHandler::registerListener(new ClickGUIRenderer("ClickGUI", this));
        EventHandler::registerListener(new GUIMouseListener("GUIMouse", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("ClickGUI");
        EventHandler::unregisterListener("GUIMouse");
        Module::onDisable();
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

    void settingsRender() override {

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

        FlarialGUI::FlarialTextWithFont(x, y, L"Edit Menu", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_text")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_enabled")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_disabled")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_primary1")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_primary2")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_primary3")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_primary4")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_secondary1")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_secondary2")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_secondary3")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_secondary4")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_secondary5")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_secondary6")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_secondary7")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_mod1")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_mod2")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_mod3")->value,
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
                                ModuleManager::getModule("ClickGUI")->settings.getSettingByName<float>(
                                        "o_colors_mod4")->value,
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
};