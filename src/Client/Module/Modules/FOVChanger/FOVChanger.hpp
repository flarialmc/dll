#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Client.hpp"
#include "FOVChangerListener.hpp"

class FOVChanger : public Module {

public:
    FOVChanger() : Module("FOV Changer", "Change your FOV beyond Minecraft's limits.",
                          IDR_FIELD_OF_VIEW_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerOrderedPriorityListener(new FOVChangerListener("FOVChange", this),1);
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("FOVChange");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("fovvalue") == nullptr) settings.addSetting("fovvalue", 60.00f);
        if (settings.getSettingByName<float>("fovaffectshand") == nullptr) settings.addSetting("fovaffectshand", false);

    }

    void settingsRender() override { }
};

