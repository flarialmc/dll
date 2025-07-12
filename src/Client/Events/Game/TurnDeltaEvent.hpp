#pragma once

#include "../Event.hpp"

class TurnDeltaEvent : public Event {

public:
    void* a1;
    Vec2<float>& delta;

    TurnDeltaEvent(void* a, Vec2<float>& a2)
        : a1(a), delta(a2) {
    }
};