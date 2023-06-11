#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FPSListener.hpp"

class FPSCounter : public Module {

public:

    FPSCounter() : Module("FPS Counter", "hiasdashgadhduaidhyahydasdjasdasjdas", "\\Flarial\\assets\\fps.png", 'L') {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new FPSListener("FPS", this));

    }

    void onDisable() override {

        EventHandler::unregisterListener("FPS");

    }
};