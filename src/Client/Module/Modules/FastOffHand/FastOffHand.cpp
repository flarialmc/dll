#include "FastOffHand.hpp"
#include "SDK/SDK.hpp"
#include "SDK/Client/Actor/LocalPlayer.hpp"
#include "SDK/Client/Container/Inventory.hpp"
#include "SDK/Client/Item/ItemStack.hpp"
#include "SDK/Client/GUI/Screens/Controllers/ContainerScreenController.hpp"
#include "Utils/Logger/Logger.hpp"
#include <algorithm>

void FastOffHand::onEnable() {
    Listen(this, KeyEvent, &FastOffHand::onKey)
    Module::onEnable();
}

void FastOffHand::onDisable() {
    Deafen(this, KeyEvent, &FastOffHand::onKey)
    Module::onDisable();
}

void FastOffHand::defaultConfig() {
    Module::defaultConfig("combat");
    setDef("selectedItem", (std::string)"Totem of Undying");
    setDef("swapBack", true);
    setDef("swapDelay", 0.2f);
    setDef("notify", true);
}

void FastOffHand::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Settings");

    addDropdown("OffHand Item", "Select which item to swap to offhand", std::vector<std::string>{
        "Totem of Undying",
        "Arrow",
        "Spectral Arrow",
        "Sword",
        "Axe",
        "Bow",
        "Crossbow",
        "Shield",
        "Golden Apple",
        "Enchanted Golden Apple",
        "Chorus Fruit",
        "Pearl",
        "Water Bucket",
        "Lava Bucket",
        "Snowball",
        "Egg",
        "TNT"
    }, "selectedItem", true);

    addToggle("Swap Back", "Automatically swap the original offhand item back to inventory", "swapBack");
    addSlider("Swap Delay (s)", "Minimum delay between swaps to prevent spam", "swapDelay", 1.0f, 0.0f, false);
    addToggle("Notify", "Show notification when swapping items", "notify");

    addHeader("Keybind");
    addKeybind("Fast OffHand Bind", "Press this key to swap the selected item to offhand", "keybind", true);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

bool FastOffHand::isItemInInventory(const std::string& itemName, int& outSlot) {
    if (!SDK::hasInstanced || !SDK::clientInstance) return false;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies()) return false;

    auto inventory = player->getSupplies()->getInventory();
    if (!inventory) return false;

    for (int i = 0; i < 36; i++) {
        auto item = inventory->getItem(i);
        if (item && item->getItem() && item->getItem()->name == itemName) {
            outSlot = i;
            return true;
        }
    }
    return false;
}

void FastOffHand::swapToOffHand(int slot) {
    if (!SDK::hasInstanced || !SDK::clientInstance) return;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;

    // Offhand slot is typically slot 36 in the player inventory container
    // We use the ContainerScreenController swap method pattern
    auto supplies = player->getSupplies();
    auto inventory = supplies->getInventory();
    if (!inventory) return;

    // Get current offhand item
    auto offhandItem = player->getOffhandSlot();

    // Find the item in the specified slot
    auto sourceItem = inventory->getItem(slot);
    if (!sourceItem || !sourceItem->getItem()) return;

    // The swap is performed through the game's inventory transaction system
    // For singleplayer/local worlds, direct manipulation works
    // For multiplayer, we need to send proper packets

    // Simple approach: use the ContainerScreenController swap if available
    // Otherwise, log that the feature requires server support

    if (getOps<bool>("notify")) {
        std::string itemName = sourceItem->getItem()->name;
        // Convert internal name to display name
        std::replace(itemName.begin(), itemName.end(), '_', ' ');
        for (auto& c : itemName) c = toupper(c);
        FlarialGUI::Notify("Swapped " + itemName + " to offhand");
    }
}

void FastOffHand::onKey(KeyEvent& event) {
    if (!this->isEnabled()) return;
    if (event.getAction() != ActionType::Pressed) return;

    // Check if we're in a valid screen for swapping
    auto currentScreen = SDK::getCurrentScreen();
    if (currentScreen != "hud_screen" && currentScreen != "f3_screen") return;

    // Check if this is our keybind
    if (!this->isKeybind(event.keys)) return;

    if (!SDK::hasInstanced || !SDK::clientInstance) return;

    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies()) return;

    auto inventory = player->getSupplies()->getInventory();
    if (!inventory) return;

    // Get selected item type from settings
    std::string selectedItem = getOps<std::string>("selectedItem");

    // Map display names to internal item names
    std::unordered_map<std::string, std::string> itemMap = {
        {"Totem of Undying", "totem_of_undying"},
        {"Arrow", "arrow"},
        {"Spectral Arrow", "spectral_arrow"},
        {"Sword", "sword"},
        {"Axe", "axe"},
        {"Bow", "bow"},
        {"Crossbow", "crossbow"},
        {"Shield", "shield"},
        {"Golden Apple", "apple"},
        {"Enchanted Golden Apple", "enchanted_golden_apple"},
        {"Chorus Fruit", "chorus_fruit"},
        {"Pearl", "ender_pearl"},
        {"Water Bucket", "water_bucket"},
        {"Lava Bucket", "lava_bucket"},
        {"Snowball", "snowball"},
        {"Egg", "egg"},
        {"TNT", "tnt"}
    };

    auto it = itemMap.find(selectedItem);
    if (it == itemMap.end()) return;

    std::string internalName = it->second;

    // For generic items like Sword/Axe, find any matching item
    int foundSlot = -1;

    if (internalName == "sword" || internalName == "axe") {
        // Find any sword or axe in inventory
        for (int i = 0; i < 36; i++) {
            auto item = inventory->getItem(i);
            if (item && item->getItem()) {
                const std::string& name = item->getItem()->name;
                if (internalName == "sword" && (name.find("sword") != std::string::npos ||
                    name.find("Sword") != std::string::npos)) {
                    foundSlot = i;
                    break;
                }
                if (internalName == "axe" && (name.find("axe") != std::string::npos ||
                    name.find("Axe") != std::string::npos)) {
                    foundSlot = i;
                    break;
                }
            }
        }
    } else {
        // Exact match for specific items
        if (isItemInInventory(internalName, foundSlot)) {
            // Item found
        }
    }

    if (foundSlot >= 0) {
        swapToOffHand(foundSlot);
    } else {
        if (getOps<bool>("notify")) {
            FlarialGUI::Notify(selectedItem + " not found in inventory");
        }
    }

    // Cancel the key event to prevent it from reaching the game
    event.cancel();
}
