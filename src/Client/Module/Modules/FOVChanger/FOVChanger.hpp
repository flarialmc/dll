#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Client.hpp"
#include "FOVChangerListener.hpp"

class FOVChanger : public Module {

public:
    FOVChanger() : Module("FOV Changer", "Change your FOV beyond Minecraft's limits.",
                          R"(\Flarial\assets\field-of-view.png)", "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerOrderedPriorityListener(new FOVChangerListener("FOVChange", this),1);
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("FOVChange");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("fovvalue") == nullptr) settings.addSetting("fovvalue", 60.00f);
        if (settings.getSettingByName<float>("fovaffectshand") == nullptr) settings.addSetting("fovaffectshand", false);

    }

    void settingsRender() override {

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::ScrollBar(toggleX, toggleY, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(toggleX, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(1.0f, "height"));

        FlarialGUI::FlarialTextWithFont(toggleX, toggleY, L"FOV Value", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        float percent = FlarialGUI::Slider(0, toggleX + FlarialGUI::SettingsTextWidth("FOV Value "),
                                           toggleY, this->settings.getSettingByName<float>("fovvalue")->value, 359.0f,
                                           0, false);

        this->settings.getSettingByName<float>("fovvalue")->value = percent;

        toggleY += Constraints::SpacingConstraint(0.25, textWidth);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Affect hand size", textWidth * 3.0f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);
        if (FlarialGUI::Toggle(1, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "fovaffectshand")->value))
            this->settings.getSettingByName<bool>("fovaffectshand")->value = !this->settings.getSettingByName<bool>(
                    "fovaffectshand")->value;

        FlarialGUI::UnsetScrollView();
    }
};

