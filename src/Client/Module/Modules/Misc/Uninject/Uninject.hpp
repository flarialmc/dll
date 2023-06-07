#pragma once
#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "UninjectListener.hpp"

class Uninject : public Module {

public:

    Uninject() : Module("Uninject", "Disables the client.", "", 'P') {};

    void onEnable() override {

        EventHandler::registerListener(new UninjectListener("Uninject", this));

    }

    void onDisable() override {

        EventHandler::unregisterListener("Uninject");

    }
};