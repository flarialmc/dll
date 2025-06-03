#pragma once

#include <cstdint>
#include "../Event.hpp"
#include <SDK/Client/GUI/Screens/Controllers/ContainerScreenController.hpp>

class ContainerScreenControllerTickEvent : public Event {
private:
    ContainerScreenController* controller;
public:
    ContainerScreenController* getContainerScreenController() {
        return this->controller;
    }

    explicit ContainerScreenControllerTickEvent(ContainerScreenController* controller) : controller(controller) {}
};  