#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

class GammaEvent : public Event, Cancellable {
private:
    float gamma;
public:
    [[nodiscard]] float getGamma() const {
        return this->gamma;
    }

    void setGamma(float e) {
        this->gamma = e;
    }

    explicit GammaEvent(float gamma) : gamma(gamma) {}
};

