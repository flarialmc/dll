#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "ClickGUIRenderer.hpp"
#include "GUIMouseListener.hpp"

class ClickGUI : public Module {

public:

    ClickGUI() : Module("ClickGUI", "What do you think it is?", "\\Flarial\\assets\\clickgui.png", 'K') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        EventHandler::registerListener(new ClickGUIRenderer("ClickGUI", this));
        EventHandler::registerListener(new GUIMouseListener("GUIMouse"));

        if(settings.getSettingByName<std::string>("keybind")->value.empty()) settings.getSettingByName<std::string>("keybind")->value = "K";

        if(settings.getSettingByName<std::string>("editmenubind") == nullptr) settings.addSetting("editmenubind", (std::string)"L");

        settings.getSettingByName<bool>("enabled")->value = false;

    }

    void onDisable() override {

        this->settings.getSettingByName<bool>("enabled")->value = false;

        Module::onDisable();

        EventHandler::unregisterListener("ClickGUI");
        EventHandler::unregisterListener("GUIMouse");

    }

    void SettingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(7.5, textWidth), 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::KeybindSelector(0, x, y, settings.getSettingByName<std::string>("keybind")->value);

        if(settings.getSettingByName<std::string>("keybind")->value.empty()) settings.getSettingByName<std::string>("keybind")->value = "K";

        y += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(x, y, L"Edit Menu", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);

        FlarialGUI::KeybindSelector(1, x + Constraints::SpacingConstraint(0.8, textWidth), y, settings.getSettingByName<std::string>("editmenubind")->value);

        if(settings.getSettingByName<std::string>("editmenubind")->value.empty()) settings.getSettingByName<std::string>("editmenubind")->value = "L";

        float rectY = Constraints::PercentageConstraint(0.35, "top");
        float rectX = x;

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Text Color").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(10, rectX + FlarialGUI::SettingsTextWidth("Text Color "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_text")->value, Client::settings.getSettingByName<float>("o_colors_text")->value, Client::settings.getSettingByName<bool>("colors_text_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Enabled Color").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(11, rectX + FlarialGUI::SettingsTextWidth("Enabled Color "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_enabled")->value, Client::settings.getSettingByName<float>("o_colors_enabled")->value, Client::settings.getSettingByName<bool>("colors_enabled_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Disabled Color").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(12, rectX + FlarialGUI::SettingsTextWidth("Disabled Color "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_disabled")->value, Client::settings.getSettingByName<float>("o_colors_disabled")->value, Client::settings.getSettingByName<bool>("colors_disabled_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Primary 1").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(13, rectX + FlarialGUI::SettingsTextWidth("Primary 1 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_primary1")->value, Client::settings.getSettingByName<float>("o_colors_primary1")->value, Client::settings.getSettingByName<bool>("colors_primary1_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Primary 2").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(14, rectX + FlarialGUI::SettingsTextWidth("Primary 2 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_primary2")->value, Client::settings.getSettingByName<float>("o_colors_primary2")->value, Client::settings.getSettingByName<bool>("colors_primary2_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Primary 3").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(15, rectX + FlarialGUI::SettingsTextWidth("Primary 3 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_primary3")->value, Client::settings.getSettingByName<float>("o_colors_primary3")->value, Client::settings.getSettingByName<bool>("colors_primary3_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Primary 4").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(16, rectX + FlarialGUI::SettingsTextWidth("Primary 4 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_primary4")->value, Client::settings.getSettingByName<float>("o_colors_primary4")->value, Client::settings.getSettingByName<bool>("colors_primary4_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Secondary 1").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(17, rectX + FlarialGUI::SettingsTextWidth("Secondary 1 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_secondary1")->value, Client::settings.getSettingByName<float>("o_colors_secondary1")->value, Client::settings.getSettingByName<bool>("colors_secondary1_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Secondary 2").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(18, rectX + FlarialGUI::SettingsTextWidth("Secondary 2 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_secondary2")->value, Client::settings.getSettingByName<float>("o_colors_secondary2")->value, Client::settings.getSettingByName<bool>("colors_secondary2_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Secondary 3").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(19, rectX + FlarialGUI::SettingsTextWidth("Secondary 3 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_secondary3")->value, Client::settings.getSettingByName<float>("o_colors_secondary3")->value, Client::settings.getSettingByName<bool>("colors_secondary3_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Secondary 4").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(20, rectX + FlarialGUI::SettingsTextWidth("Secondary 4 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_secondary4")->value, Client::settings.getSettingByName<float>("o_colors_secondary4")->value, Client::settings.getSettingByName<bool>("colors_secondary4_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Secondary 5").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(21, rectX + FlarialGUI::SettingsTextWidth("Secondary 5 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_secondary5")->value, Client::settings.getSettingByName<float>("o_colors_secondary5")->value, Client::settings.getSettingByName<bool>("colors_secondary5_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("Secondary 6").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(26, rectX + FlarialGUI::SettingsTextWidth("Secondary 6 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_secondary6")->value, Client::settings.getSettingByName<float>("o_colors_secondary6")->value, Client::settings.getSettingByName<bool>("colors_secondary6_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("ModCard 1").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(22, rectX + FlarialGUI::SettingsTextWidth("ModCard 1 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_mod1")->value, Client::settings.getSettingByName<float>("o_colors_mod1")->value, Client::settings.getSettingByName<bool>("colors_mod1_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("ModCard 2").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(23, rectX + FlarialGUI::SettingsTextWidth("ModCard 2 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_mod2")->value, Client::settings.getSettingByName<float>("o_colors_mod2")->value, Client::settings.getSettingByName<bool>("colors_mod2_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("ModCard 3").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(24, rectX + FlarialGUI::SettingsTextWidth("ModCard 3 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_mod3")->value, Client::settings.getSettingByName<float>("o_colors_mod3")->value, Client::settings.getSettingByName<bool>("colors_mod3_rgb")->value);

        rectY += Constraints::SpacingConstraint(0.35, textWidth);
        FlarialGUI::FlarialTextWithFont(rectX, rectY, FlarialGUI::to_wide("ModCard 4").c_str(),
                                        textWidth * 6.9f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_EXTRA_LIGHT);
        FlarialGUI::ColorPicker(25, rectX + FlarialGUI::SettingsTextWidth("ModCard 4 "), rectY - Constraints::SpacingConstraint(0.017, textWidth), Client::settings.getSettingByName<std::string>("colors_mod4")->value, Client::settings.getSettingByName<float>("o_colors_mod4")->value, Client::settings.getSettingByName<bool>("colors_mod4_rgb")->value);


        // next element

        FlarialGUI::UnsetScrollView();

        FlarialGUI::ColorPickerWindow(10, Client::settings.getSettingByName<std::string>("colors_text")->value, Client::settings.getSettingByName<float>("o_colors_text")->value, Client::settings.getSettingByName<bool>("colors_text_rgb")->value);
        FlarialGUI::ColorPickerWindow(11, Client::settings.getSettingByName<std::string>("colors_enabled")->value, Client::settings.getSettingByName<float>("o_colors_enabled")->value, Client::settings.getSettingByName<bool>("colors_enabled_rgb")->value);
        FlarialGUI::ColorPickerWindow(12, Client::settings.getSettingByName<std::string>("colors_disabled")->value, Client::settings.getSettingByName<float>("o_colors_disabled")->value, Client::settings.getSettingByName<bool>("colors_disabled_rgb")->value);
        FlarialGUI::ColorPickerWindow(13, Client::settings.getSettingByName<std::string>("colors_primary1")->value, Client::settings.getSettingByName<float>("o_colors_primary1")->value, Client::settings.getSettingByName<bool>("colors_primary1_rgb")->value);
        FlarialGUI::ColorPickerWindow(14, Client::settings.getSettingByName<std::string>("colors_primary2")->value, Client::settings.getSettingByName<float>("o_colors_primary2")->value, Client::settings.getSettingByName<bool>("colors_primary2_rgb")->value);
        FlarialGUI::ColorPickerWindow(15, Client::settings.getSettingByName<std::string>("colors_primary3")->value, Client::settings.getSettingByName<float>("o_colors_primary3")->value, Client::settings.getSettingByName<bool>("colors_primary3_rgb")->value);
        FlarialGUI::ColorPickerWindow(16, Client::settings.getSettingByName<std::string>("colors_primary4")->value, Client::settings.getSettingByName<float>("o_colors_primary4")->value, Client::settings.getSettingByName<bool>("colors_primary4_rgb")->value);
        FlarialGUI::ColorPickerWindow(17, Client::settings.getSettingByName<std::string>("colors_secondary1")->value, Client::settings.getSettingByName<float>("o_colors_secondary1")->value, Client::settings.getSettingByName<bool>("colors_secondary1_rgb")->value);
        FlarialGUI::ColorPickerWindow(18, Client::settings.getSettingByName<std::string>("colors_secondary2")->value, Client::settings.getSettingByName<float>("o_colors_secondary2")->value, Client::settings.getSettingByName<bool>("colors_secondary2_rgb")->value);
        FlarialGUI::ColorPickerWindow(19, Client::settings.getSettingByName<std::string>("colors_secondary3")->value, Client::settings.getSettingByName<float>("o_colors_secondary3")->value, Client::settings.getSettingByName<bool>("colors_secondary3_rgb")->value);
        FlarialGUI::ColorPickerWindow(20, Client::settings.getSettingByName<std::string>("colors_secondary4")->value, Client::settings.getSettingByName<float>("o_colors_secondary4")->value, Client::settings.getSettingByName<bool>("colors_secondary4_rgb")->value);
        FlarialGUI::ColorPickerWindow(21, Client::settings.getSettingByName<std::string>("colors_secondary5")->value, Client::settings.getSettingByName<float>("o_colors_secondary5")->value, Client::settings.getSettingByName<bool>("colors_secondary5_rgb")->value);
        FlarialGUI::ColorPickerWindow(26, Client::settings.getSettingByName<std::string>("colors_secondary6")->value, Client::settings.getSettingByName<float>("o_colors_secondary6")->value, Client::settings.getSettingByName<bool>("colors_secondary6_rgb")->value);
        FlarialGUI::ColorPickerWindow(22, Client::settings.getSettingByName<std::string>("colors_mod1")->value, Client::settings.getSettingByName<float>("o_colors_mod1")->value, Client::settings.getSettingByName<bool>("colors_mod1_rgb")->value);
        FlarialGUI::ColorPickerWindow(23, Client::settings.getSettingByName<std::string>("colors_mod2")->value, Client::settings.getSettingByName<float>("o_colors_mod2")->value, Client::settings.getSettingByName<bool>("colors_mod2_rgb")->value);
        FlarialGUI::ColorPickerWindow(24, Client::settings.getSettingByName<std::string>("colors_mod3")->value, Client::settings.getSettingByName<float>("o_colors_mod3")->value, Client::settings.getSettingByName<bool>("colors_mod3_rgb")->value);
        FlarialGUI::ColorPickerWindow(25, Client::settings.getSettingByName<std::string>("colors_mod4")->value, Client::settings.getSettingByName<float>("o_colors_mod4")->value, Client::settings.getSettingByName<bool>("colors_mod4_rgb")->value);
    }
};