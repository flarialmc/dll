#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Events/Game/ItemDropEvent.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"
#include <chrono>

class InventoryLock : public Module {
    OptionsParser parser;
    ItemStack* lastItemThrown;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastThrown = std::chrono::high_resolution_clock::now();
public:


    InventoryLock() : Module("Inventory Lock", "Locks Items from being dropped",
        IDR_SERVER_IP_PNG, "", false, {"item", "drop", "inventory"}) {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onKey(KeyEvent& event);

    void onItemDrop(ItemDropEvent& event);
};

