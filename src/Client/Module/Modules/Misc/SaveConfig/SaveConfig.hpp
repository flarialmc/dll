#pragma once

#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "SaveConfigListener.hpp"

class SaveConfig : public Module {

public:

    SaveConfig() : Module("SaveConfig", "Disables the client.", "", "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new SaveConfigListener("SaveConfig"));
        Module::onEnable();

    }

    void onDisable() override {

        EventHandler::unregisterListener("SaveConfig");
        Module::onEnable();

    }
};