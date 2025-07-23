#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class ContainerScreenControllerHook : public Hook {
private:
    static int64_t ContainerScreenController__onContainerSlotHovered(ContainerScreenController *_this, const std::string* collectionName, int64_t slot) {

        auto event = nes::make_holder<ContainerSlotHoveredEvent>(*collectionName, slot);
        eventMgr.trigger(event);

        return funcOriginal0(_this, collectionName, slot);
    }

    static uint32_t ContainerScreenController__tick(ContainerScreenController *_this) {

        auto event = nes::make_holder<ContainerScreenControllerTickEvent>(_this);
        eventMgr.trigger(event);

        return funcOriginal1(_this);
    }

public:
    typedef int64_t(__thiscall *original0)(ContainerScreenController *, const std::string*, int64_t);

    static inline original0 funcOriginal0 = nullptr;

    typedef uint32_t(__thiscall *original1)(ContainerScreenController *);

    static inline original1 funcOriginal1 = nullptr;

    ContainerScreenControllerHook() : Hook("ContainerScreenControllerHooks", 0) {}

    void enableHook() override {
        auto address0 = GET_SIG_ADDRESS("ContainerScreenController::_onContainerSlotHovered");
        this->manualHook((void *) address0, (void *) ContainerScreenController__onContainerSlotHovered, (void **) &funcOriginal0);

        auto address1 = Memory::offsetFromSig(GET_SIG_ADDRESS("ContainerScreenController::tick"), 1);
        this->manualHook((void *) address1, (void *) ContainerScreenController__tick, (void **) &funcOriginal1);
    }
};