#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class HurtColorHook : public Hook {
private:

    static MCCColor *HurtColorCallback(void *a1, MCCColor *color, void *a3) {

        HurtColorEvent event(funcOriginal(a1, color, a3));
        EventHandler::onGetHurtColor(event);

        return event.getHurtColor();

    }

public:
    typedef MCCColor *(__thiscall *HurtColorOriginal)(void *a1, MCCColor *color, void *a3);

    static inline HurtColorOriginal funcOriginal = nullptr;

    HurtColorHook() : Hook("Hurt Color Hook", "") {}

    void enableHook() override {

        auto RefAddr = Memory::findSig("E8 ? ? ? ? E9 ? ? ? ? 8B 43 ? 48 8D 54 24");
        auto RealFunc = Memory::offsetFromSig(RefAddr, 1);


        this->manualHook((void *) RealFunc, HurtColorCallback, (void **) &funcOriginal);
    }
};


