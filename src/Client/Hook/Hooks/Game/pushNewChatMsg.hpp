#pragma once
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class pushNewChatMsgHook : public Hook {
private:
    static void pushNewChatMsg(void* hudScreenController, std::string message, float time) {

        auto event = nes::make_holder<pushNewChatMsgEvent>(message, time);
        eventMgr.trigger(event);

        funcOriginal(hudScreenController, message, time);
    }

public:
    typedef void(__thiscall* original)(void* hudScreenController, std::string message, float time);

    static inline original funcOriginal = nullptr;

    pushNewChatMsgHook() : Hook("pushNewChatMsgHook", GET_SIG_ADDRESS("HudScreenController::_pushNewChatMessage")) {}

    void enableHook() override {
        this->autoHook((void*)pushNewChatMsg, (void**)&funcOriginal);
    }
};