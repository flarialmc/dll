#pragma once

#include "../Event.hpp"

class displayClientMessageEvent : public Event {
private:
    std::string message;
public:
    explicit displayClientMessageEvent(std::string message) {
        this->message = message;
    }
};