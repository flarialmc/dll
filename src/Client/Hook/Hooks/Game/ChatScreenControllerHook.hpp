#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/Game/ChatScreenControllerTickEvent.hpp"

class ChatScreenControllerHook : public Hook {
private:

    static uint32_t ChatScreenController__tick(ChatScreenController *_this) {
        auto event = nes::make_holder<ChatScreenControllerTickEvent>(_this);
        eventMgr.trigger(event);

        return funcOriginalTick(_this); // <----- Crashes right here!!!!
    }

public:

    typedef uint32_t(__thiscall *originalTick)(ChatScreenController *);

    static inline originalTick funcOriginalTick = nullptr;

    ChatScreenControllerHook() : Hook("ChatScreenControllerHooks", 0) {}

    void enableHook() override {
        auto addressTick = GET_SIG_ADDRESS("ChatScreenController::tick");
        this->manualHook((void *) addressTick, (void *) ChatScreenController__tick, (void **) &funcOriginalTick);
    }
};
