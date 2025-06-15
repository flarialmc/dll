#include "SpeedDisplay.hpp"

SpeedDisplay::SpeedDisplay(): Module("Speed Display", "Displays your current travel speed in blocks/second.",
                                     IDR_SPEED_PNG, "")
{
    
}

void SpeedDisplay::onEnable()
{
    Listen(this, RenderEvent, &SpeedDisplay::onRender)
    Listen(this, TickEvent, &SpeedDisplay::onTick)
    Module::onEnable();
}

void SpeedDisplay::onDisable()
{
    Deafen(this, RenderEvent, &SpeedDisplay::onRender)
    Deafen(this, TickEvent, &SpeedDisplay::onTick)
    Module::onDisable();
}

void SpeedDisplay::defaultConfig()
{
    setDef("text", (std::string)"{value} m/s");
    Module::defaultConfig("all");
    if (ModuleManager::initialized) Client::SaveSettings();
}

void SpeedDisplay::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Speed Display");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void SpeedDisplay::onRender(RenderEvent& event)
{
    if (!this->isEnabled()) return;
    this->normalRender(15, speed);
}

void SpeedDisplay::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance->getLocalPlayer())
        return;
    auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
    if (stateVectorComponent != nullptr) {
        speed = std::format("{:.2f}", stateVectorComponent->Pos.dist(PrevPos) * 20);
        PrevPos = stateVectorComponent->Pos;
    }
}
