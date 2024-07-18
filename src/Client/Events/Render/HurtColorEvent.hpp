#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../GUI/Data.hpp"

class HurtColorEvent : public Event, Cancellable {
private:
    MCCColor* hurtColor;
public:
    [[nodiscard]] MCCColor* getHurtColor() const {
        return this->hurtColor;
    }

    void setHurtColor(MCCColor* e) {
        this->hurtColor = e;
    }

    void setHurtColorFromD2DColor(D2D1_COLOR_F& e, float alpha) {
        this->hurtColor->r = e.r;
        this->hurtColor->g = e.g;
        this->hurtColor->b = e.b;
        this->hurtColor->a = alpha;
    }

    explicit HurtColorEvent(MCCColor* hurtColor) : hurtColor(hurtColor) {}
};

