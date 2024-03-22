#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../GUI/D2D.hpp"

class FogColorEvent : public Event, Cancellable {
private:
    MCCColor& fogColor;
public:
    [[nodiscard]] MCCColor& getFogColor() const {
        return this->fogColor;
    }

    void setFogColor(MCCColor& e) {
        this->fogColor = e;
    }

    void setFogColorFromD2DColor(D2D1_COLOR_F& e, float alpha) {
        this->fogColor.r = e.r;
        this->fogColor.g = e.g;
        this->fogColor.b = e.b;
        this->fogColor.a = alpha;
    }

    explicit FogColorEvent(MCCColor &fogColor) : fogColor(fogColor) {}
};

