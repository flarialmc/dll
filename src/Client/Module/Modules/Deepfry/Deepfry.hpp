#pragma once

#include "DeepfryListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Deepfry : public Module {

public:

    Deepfry() : Module("Deepfry", "chike wi!", "\\Flarial\\assets\\frying-pan.png", 'b') {

        onEnable();

    };

    void onEnable() override {
        EventHandler::registerListener(new DeepfryListener("Deepfry", this));
        Module::onEnable();
    }


    void onDisable() override {
        Module::onDisable();
    }
};