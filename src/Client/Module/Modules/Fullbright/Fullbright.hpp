#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class Fullbright : public Module {

public:

    Fullbright() : Module("Fullbright", "No need for torches!\nProvides consistent and constant illumination.\nEffectively removing darkness and shadows.", "\\Flarial\\assets\\fullbright.png", 'C') {

        onEnable();

    };

    void NormalRender(int index, std::string text, std::string value) override {

    }

    void onEnable() override {
        Module::onEnable();
    }


    void onDisable() override {
        Module::onDisable();
    }
};