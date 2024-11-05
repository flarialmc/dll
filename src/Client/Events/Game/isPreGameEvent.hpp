#pragma once

#include "../Event.hpp"

class isPreGameEvent : public Event {
private:
    bool state = false;
public:
    isPreGameEvent(bool currentState);
    void setState(bool newState);
    bool getState();
};
