#pragma once

#include "../Module.hpp"
#include "ZoomListener.hpp"
#include "../../../Events/EventHandler.hpp"


class Zoom : public Module {

public:
    
    float fov = 30.0f;

    Zoom() : Module("Zoom", "chike wi!", "\\Flarial\\assets\\fps.png", 'C') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new ZoomListener("Zoom", this));

        Module::onEnable();
    }

    
    void onDisable() override {
        Module::onDisable();
    }
};