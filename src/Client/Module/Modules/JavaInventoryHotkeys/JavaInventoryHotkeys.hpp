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
public:
    JavaInventoryHotkeys() : Module("Java Inventory Hotkeys",
                          "Allows to swap items in your inventory java-like.",
                          IDR_FULLBRIGHT_PNG, "") {
        Module::setup();

    };

    void onEnable() override {
        Listen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
        Listen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
        Listen(this, KeyEvent, &JavaInventoryHotkeys::onKey)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
        Deafen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
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

    int isSlotKeybind(std::string key) {
        for (int i = 1; i < 10; i++) {
            auto value = parser.parseOptionsFile()["keyboard_type_0_key.hotbar." + std::to_string(i)];
            if (value == key) return i - 1;
        }

        return -1;
    }

    void onKey(KeyEvent &event) {
        if(event.getAction() != 1) return;

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

        for (moveRequests; !moveRequests.empty(); moveRequests.pop()) {
            auto request = moveRequests.front();

            controller->swap(request.collectionName, request.hoveredSlot, "hotbar_items", request.destSlot);
        }
    };
};