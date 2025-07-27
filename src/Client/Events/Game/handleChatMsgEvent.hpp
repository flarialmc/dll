#pragma once

#include "../Event.hpp"

class handleChatMsgEvent : public Event {
private:
    std::string message;
    std::string ttsMessage;
public:
    explicit handleChatMsgEvent(std::string message, std::string ttsMessage) {
        this->message = message;
        this->ttsMessage = ttsMessage;
    }
    std::string getMessage() {
        return message;
    }

    std::string getTTSMessage() {
        return ttsMessage;
    }
};