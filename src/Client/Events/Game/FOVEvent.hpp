#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class FOVEvent : public Event, public Cancellable {
private:
    float fov;
public:
    [[nodiscard]] float getFOV() const {
        return this->fov;
    }

    void setFOV(float e) {
        this->fov = e;
    }

    explicit FOVEvent(float fov) : fov(fov) {}
};