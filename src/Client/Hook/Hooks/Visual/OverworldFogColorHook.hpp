#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class OverworldFogColorHook : public Hook {
private:

    static MCCColor &
    OverworldFogColorCallback(Dimension *_this, MCCColor &result, MCCColor const &baseColor, float brightness) {


        FogColorEvent event(funcOriginal(_this, result, baseColor, brightness));
        EventHandler::onGetFogColor(event);

        return event.getFogColor();

    }

public:
    typedef MCCColor &(__thiscall *OverworldFogColorOriginal)(Dimension *, MCCColor &, MCCColor const &, float);

    static inline OverworldFogColorOriginal funcOriginal = nullptr;

    OverworldFogColorHook() : Hook("Overworld Fog Color Hook",
                                   "41 0F 10 08 48 8B C2 0F 28 D3 F3 0F 59 1D ? ? ? ? F3 0F 59 15 ? ? ? ? F3 0F 58 1D ? ? ? ? 0F 11 0A F3 0F 58 15 ? ? ? ? F3 0F 59 5A ?") {}

    void enableHook() override {

        this->autoHook((void *) OverworldFogColorCallback, (void **) &funcOriginal);

    }
};


