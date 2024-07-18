#pragma once

#include "HueListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class HueChanger : public Module {

public:

    HueChanger() : Module("Saturation", "A filter to saturate or\ndesaturate Minecraft.",
                          IDR_FULLBRIGHT_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerPriorityListener(new HueListener("Hue", this));
        Module::onEnable();
    }


    void onDisable() override {
        EventHandler::unregisterListener("Hue");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.5f);
    }

    void settingsRender() override { }
};