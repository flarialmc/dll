#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "SnapLookListener.hpp"

class SnapLook : public Module {

public:

    SnapLook() : Module("SnapLook", "Quickly look behind you.", IDR_EYE_PNG, "V") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new SnapListener("Snap", this));
        Module::onEnable();

    }

    void onDisable() override {
        EventHandler::unregisterListener("Snap");
        Module::onDisable();
    }

    void settingsRender() override { }
};