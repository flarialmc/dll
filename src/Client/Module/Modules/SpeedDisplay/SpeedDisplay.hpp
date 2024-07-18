#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SpeedDisplayListener.hpp"
#include "../../../Client.hpp"

class SpeedDisplay : public Module {

public:


    SpeedDisplay() : Module("Speed Display", "Displays your current travel speed in blocks/second.",
                            IDR_SPEED_PNG, "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new SpeedDisplayListener("SpeedDisplayListener", this));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("SpeedDisplayListener");
        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{value} m/s");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 1.00f);
    }

    void settingsRender() override { }
};

