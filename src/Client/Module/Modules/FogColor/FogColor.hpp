#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FogColorListener.hpp"


class FogColor : public Module {

public:

    FogColor() : Module("Fog Color", "Changes the color of the\nMinecraft fog.", IDR_SMOKE_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new FogColorListener("FogColor", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("FogColor");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("color") == nullptr)
            settings.addSetting("color", (std::string) "FFFFFF");
        if (settings.getSettingByName<bool>("color_rgb") == nullptr) settings.addSetting("color_rgb", false);
        if (settings.getSettingByName<float>("colorOpacity") == nullptr) settings.addSetting("colorOpacity", 0.6f);

    }

    void settingsRender() override { }
};