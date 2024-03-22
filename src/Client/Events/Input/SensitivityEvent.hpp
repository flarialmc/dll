#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class SensitivityEvent : public Event, Cancellable {
private:
    float sensitivity;
public:
    [[nodiscard]] float getSensitivity() const {
        return this->sensitivity;
    }

    void setSensitivity(float e) {
        this->sensitivity = e;
    }

    explicit SensitivityEvent(float e) {
        this->sensitivity = e;
    }
};
