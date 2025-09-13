#pragma once

#include <cstdint>
#include "../Event.hpp"
#include <SDK/Client/GUI/Screens/Controllers/ChatScreenController.hpp>

class ChatScreenControllerTickEvent : public Event {
private:
    ChatScreenController* controller;
public:
    ChatScreenController* getChatScreenController() {
        return this->controller;
    }

    explicit ChatScreenControllerTickEvent(ChatScreenController* controller) : controller(controller) {}
};