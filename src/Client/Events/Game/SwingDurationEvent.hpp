#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class SwingDurationEvent : public Event, public Cancellable {
private:
    int duration;
public:
    [[nodiscard]] int getDuration() const {
        return this->duration;
    }

    void setDuration(int d) {
        this->duration = d;
    }

    explicit SwingDurationEvent(int duration) : duration(duration) {}
};