#pragma once

#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "rgbListener.hpp"

class RGBController : public Module {

public:

    RGBController() : Module("RGB Controller", "Does the rgb bing chilling", "", "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new rgbListener("RGB Controller"));
        Module::onEnable();

    }

    void onDisable() override {

        EventHandler::unregisterListener("RGB Controller");
        Module::onDisable();

    }
};