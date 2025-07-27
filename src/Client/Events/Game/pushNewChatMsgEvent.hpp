#pragma once

#include "../Event.hpp"

class pushNewChatMsgEvent : public Event {
private:
    std::string message;
    float time;
public:
    explicit pushNewChatMsgEvent(std::string message, float time) {
        this->message = message;
        this->time = time;
    }
    std::string getMessage() {
        return message;
    }

    float getTime() {
        return time;
    }
};