#pragma once

#include "DeepfryListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Deepfry : public Module {

public:

    void NormalRender(int index, std::string text, std::string value) override {

    }

    Deepfry() : Module("Deepfry", "Theres only one way to find out.", "\\Flarial\\assets\\frying-pan.png", 'b') {

        onEnable();

    };

    void onEnable() override {

        if (settings.getSettingByName<bool>("paint") == nullptr) settings.addSetting("paint", false);

        EventHandler::registerListener(new DeepfryListener("Deepfry", this));
        Module::onEnable();
    }


    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Paint Effect", textWidth * 3.0f, textHeight, DWRITE_TEXT_ALIGNMENT_LEADING,
                                        Constraints::RelativeConstraint(0.12, "height", true),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(1, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "paint")->value)) this->settings.getSettingByName<bool>("paint")->value = !this->settings.getSettingByName<bool>("paint")->value;


    }
};