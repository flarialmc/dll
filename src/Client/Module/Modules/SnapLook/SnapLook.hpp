#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SnapLookListener.hpp"

class SnapLook : public Module {

public:

    SnapLook() : Module("SnapLook", "snap", "\\Flarial\\assets\\eye.png", 'V') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new SnapListener("Snap", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    void DefaultConfig() override {
        if (settings.getSettingByName<std::string>("keybind")->value == (std::string)"") settings.getSettingByName<std::string>("keybind")->value = "V";

    }

    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, textWidth), 2);
        FlarialGUI::SetScrollView(x, y, Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.90, "height"));

        FlarialGUI::KeybindSelector(0, x, y, settings.getSettingByName<std::string>("keybind")->value);


        FlarialGUI::UnsetScrollView();

    }
};