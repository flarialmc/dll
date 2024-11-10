#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class UIControlGetPositionEvent : public Event {
private:
    UIControl* control;
    Vec2<float>* position;
public:
    void setPosition(Vec2<float> newPos) {
        if(position == nullptr) return;
        *this->position = newPos;
    }

    Vec2<float>* getPosition() {
        return this->position;
    }

    UIControl* getControl() {
        return this->control;
    }

    explicit UIControlGetPositionEvent(UIControl* control, Vec2<float>* position) : control(control), position(position) {}
};

