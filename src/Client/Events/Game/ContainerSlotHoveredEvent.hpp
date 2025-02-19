#pragma once

#include <cstdint>
#include "../Event.hpp"

class ContainerSlotHoveredEvent : public Event {
private:
    int64_t hoveredSlot;
    std::string collectionName;
public:
    [[nodiscard]] int64_t getHoveredSlot() {
        return this->hoveredSlot;
    }

    [[nodiscard]] std::string getCollectionName() {
        return this->collectionName;
    }

    explicit ContainerSlotHoveredEvent(std::string collectionName, int64_t hoveredSlot) : collectionName(collectionName), hoveredSlot(hoveredSlot) {}
};