#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Events/Render/FogColorEvent.hpp"
#include "SDK/Client/Level/Dimension.hpp"

class DimensionFogColorHook : public Hook {
private:

    static MCCColor &DimensionFogColorCallback(Dimension *_this, MCCColor &result, MCCColor const &baseColor, float brightness);

public:
    typedef MCCColor &(__thiscall *dimensionFogColorOriginal)(Dimension *, MCCColor &, MCCColor const &, float);

    static inline dimensionFogColorOriginal funcOriginal = nullptr;

    DimensionFogColorHook();

    void enableHook() override;
};


