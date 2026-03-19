#include "SpeedDisplay.hpp"

void SpeedDisplay::onEnable()
{
    HUDModule::onEnable();
    Listen(this, TickEvent, &SpeedDisplay::onTick)
}

void SpeedDisplay::onDisable()
{
    Deafen(this, TickEvent, &SpeedDisplay::onTick)
    HUDModule::onDisable();
}

std::string SpeedDisplay::getDisplayValue()
{
    return speed;
}

void SpeedDisplay::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer())
        return;
    auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
    if (stateVectorComponent != nullptr) {
        speed = std::format("{:.2f}", stateVectorComponent->Pos.dist(PrevPos) * 20);
        PrevPos = stateVectorComponent->Pos;
    }
}
