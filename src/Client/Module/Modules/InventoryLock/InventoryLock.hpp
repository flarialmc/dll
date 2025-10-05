#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Events/Game/ItemDropEvent.hpp"
#include "Events/Game/ContainerSlotHoveredEvent.hpp"
#include "Events/Game/ContainerScreenControllerTickEvent.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"
#include <chrono>
#include "../../../../Assets/Assets.hpp"

class InventoryLock : public Module {
    OptionsParser parser;
    ItemStack* lastItemThrown;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastThrown = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> lastSlotHovered = std::chrono::high_resolution_clock::now();
    int64_t currentHoveredSlot;
    std::string currentCollectionName;
    ContainerScreenController* lastContainer;
public:


    InventoryLock() : Module("Inventory Lock", "Locks Items from being dropped",
        IDR_INVENTORYLOCK_PNG, "", false, {"item", "drop", "inventory"}) {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onKey(KeyEvent& event);

    void onItemDrop(ItemDropEvent& event);

    void onContainerSlotHovered(ContainerSlotHoveredEvent& event);

    void onContainerTick(ContainerScreenControllerTickEvent& event);

private:
    bool shouldLockItem(ItemStack* item);
    bool handleDoubleClickThrow(ItemStack* item, KeyEvent& event);
};

