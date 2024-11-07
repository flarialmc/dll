#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class HurtColorHook : public Hook {
private:

    static MCCColor *HurtColorCallback(void *a1, MCCColor *color, void *a3) {

        auto event = nes::make_holder<HurtColorEvent>(funcOriginal(a1, color, a3));

        if(WinrtUtils::checkAboveOrEqual(21,40)) {
            auto actor = hat::member_at<Actor*>(a3, 0x38);
            if(actor && actor->getHurtTime() > 0) {
                eventMgr.trigger(event);
            }
        } else {
            eventMgr.trigger(event);
        }

        return event->getHurtColor();

    }

public:
    typedef MCCColor *(__thiscall *HurtColorOriginal)(void *a1, MCCColor *color, void *a3);

    static inline HurtColorOriginal funcOriginal = nullptr;

    HurtColorHook() : Hook("Hurt Color Hook", GET_SIG_ADDRESS("HurtColor")) {}

    void enableHook() override {
        auto RealFunc = Memory::offsetFromSig(address, 1);

        this->manualHook((void *) RealFunc, (void*)HurtColorCallback, (void **) &funcOriginal);
    }
};


