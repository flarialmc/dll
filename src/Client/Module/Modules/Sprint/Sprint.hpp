#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SprintListener.hpp"


class Sprint : public Module {

public:


    Sprint() : Module("Auto Sprint", "Automatically sprints for you!!!", "\\Flarial\\assets\\memory.png", 'M') {

        onEnable();

    };

    void onEnable() override {

        Module::onEnable();

        if (settings.getSettingByName<bool>("toggled") == nullptr) {
            settings.addSetting("toggled", false);
        }

        if (settings.getSettingByName<bool>("always") == nullptr) {
            settings.addSetting("always", false);
        }

        EventHandler::registerListener(new SprintListener("Sprint", this));
    }

    void onDisable() override {

        EventHandler::unregisterListener("Sprint");

        Module::onDisable();

    }

    void SettingsRender() override {
        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");
        if (FlarialGUI::Toggle(0, toggleX, toggleY, D2D1::ColorF(255.0f / 255.0f, 35.0f / 255.0f, 58.0f / 255.0f), D2D1::ColorF(112.0f / 255.0f, 75.0f / 255.0f, 82.0f / 255.0f), D2D1::ColorF(D2D1::ColorF::White), this->settings.getSettingByName<bool>("always")->value)) this->settings.getSettingByName<bool>("always")->value = !this->settings.getSettingByName<bool>("always")->value;

    }
};

