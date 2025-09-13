#include "UpsideDown.hpp"
#include "Client.hpp"



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
