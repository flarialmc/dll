#pragma once

#include "../Event.hpp"
#include "../../GUI/D2D.hpp"
#include "../../Utils/Utils.hpp"
#include "SDK/Client/Render/BaseActorRenderContext.hpp"

class ActorShaderParamsEvent : public Event {
    Actor *entity;
    MCCColor *overlay;
    MCCColor *changeColor;
    MCCColor *glintColor;
    unsigned char *lightEmission;
public:
    [[nodiscard]] MCCColor* getGlintColor() const { return this->glintColor; }
    [[nodiscard]] Actor* getEntity() const { return this->entity; }
    [[nodiscard]] MCCColor* getOverlay() const { return this->overlay; }
    [[nodiscard]] MCCColor* getChangeColor() const { return this->changeColor; }
    [[nodiscard]] unsigned char* getLightEmission() const { return this->lightEmission; }

    void setGlintColor(MCCColor* c) { this->glintColor = c; }
    void setOverlay(MCCColor* o) { this->overlay = o; }
    void setChangeColor(MCCColor* c) { this->changeColor = c; }
    void setLightEmission(unsigned char* e) { this->lightEmission = e; }

    void setGlintColorFromD2D(D2D1_COLOR_F& c, float alpha) {
        this->glintColor->r = c.r * alpha;
        this->glintColor->g = c.g * alpha;
        this->glintColor->b = c.b * alpha;
        this->glintColor->a = alpha;
    }

    explicit ActorShaderParamsEvent(
        Actor *entity,
        MCCColor *overlay, MCCColor *changeColor, MCCColor *glintColor,
        unsigned char *lightEmission
    ) {
        this->entity = entity; this->overlay = overlay; this->changeColor = changeColor; this->glintColor = glintColor; this->lightEmission = lightEmission;
    }
};

