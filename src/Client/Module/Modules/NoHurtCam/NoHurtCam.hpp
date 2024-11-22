#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "NoHurtCamListener.hpp"


class NoHurtCam : public Module {
public:

    NoHurtCam() : Module("No Hurt Cam", "Disables hurt camera animation", IDR_REACH_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new NoHurtCamListener("NoHurtCam", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("NoHurtCam");
        Module::onDisable();
    }
};