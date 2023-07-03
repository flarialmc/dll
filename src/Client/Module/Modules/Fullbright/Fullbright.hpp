#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Fullbright : public Module {

public:

    Fullbright() : Module("Fullbright", "chike wi!", "\\Flarial\\assets\\fullbright.png", 'C') {

        onEnable();

    };

    void onEnable() override {
        Module::onEnable();
    }


    void onDisable() override {
        Module::onDisable();
    }
};