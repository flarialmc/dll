#pragma once
#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "rgbListener.hpp"

class RGBController : public Module {

public:

    RGBController() : Module("RGB Controller", "Does the rgb bing chilling", "", VK_F7) {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new rgbListener("RGB Controller"));

    }

    void onDisable() override {

        EventHandler::unregisterListener("RGB Controller");

    }
};