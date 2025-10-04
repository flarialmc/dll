#include "../../../../SDK/Client/Actor/LocalPlayer.hpp"
#include "InventoryLock.hpp"
#include "../../../../SDK/Client/Actor/LocalPlayer.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

void InventoryLock::onEnable() {
    parser.parseOptionsFile();
    Listen(this, KeyEvent, &InventoryLock::onKey)
    Listen(this, ItemDropEvent, &InventoryLock::onItemDrop)
    Listen(this, ContainerSlotHoveredEvent, &InventoryLock::onContainerSlotHovered)
    Listen(this, ContainerScreenControllerTickEvent, &InventoryLock::onContainerTick)
    Module::onEnable();
}

void InventoryLock::onDisable() {
    Deafen(this, KeyEvent, &InventoryLock::onKey)
    Deafen(this, ItemDropEvent, &InventoryLock::onItemDrop)
    Deafen(this, ContainerSlotHoveredEvent, &InventoryLock::onContainerSlotHovered)
    Deafen(this, ContainerScreenControllerTickEvent, &InventoryLock::onContainerTick)
    Module::onDisable();
}

void InventoryLock::defaultConfig() {
    Module::defaultConfig("core");
    setDef("toolsonly", false);
    setDef("doubleclickthrow", false);
}

void InventoryLock::settingsRender(float settingsOffset) {

    initSettingsPage();

    addToggle("Tools Only", "Only lock tools from being dropped", "toolsonly");
    addToggle("Double Click Throw", "Throw a locked item by double clicking your drop key", "doubleclickthrow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void InventoryLock::onKey(KeyEvent& event)
{
    std::string option = parser.options["ctrl_fullkeyboardgameplay"] == "1" ? "keyboard_type_1_key." : "keyboard_type_0_key.";
    auto value = parser.options[option + "drop"];

    if (value != std::to_string(event.getKey()) || event.action != 1) return;

    auto now = std::chrono::high_resolution_clock::now();
    auto timeSinceHover = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSlotHovered).count();
    bool recentlyHovered = timeSinceHover < 50;

    ItemStack* item = nullptr;
    auto player = SDK::clientInstance->getLocalPlayer();

    if (SDK::currentScreen == "hud_screen")
    {
        // Handle hotbar drops
        auto inventory = player->getSupplies()->getInventory();
        auto selectedSlot = player->getSupplies()->getSelectedSlot();
        if (inventory->getItem(selectedSlot)->getItem() != nullptr)
        {
            item = inventory->getItem(selectedSlot);
        }
    }
    else if (recentlyHovered)
    {
        auto srcContainerType = lastContainer->getContainerType(currentCollectionName);

        item = lastContainer->getContainerItem(srcContainerType, currentHoveredSlot);
    }

    if (item && item->getItem() != nullptr)
    {
        if (shouldLockItem(item))
        {
            if (!handleDoubleClickThrow(item, event))
            {
                event.cancel();
            }
        }
    }
}

void InventoryLock::onItemDrop(ItemDropEvent& event)
{
}

void InventoryLock::onContainerSlotHovered(ContainerSlotHoveredEvent& event)
{
    currentHoveredSlot = event.getHoveredSlot();
    currentCollectionName = event.getCollectionName();
}

void InventoryLock::onContainerTick(ContainerScreenControllerTickEvent& event)
{
    if (lastContainer != event.getContainerScreenController()) {
        lastContainer = event.getContainerScreenController();
    };
    lastSlotHovered = std::chrono::high_resolution_clock::now();
}

bool InventoryLock::shouldLockItem(ItemStack* item)
{
    if (getOps<bool>("toolsonly"))
    {
        return item->getMaxDamage() != 0;
    }
    return true;
}

bool InventoryLock::handleDoubleClickThrow(ItemStack* item, KeyEvent& event)
{
    if (!getOps<bool>("doubleclickthrow")) return false;

    auto now = std::chrono::high_resolution_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastThrown).count();

    if (lastItemThrown == item && timeDiff < 300)
    {
        lastThrown = now;
        lastItemThrown = item;
        return true;
    }
    else
    {
        lastThrown = now;
        lastItemThrown = item;
        return false;
    }
}
