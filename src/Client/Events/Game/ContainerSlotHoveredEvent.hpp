#pragma once

#include <cstdint>
#include "../Event.hpp"

class ContainerScreenController;

class ContainerSlotHoveredEvent : public Event {
private:
    int64_t hoveredSlot;
    std::string collectionName;
    ContainerScreenController* controller;
public:
    [[nodiscard]] int64_t getHoveredSlot() {
        return this->hoveredSlot;
    }

    [[nodiscard]] std::string getCollectionName() {
        return this->collectionName;
    }

    [[nodiscard]] ContainerScreenController* getController() {
        return this->controller;
    }

    explicit ContainerSlotHoveredEvent(std::string collectionName, int64_t hoveredSlot, ContainerScreenController* controller = nullptr)
        : collectionName(collectionName), hoveredSlot(hoveredSlot), controller(controller) {}
};