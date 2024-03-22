#pragma once

#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "CentreCursorListener.hpp"

class CentreCursor : public Module {

public:

    CentreCursor() : Module("CentreCursor", "Center your cursor on GUI open.", "", "") {

        Module::setup();

    };

    void onEnable() override {

        EventHandler::registerListener(new CentreCursorListener("CentreCursor"));
        Module::onEnable();
    }

    void onDisable() override {

        EventHandler::unregisterListener("CentreCursor");
        Module::onDisable();

    }
};