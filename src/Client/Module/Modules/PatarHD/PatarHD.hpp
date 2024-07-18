#pragma once

#include "../Module.hpp"
#include "PatarHDListener.hpp"
#include "../../../Events/EventHandler.hpp"


class PatarHD : public Module {

public:

    PatarHD() : Module("PatarHD", "Who is this now?", IDR_SKULL_PNG, "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new PatarHDListener("PatarHDListener", this));
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("PatarHDListener");
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<bool>("dvdmode") == nullptr)
            settings.addSetting("dvdmode", true);
        if (settings.getSettingByName<float>("xveloc") == nullptr)
            settings.addSetting("xveloc", 1.0f);
        if (settings.getSettingByName<float>("yveloc") == nullptr)
            settings.addSetting("yveloc", 0.69f);
        if (settings.getSettingByName<float>("scale") == nullptr)
            settings.addSetting("scale", 1.0f);
    }

    void settingsRender() override { }
};