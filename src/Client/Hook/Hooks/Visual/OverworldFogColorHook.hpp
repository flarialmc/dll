#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class OverworldFogColorHook : public Hook {
private:

    static MCCColor &
    OverworldFogColorCallback(Dimension *_this, MCCColor &result, MCCColor const &baseColor, float brightness);

public:
    typedef MCCColor &(__thiscall *OverworldFogColorOriginal)(Dimension *, MCCColor &, MCCColor const &, float);

    static inline OverworldFogColorOriginal funcOriginal = nullptr;

    OverworldFogColorHook();

    void enableHook() override;
};


