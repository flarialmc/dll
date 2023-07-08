#pragma once

#include "DeepfryListener.hpp"
#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Deepfry : public Module {

public:

    void NormalRender(int index, std::string text, std::string value) override {

    }

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