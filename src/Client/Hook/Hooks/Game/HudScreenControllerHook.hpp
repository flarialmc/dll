#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "SDK/Client/GUI/Screens/Controllers/HudScreenController.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"
// #include "Events/Game/HudScreenControllerTickEvent.hpp"

class HudScreenControllerHook : public Hook {
private:

    static void HudScreenController__tick(HudScreenController *controller) {
        // auto event = nes::make_holder<HudScreenControllerTickEvent>(_this);
        // eventMgr.trigger(event);
        // Logger::debug("ticking lol {}", controller->mHasPresentedInitialChat);

        // controller->mHasPresentedInitialChat = false;

        return funcOriginalTick(controller);
    }

public:

    typedef void(__thiscall *originalTick)(HudScreenController *);

    static inline originalTick funcOriginalTick = nullptr;

    HudScreenControllerHook() : Hook("HudScreenControllerHooks", 0) {}

    void enableHook() override {
        auto addressTick = GET_SIG_ADDRESS("HudScreenController::tick");
        this->manualHook((void *) addressTick, (void *) HudScreenController__tick, (void **) &funcOriginalTick);
    }
};
