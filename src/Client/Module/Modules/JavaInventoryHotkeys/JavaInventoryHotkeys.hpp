#pragma once

#include "../Module.hpp"

struct MoveRequest {
    int64_t destSlot;
    int64_t hoveredSlot;
    std::string collectionName;
};


class JavaInventoryHotkeys : public Module {
private:
    std::string slots[9] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
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

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("slot1") == nullptr) settings.addSetting("slot1", slots[0]);
        if (settings.getSettingByName<std::string>("slot2") == nullptr) settings.addSetting("slot2", slots[1]);
        if (settings.getSettingByName<std::string>("slot3") == nullptr) settings.addSetting("slot3", slots[2]);
        if (settings.getSettingByName<std::string>("slot4") == nullptr) settings.addSetting("slot4", slots[3]);
        if (settings.getSettingByName<std::string>("slot5") == nullptr) settings.addSetting("slot5", slots[4]);
        if (settings.getSettingByName<std::string>("slot6") == nullptr) settings.addSetting("slot6", slots[5]);
        if (settings.getSettingByName<std::string>("slot7") == nullptr) settings.addSetting("slot7", slots[6]);
        if (settings.getSettingByName<std::string>("slot8") == nullptr) settings.addSetting("slot8", slots[7]);
        if (settings.getSettingByName<std::string>("slot9") == nullptr) settings.addSetting("slot9", slots[8]);
    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Keybinds");
        for (int i = 0; i < 9; i++) {
            this->addKeybind("Slot " + std::to_string(i + 1), "", settings.getSettingByName<std::string>("slot" + std::to_string(i + 1))->value);
        }

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    int isSlotKeybind(std::string key) {
        for (int i = 0; i < 9; i++) {
            auto value = settings.getSettingByName<std::string>("slot" + std::to_string(i + 1))->value;
            if (value == key) return i;
        }

        return -1;
    }

    void onKey(KeyEvent &event) {
        if(event.getAction() != 1) return;

        auto targetSlot = isSlotKeybind(event.getKeyAsString(true));
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