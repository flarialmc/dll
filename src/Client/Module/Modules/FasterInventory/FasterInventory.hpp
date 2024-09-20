#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "FasterInventoryListener.hpp"


class FasterInventory : public Module {
public:

    FasterInventory() : Module("Faster Inventory", "Makes inventory opening ping-independent", IDR_LIKE_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        EventHandler::registerListener(new FasterInventoryListener("FasterInventory", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("FasterInventory");
        Module::onDisable();
    }
};