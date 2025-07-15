#pragma once

#include "../Event.hpp"
#include "../../GUI/D2D.hpp"
#include "../../Utils/Utils.hpp"
#include "SDK/Client/Render/BaseActorRenderContext.hpp"

class ActorShaderParamsEvent : public Event {
    MCCColor overlay;
    MCCColor changeColor;
    MCCColor glintColor;
public:
    [[nodiscard]] MCCColor *getGlintColor() { return &this->glintColor; }
    [[nodiscard]] MCCColor *getOverlay() { return &this->overlay; }
    [[nodiscard]] MCCColor *getChangeColor() { return &this->changeColor; }

    void setGlintColor(MCCColor c) { this->glintColor = c; }
    void setOverlay(MCCColor o) { this->overlay = o; }
    void setChangeColor(MCCColor c) { this->changeColor = c; }

    void setGlintColorFromD2D(D2D1_COLOR_F &c, float alpha) {
        this->glintColor.r = c.r * alpha;
        this->glintColor.g = c.g * alpha;
        this->glintColor.b = c.b * alpha;
        this->glintColor.a = alpha;
    }

    explicit ActorShaderParamsEvent(
        MCCColor *overlay,
        MCCColor *changeColor,
        MCCColor *glintColor
    ): overlay(*overlay), changeColor(*changeColor), glintColor(*glintColor) {
    }
};
