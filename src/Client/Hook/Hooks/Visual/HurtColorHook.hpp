#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class HurtColorHook : public Hook {
private:

    static MCCColor *HurtColorCallback(void *a1, MCCColor *color, void *a3) {

        HurtColorEvent event(funcOriginal(a1, color, a3));

        if(WinrtUtils::checkAboveOrEqual(21,40)) {
            auto actor = hat::member_at<Actor*>(a3, 0x38);
            if(actor && actor->getHurtTime() > 0) {
                EventHandler::onGetHurtColor(event);
            }
        } else {
            EventHandler::onGetHurtColor(event);
        }

        return event.getHurtColor();

    }

public:
    typedef MCCColor *(__thiscall *HurtColorOriginal)(void *a1, MCCColor *color, void *a3);

    static inline HurtColorOriginal funcOriginal = nullptr;

    HurtColorHook() : Hook("Hurt Color Hook", "") {}

    void enableHook() override {

        auto RefAddr = GET_SIG_ADDRESS("HurtColor");
        auto RealFunc = Memory::offsetFromSig(RefAddr, 1);


        this->manualHook((void *) RealFunc, (void*)HurtColorCallback, (void **) &funcOriginal);
    }
};


