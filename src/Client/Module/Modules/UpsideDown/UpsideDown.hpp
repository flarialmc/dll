#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../Client.hpp"
#include "UpsideDownListener.hpp"

class UpsideDown : public Module {

public:


    UpsideDown() : Module("Upside Down", "No need to flip your monitor!!", IDR_UPSIDE_DOWN_PNG, "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerOrderedPriorityListener(new UpsideDownListener("UpsideDown", this), 3);
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("UpsideDown");
        Module::onDisable();
    }

};

