#pragma once
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class handleChatMsgHook : public Hook {
private:
    static void handleChatMsg(ChatScreenController* chatScreenController, std::string message, std::string ttsMessage) {

        auto event = nes::make_holder<handleChatMsgEvent>(message, ttsMessage);
        eventMgr.trigger(event);

        funcOriginal(chatScreenController, message, ttsMessage);
    }

public:
    typedef void(__thiscall* original)(ChatScreenController* chatScreenController, std::string message, std::string ttsMessage);

    static inline original funcOriginal = nullptr;

    handleChatMsgHook() : Hook("handleChatMsgHook", GET_SIG_ADDRESS("ChatScreenController::_handleChatMessage")) {}

    void enableHook() override {
        this->autoHook((void*)handleChatMsg, (void**)&funcOriginal);
    }
};