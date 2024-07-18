#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FPSListener.hpp"

class FPSCounter : public Module {

public:

    FPSCounter() : Module("FPS", "Shows how much Frames Per Second (FPS)\nyour device is rendering.",
                          IDR_FPS_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new FPSListener("FPS", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("FPS");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "FPS: {value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
        if (settings.getSettingByName<float>("fpsSpoofer") == nullptr) settings.addSetting("fpsSpoofer", 1.0f);

    }

    void settingsRender() override { }
};