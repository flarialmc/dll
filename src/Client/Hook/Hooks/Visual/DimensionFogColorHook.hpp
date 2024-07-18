#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Events/Render/FogColorEvent.hpp"

class Dimension {
public:
    float x = 0;
    float y = 0;
    float height = 0;
    float width = 0;
};

class DimensionFogColorHook : public Hook {
private:


    static MCCColor &
    DimensionFogColorCallback(Dimension *_this, MCCColor &result, MCCColor const &baseColor, float brightness) {

        FogColorEvent event(funcOriginal(_this, result, baseColor, brightness));
        EventHandler::onGetFogColor(event);

        return event.getFogColor();

    }

public:
    typedef MCCColor &(__thiscall *dimensionFogColorOriginal)(Dimension *, MCCColor &, MCCColor const &, float);

    static inline dimensionFogColorOriginal funcOriginal = nullptr;

    DimensionFogColorHook() : Hook("Fog Color Hook", GET_SIG("Dimension::getBrightnessDependentFogColor")) {}

    void enableHook() override {

        this->autoHook( (void *) DimensionFogColorCallback, (void **) &funcOriginal);

    }
};


