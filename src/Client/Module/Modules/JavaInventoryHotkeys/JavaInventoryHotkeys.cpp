#include "JavaInventoryHotkeys.hpp"

#include "Events/EventManager.hpp"

void JavaInventoryHotkeys::onEnable() {
    parser.parseOptionsFile();

    Listen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
    Listen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
    Listen(this, MouseEvent, &JavaInventoryHotkeys::onMouseInput)
    Listen(this, KeyEvent, &JavaInventoryHotkeys::onKey)

    Module::onEnable();
}

void JavaInventoryHotkeys::onDisable() {
    Deafen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
    Deafen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
    Deafen(this, MouseEvent, &JavaInventoryHotkeys::onMouseInput)
    Deafen(this, KeyEvent, &JavaInventoryHotkeys::onKey)
    Module::onDisable();
}

void JavaInventoryHotkeys::settingsRender(float settingsOffset) {
    initSettingsPage();


    FlarialGUI::UnsetScrollView();

    this->resetPadding();
}

int JavaInventoryHotkeys::isSlotKeybind(std::string key) {
    for (int i = 0; i < 9; i++) {
        std::string option = parser.options["ctrl_fullkeyboardgameplay"] == "1" ? "keyboard_type_1_key.hotbar." : "keyboard_type_0_key.hotbar.";
        auto value = parser.options[option + FlarialGUI::cached_to_string(i + 1)];
        if (value == key) return i;
    }

    return -1;
}

void JavaInventoryHotkeys::onMouseInput(MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (event.getAction() != MouseAction::Press) return;
    std::string screen = SDK::getCurrentScreen();
    if (screen == "hud_screen" || screen == "pause_screen") return;

    auto targetSlot = isSlotKeybind(FlarialGUI::cached_to_string(-100 + (int)event.getButton()));
    if (targetSlot == -1) return;

    moveRequests.push({ targetSlot, currentHoveredSlot, currentCollectionName });
}

void JavaInventoryHotkeys::onKey(KeyEvent &event) {
    if (!this->isEnabled()) return;
    std::string screen = SDK::getCurrentScreen();
    if (screen == "hud_screen" || screen == "pause_screen") clearQueue();
    
    if (event.getAction() != ActionType::Pressed) return;

    auto targetSlot = isSlotKeybind(FlarialGUI::cached_to_string(event.getKey()));
    if (targetSlot == -1) return;

    moveRequests.push({ targetSlot, currentHoveredSlot, currentCollectionName });
}

void JavaInventoryHotkeys::onContainerSlotHovered(ContainerSlotHoveredEvent &event) {
    if (!this->isEnabled()) return;
    currentHoveredSlot = event.getHoveredSlot();
    currentCollectionName = event.getCollectionName();
}

void JavaInventoryHotkeys::onContainerTick(ContainerScreenControllerTickEvent &event) {
    if (!this->isEnabled()) return;
    auto controller = event.getContainerScreenController();
    if (!controller) return;
    if (lastContainer != controller) {
        clearQueue();
        lastContainer = controller;
    };

    for (moveRequests; !moveRequests.empty(); moveRequests.pop()) {
        auto request = moveRequests.front();

        if (!canSwap(request.collectionName)) continue;
        controller->swap(request.collectionName, request.hoveredSlot, "hotbar_items", request.destSlot);
    }
}

bool JavaInventoryHotkeys::canSwap(std::string_view collectionName) {
    bool isItemContainer = collectionName.find("_item") != std::string_view::npos;
    bool isRecipeContainer = collectionName.find("recipe_") != std::string_view::npos;
    bool isSearchContainerOrBar = collectionName.find("search") != std::string_view::npos;
    return isItemContainer || (isRecipeContainer && !isSearchContainerOrBar);
}

void JavaInventoryHotkeys::clearQueue() {
    for (moveRequests; !moveRequests.empty(); moveRequests.pop()) {}
}
