#include "Sneak.hpp"

#include "Events/EventManager.hpp"

Sneak::Sneak(): Module("Toggle Sneak", "No need to hold down your sneak key.", IDR_SLOWLY_PNG, "SHIFT")
{

    

}

void Sneak::onEnable()
{
    Listen(this, KeyEvent, &Sneak::onKey)
    Listen(this, TickEvent, &Sneak::onTick)
    Module::onEnable();
}

void Sneak::onDisable()
{
    Deafen(this, KeyEvent, &Sneak::onKey)
    Deafen(this, TickEvent, &Sneak::onTick)
    Module::onDisable();
}

void Sneak::defaultConfig()
{
    getKeybind();
    Module::defaultConfig("core");
    setDef("status", false);
    setDef("textscale", 0.80f);
    setDef("always", false);
    
}

void Sneak::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Toggle Sneak");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Sneak::onKey(KeyEvent& event)
{ // TODO: it lets sneak key up through (flickers sneak once)
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
        toggleSneaking = !toggleSneaking;
    }
}

void Sneak::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
            if (toggleSneaking) {
                handler->sneaking = true;
                toggled = true;
            }
            if (!toggleSneaking and toggled) {
                handler->sneaking = false;
                toggled = false;
            }
        }
    }
}
