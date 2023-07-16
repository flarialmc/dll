#pragma once
#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "SaveConfigListener.hpp"

class SaveConfig : public Module {

public:

    SaveConfig() : Module("SaveConfig", "Disables the client.", "", VK_F7) {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new SaveConfigListener("SaveConfig"));

    }

    void onDisable() override {

        EventHandler::unregisterListener("SaveConfig");

    }
};