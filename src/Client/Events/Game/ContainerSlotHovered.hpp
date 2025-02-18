#pragma once

#include <cstdint>
#include "../Event.hpp"
#include "../Cancellable.hpp"

class ContainerSlotHovered : public Event {
private:
    int64_t hoveredSlot;
    std::string containerName;
public:
    [[nodiscard]] int64_t getHoveredSlot() {
        return this->hoveredSlot;
    }

    [[nodiscard]] std::string getContainerName() {
        return this->containerName;
    }

    explicit ContainerSlotHovered(std::string containerName, int64_t hoveredSlot) : containerName(containerName), hoveredSlot(hoveredSlot) {}
};