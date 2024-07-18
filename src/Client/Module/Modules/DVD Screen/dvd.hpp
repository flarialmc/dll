#pragma once

#include "../Module.hpp"
#include "dvdListener.hpp"
#include "../../../Events/EventHandler.hpp"


class DVD : public Module {

public:

    DVD() : Module("DVD Screen", "Overlays the DVD Screensaver", IDR_SKULL_PNG, "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new dvdListener("dvdListener", this));

        Module::onEnable();

    }

    void onDisable() override {

        EventHandler::unregisterListener("dvdListener");

        Module::onDisable();

    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("xveloc") == nullptr)
            settings.addSetting("xveloc", 1.0f);
        if (settings.getSettingByName<float>("yveloc") == nullptr)
            settings.addSetting("yveloc", 0.69f);
        if (settings.getSettingByName<float>("scale") == nullptr)
            settings.addSetting("scale", 1.0f);

    }

    void settingsRender() override { }
};