#pragma once

#include "DeepfryListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Deepfry : public Module {

public:
    Deepfry() : Module("Deepfry", "Theres only one way to find out.", IDR_FRYING_PAN_PNG, "") {

        Module::setup();
    };

    void onEnable() override {

        EventHandler::registerPriorityListener(new DeepfryListener("Deepfry", this));

        Module::onEnable();

    }

    void onDisable() override {

        EventHandler::unregisterListener("Deepfry");

        Module::onDisable();

    }

    void defaultConfig() override {
        if (settings.getSettingByName<bool>("paint") == nullptr) settings.addSetting("paint", false);
    }

    void settingsRender() override { }
};