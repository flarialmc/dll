#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AnimationsListener.hpp"


class Animations : public Module {

public:

    Animations() : Module("Animations", "Animate your selected slot square\nwhile you switch slots.",
                          IDR_ANIMATIONS_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new AnimationsListener("Animations", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("Animations");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("hotbarSpeed") == nullptr) settings.addSetting("hotbarSpeed", 7.0f);

    }

    void settingsRender() override { }
};
