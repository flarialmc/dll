#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"

class UpsideDown : public Module {

public:


    UpsideDown() : Module("Upside Down", "No need to flip your monitor!!", IDR_UPSIDE_DOWN_PNG, "", false, false) {

        Module::setup();

    };

    void onEnable() override {
        Listen(this, FOVEvent, &UpsideDown::onGetFOV)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, FOVEvent, &UpsideDown::onGetFOV)
        Module::onDisable();
    }

    void onGetFOV(FOVEvent &event) {
        auto fov = event.getFOV();
        fov = 360 - fov;
        event.setFOV(fov);
    }
};

