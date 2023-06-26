#pragma once
#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "UninjectListener.hpp"

class Uninject : public Module {

public:

    Uninject() : Module("Uninject", "Disables the client.", "", VK_F8) {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new UninjectListener("Uninject"));

    }

    void onDisable() override {

        EventHandler::unregisterListener("Uninject");

    }
};