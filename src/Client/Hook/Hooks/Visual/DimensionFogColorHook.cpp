#include "DimensionFogColorHook.hpp"

MCCColor& DimensionFogColorHook::DimensionFogColorCallback(Dimension* _this, MCCColor& result,
    MCCColor const& baseColor, float brightness)
{

    auto event = nes::make_holder<FogColorEvent>(funcOriginal(_this, result, baseColor, brightness));
    eventMgr.trigger(event);

    result = event->getFogColor();
    return result;
}

DimensionFogColorHook::DimensionFogColorHook(): Hook("Fog Color Hook", GET_SIG_ADDRESS("Dimension::getBrightnessDependentFogColor"))
{}

void DimensionFogColorHook::enableHook()
{

    this->autoHook( (void *) DimensionFogColorCallback, (void **) &funcOriginal);

}
