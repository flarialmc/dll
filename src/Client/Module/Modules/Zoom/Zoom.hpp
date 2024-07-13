#pragma once

#include "../Module.hpp"
#include "ZoomListener.hpp"
#include "../../../Events/EventHandler.hpp"


class Zoom : public Module {

public:

    Zoom() : Module("Zoom", "Allows you to see distant places.", IDR_MAGNIFY_PNG, "C") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerOrderedPriorityListener(new ZoomListener("Zoom", this), 2);
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("Zoom");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("alwaysanim") == nullptr) settings.addSetting("alwaysanim", false);

        if (settings.getSettingByName<bool>("SaveModifier") == nullptr) settings.addSetting("SaveModifier", true);
        if (settings.getSettingByName<bool>("hidehand") == nullptr) settings.addSetting("hidehand", true);
        if (settings.getSettingByName<bool>("hidemodules") == nullptr) settings.addSetting("hidemodules", false);
        //if (settings.getSettingByName<bool>("hidehud") == nullptr) settings.addSetting("hidehud", false);
        if (settings.getSettingByName<float>("modifier") == nullptr) settings.addSetting("modifier", 10.0f);
        if (settings.getSettingByName<float>("anim") == nullptr) settings.addSetting("anim", 0.20f);
        if (settings.getSettingByName<float>("disableanim") == nullptr) settings.addSetting("disableanim", false);
    }

    void settingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::KeybindSelector(0, toggleX, toggleY, getKeybind());

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Modifier", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);


        float percent = FlarialGUI::Slider(7, toggleX + FlarialGUI::SettingsTextWidth("Modifier "),
                                           toggleY, this->settings.getSettingByName<float>("modifier")->value, 30, 0,
                                           false);

        this->settings.getSettingByName<float>("modifier")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.35, textWidth);

        textWidth *= 1.5f;

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"Animation Speed", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);


        percent = FlarialGUI::Slider(8, toggleX + FlarialGUI::SettingsTextWidth("Animation Speed "),
                                     toggleY, this->settings.getSettingByName<float>("anim")->value, 0.40);

        this->settings.getSettingByName<float>("anim")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.25, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.38, textWidth), toggleY,
                                        L"Save Modifier", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(1, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "SaveModifier")->value))
            this->settings.getSettingByName<bool>("SaveModifier")->value = !this->settings.getSettingByName<bool>(
                    "SaveModifier")->value;

        toggleX = Constraints::PercentageConstraint(0.55, "left");
        toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Hide hand", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(2, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "hidehand")->value))
            this->settings.getSettingByName<bool>("hidehand")->value = !this->settings.getSettingByName<bool>(
                    "hidehand")->value;

//        toggleY += Constraints::SpacingConstraint(0.25, textWidth);
//
//        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
//                                        L"Hide hud", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
//                                        Constraints::RelativeConstraint(0.12, "height", true),
//                                        DWRITE_FONT_WEIGHT_NORMAL);
//        if (FlarialGUI::Toggle(3, toggleX, toggleY, this->settings.getSettingByName<bool>(
//                "hidehud")->value))
//            this->settings.getSettingByName<bool>("hidehud")->value = !this->settings.getSettingByName<bool>(
//                    "hidehud")->value;

        toggleY += Constraints::SpacingConstraint(0.25, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Hide modules", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(4, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "hidemodules")->value))
            this->settings.getSettingByName<bool>("hidemodules")->value = !this->settings.getSettingByName<bool>(
                    "hidemodules")->value;

        toggleY += Constraints::SpacingConstraint(0.25, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Disable animation", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(5, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "disableanim")->value))
            this->settings.getSettingByName<bool>("disableanim")->value = !this->settings.getSettingByName<bool>(
                    "disableanim")->value;

        toggleY += Constraints::SpacingConstraint(0.25, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Always animate", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(6, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "alwaysanim")->value))
            this->settings.getSettingByName<bool>("alwaysanim")->value = !this->settings.getSettingByName<bool>(
                    "alwaysanim")->value;

        FlarialGUI::UnsetScrollView();

    }
};