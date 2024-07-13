#pragma once

#include "DeepfryListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Deepfry : public Module {

public:
    Deepfry() : Module("Deepfry", "Theres only one way to find out.", IDR_FRYING_PAN_PNG, "") {

        Module::setup();
    };

    void onEnable() override {

        EventHandler::registerPriorityListener(new DeepfryListener("Deepfry", this));

        Module::onEnable();

    }

    void onDisable() override {

        EventHandler::unregisterListener("Deepfry");

        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("paint") == nullptr) settings.addSetting("paint", false);
    }

    void settingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Paint Effect", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(1, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "paint")->value))
            this->settings.getSettingByName<bool>("paint")->value = !this->settings.getSettingByName<bool>(
                    "paint")->value;


    }
};