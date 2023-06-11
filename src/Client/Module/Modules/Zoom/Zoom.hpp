#pragma once

#include "../Module.hpp"


class Zoom : public Module {

public:
    
    float fov = 30.0f;

    Zoom() : Module("Zoom", "chike wi!", "\\Flarial\\assets\\fps.png", 'C') {};

    void onEnable() override {
        Module::onEnable();
    }

    
    void onDisable() override {
        Module::onDisable();
    }
};