#include "UpsideDown.hpp"

#include "Events/EventManager.hpp"

UpsideDown::UpsideDown(): Module("Upside Down", "No need to flip your monitor!!", IDR_UPSIDE_DOWN_PNG, "")
{
    Module::setup();
}

void UpsideDown::onEnable()
{
    Listen(this, FOVEvent, &UpsideDown::onGetFOV)
    Module::onEnable();
}

void UpsideDown::onDisable()
{
    Deafen(this, FOVEvent, &UpsideDown::onGetFOV)
    Module::onDisable();
}

void UpsideDown::onGetFOV(FOVEvent& event)
{
    if (!this->isEnabled()) return;
    auto fov = event.getFOV();
    fov = 360 - fov;
    event.setFOV(fov);
}

void UpsideDown::defaultConfig()
{
    Module::defaultConfig("core");
}
