#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class OverworldFogColorHook : public Hook {
private:

    static MCCColor &
    OverworldFogColorCallback(Dimension *_this, MCCColor &result, MCCColor const &baseColor, float brightness) {

        auto event = nes::make_holder<FogColorEvent>(funcOriginal(_this, result, baseColor, brightness));
        eventMgr.trigger(event);

        return event->getFogColor();
    }

public:
    typedef MCCColor &(__thiscall *OverworldFogColorOriginal)(Dimension *, MCCColor &, MCCColor const &, float);

    static inline OverworldFogColorOriginal funcOriginal = nullptr;

    OverworldFogColorHook() : Hook("Overworld Fog Color Hook",GET_SIG_ADDRESS("OverworldDimension::getBrightnessDependentFogColor")) {}

    void enableHook() override {

        this->autoHook((void *) OverworldFogColorCallback, (void **) &funcOriginal);

    }
};


