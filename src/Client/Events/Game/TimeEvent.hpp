#pragma once

#include "../Event.hpp"

class TimeEvent : public Event {
private:
    float time;
public:
    explicit TimeEvent(float time) : time(time) {}
    [[nodiscard]] float getTime() const { return time; }
    void setTime(float newTime) { this->time = newTime; }
};