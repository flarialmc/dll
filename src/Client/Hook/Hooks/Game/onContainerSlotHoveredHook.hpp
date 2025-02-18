#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class onContainerSlotHoveredHook : public Hook {
private:
    static int64_t ContainerScreenController__onContainerSlotHovered(class ContainerScreenController *_this, const std::string* containerName, int64_t slot) {

        auto event = nes::make_holder<ContainerSlotHovered>(*containerName, slot);
        eventMgr.trigger(event);

        return funcOriginal(_this, containerName, slot);
    }

public:
    typedef int64_t(__thiscall *original)(class ContainerScreenController *, const std::string*, int64_t);

    static inline original funcOriginal = nullptr;

    onContainerSlotHoveredHook() : Hook("onContainerSlotHoveredHook", GET_SIG_ADDRESS("ContainerScreenController::_onContainerSlotHovered")) {}

    void enableHook() override {
        this->autoHook((void *) ContainerScreenController__onContainerSlotHovered, (void **) &funcOriginal);
    }
};