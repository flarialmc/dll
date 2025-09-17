#include "OverworldFogColorHook.hpp"
#include "../../../../SDK/Client/Level/Dimension.hpp"

MCCColor& OverworldFogColorHook::OverworldFogColorCallback(Dimension* _this, MCCColor& result,
    MCCColor const& baseColor, float brightness)
{

    auto event = nes::make_holder<FogColorEvent>(funcOriginal(_this, result, baseColor, brightness));
    eventMgr.trigger(event);

    return event->getFogColor();
}

OverworldFogColorHook::OverworldFogColorHook(): Hook("Overworld Fog Color Hook", GET_SIG_ADDRESS("OverworldDimension::getBrightnessDependentFogColor"))
{}

void OverworldFogColorHook::enableHook()
{

    this->autoHook((void *) OverworldFogColorCallback, (void **) &funcOriginal);

}
