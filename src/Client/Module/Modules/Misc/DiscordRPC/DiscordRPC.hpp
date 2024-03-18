#pragma once

#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "DiscordRPCListener.hpp"

class DiscordRPC : public Module {

public:

    DiscordRPC() : Module("DiscordRPC", "Allowes launcher to get server the player is in.", "", "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new DiscordRPCListener("CentreCursor"));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("CentreCursor");
        Module::onDisable();

    }
};