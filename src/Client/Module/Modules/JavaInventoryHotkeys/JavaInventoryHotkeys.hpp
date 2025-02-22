#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Options/OptionsParser.hpp"

struct MoveRequest {
    int64_t destSlot;
    int64_t hoveredSlot;
    std::string collectionName;
};


class JavaInventoryHotkeys : public Module {
private:
    OptionsParser parser;
    int64_t currentHoveredSlot;
    std::string currentCollectionName;

    std::queue<MoveRequest> moveRequests;
    void* lastContainer = nullptr;
    bool isInPauseOrGame = false;
public:
    JavaInventoryHotkeys() : Module("Java Inventory Hotkeys",
                          "Allows to swap items in your inventory java-like.",
                          IDR_FULLBRIGHT_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        parser.parseOptionsFile();

        Listen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
        Listen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
        Listen(this, SetupAndRenderEvent, &JavaInventoryHotkeys::onSetupAndRender)
        Listen(this, MouseEvent, &JavaInventoryHotkeys::onMouseInput)
        Listen(this, KeyEvent, &JavaInventoryHotkeys::onKey)

        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
        Deafen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
        Deafen(this, SetupAndRenderEvent, &JavaInventoryHotkeys::onSetupAndRender)
        Deafen(this, MouseEvent, &JavaInventoryHotkeys::onMouseInput)
        Deafen(this, KeyEvent, &JavaInventoryHotkeys::onKey)
        Module::onDisable();
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));


        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    void onSetupAndRender(SetupAndRenderEvent &event) {
        auto screen = SDK::getCurrentScreen();
        isInPauseOrGame = screen == "hud_screen" || screen == "pause_screen";
        if(isInPauseOrGame)
            clearQueue();
    }

    int isSlotKeybind(std::string key) {
        for (int i = 0; i < 9; i++) {
            auto value = parser.options["keyboard_type_0_key.hotbar." + std::to_string(i + 1)];
            if (value == key) return i;
        }

        return -1;
    }

    void onMouseInput(MouseEvent &event) {
        if(event.getAction() != MouseAction::Press) return;
        if(isInPauseOrGame) return;

        auto targetSlot = isSlotKeybind(std::to_string(-100 + (int)event.getButton()));
        if (targetSlot == -1) return;

        moveRequests.push({targetSlot, currentHoveredSlot, currentCollectionName});
    }

    void onKey(KeyEvent &event) {
        if(event.getAction() != ActionType::Pressed) return;
        if(isInPauseOrGame) return;

        auto targetSlot = isSlotKeybind(std::to_string(event.getKey()));
        if (targetSlot == -1) return;

        moveRequests.push({targetSlot, currentHoveredSlot, currentCollectionName});
    }

    void onContainerSlotHovered(ContainerSlotHoveredEvent &event) {
        currentHoveredSlot = event.getHoveredSlot();
        currentCollectionName = event.getCollectionName();
    };

    void onContainerTick(ContainerScreenControllerTickEvent &event) {
        auto controller = event.getContainerScreenController();
        if(!controller) return;
        if(lastContainer != controller) {
            clearQueue();
            lastContainer = controller;
        };

        for (moveRequests; !moveRequests.empty(); moveRequests.pop()) {
            auto request = moveRequests.front();

            controller->swap(request.collectionName, request.hoveredSlot, "hotbar_items", request.destSlot);
        }
    };

    void clearQueue() {
        for (moveRequests; !moveRequests.empty(); moveRequests.pop()) {}
    }
};