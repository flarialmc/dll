#pragma once

#include "../Module.hpp"
#include "ZoomListener.hpp"
#include "../../../Events/EventHandler.hpp"


class Zoom : public Module {

public:

    Zoom() : Module("Zoom", "Allows you to see distant places.", IDR_MAGNIFY_PNG, "C") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerOrderedPriorityListener(new ZoomListener("Zoom", this), 2);
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("Zoom");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("alwaysanim") == nullptr) settings.addSetting("alwaysanim", false);

        if (settings.getSettingByName<bool>("SaveModifier") == nullptr) settings.addSetting("SaveModifier", true);
        if (settings.getSettingByName<bool>("hidehand") == nullptr) settings.addSetting("hidehand", true);
        if (settings.getSettingByName<bool>("hidemodules") == nullptr) settings.addSetting("hidemodules", false);
        //if (settings.getSettingByName<bool>("hidehud") == nullptr) settings.addSetting("hidehud", false);
        if (settings.getSettingByName<float>("modifier") == nullptr) settings.addSetting("modifier", 10.0f);
        if (settings.getSettingByName<float>("anim") == nullptr) settings.addSetting("anim", 0.20f);
        if (settings.getSettingByName<float>("disableanim") == nullptr) settings.addSetting("disableanim", false);
    }

    void settingsRender() override { }
};