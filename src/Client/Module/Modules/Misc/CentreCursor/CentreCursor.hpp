#pragma once
#include "../../Module.hpp"
#include "../../../../Client.hpp"
#include "../../../../Events/EventHandler.hpp"
#include "CentreCursorListener.hpp"

class CentreCursor : public Module {

public:

    CentreCursor() : Module("CentreCursor", "Center your cursor on GUI open.", "", VK_F7) {

        onEnable();

    };

    void onEnable() override {

        EventHandler::registerListener(new CentreCursorListener("CentreCursor"));

    }

    void onDisable() override {

        EventHandler::unregisterListener("CentreCursor");

    }
};