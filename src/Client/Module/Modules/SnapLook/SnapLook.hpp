#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SnapLookListener.hpp"

class SnapLook : public Module {

public:

    SnapLook() : Module("SnapLook", "Quickly look behind you.", "\\Flarial\\assets\\eye.png", 'V') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new SnapListener("Snap", this));

        Module::onEnable();

    }

    void NormalRender(int index, std::string text, std::string value) override {

    }

    void DefaultConfig() override {

        if (settings.getSettingByName<std::string>("keybind") == nullptr) settings.addSetting("keybind", (std::string)"V");

    }

    void onDisable() override {
        Module::onDisable();
    }

    void SettingsRender() override {

        float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        FlarialGUI::KeybindSelector(0, toggleX, toggleY, settings.getSettingByName<std::string>("keybind")->value);
    }
};