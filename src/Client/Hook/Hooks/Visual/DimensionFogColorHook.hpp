#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Events/Render/FogColorEvent.hpp"

class DimensionFogColorHook : public Hook {
private:

    static MCCColor &DimensionFogColorCallback(Dimension *_this, MCCColor &result, MCCColor const &baseColor, float brightness) {

        auto event = nes::make_holder<FogColorEvent>(funcOriginal(_this, result, baseColor, brightness));
        eventMgr.trigger(event);

        return event->getFogColor();
    }

public:
    typedef MCCColor &(__thiscall *dimensionFogColorOriginal)(Dimension *, MCCColor &, MCCColor const &, float);

    static inline dimensionFogColorOriginal funcOriginal = nullptr;

    DimensionFogColorHook() : Hook("Fog Color Hook", GET_SIG_ADDRESS("Dimension::getBrightnessDependentFogColor")) {}

    void enableHook() override {

        this->autoHook( (void *) DimensionFogColorCallback, (void **) &funcOriginal);

    }
};


