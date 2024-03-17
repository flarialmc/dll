#pragma once

#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "UninjectListener.hpp"

class Uninject : public Module {

public:

    Uninject() : Module("Uninject", "Disables the client.", "", "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new UninjectListener("Uninject"));

    }
};