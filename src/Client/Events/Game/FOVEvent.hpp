#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class FOVEvent : public Event, Cancellable {
private:
    float fov;
public:
    float getFOV() {
        return this->fov;
    }

    void setFOV(float e) {
        this->fov = e;
    }

    explicit FOVEvent(float fov) : fov(fov) {}
};