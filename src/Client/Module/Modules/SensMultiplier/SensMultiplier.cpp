#include "SensMultiplier.hpp"

void SensMultiplier::onEnable() {
    Listen(this, TurnDeltaEvent, &SensMultiplier::onTurnDeltaEvent)
    Module::onEnable();
}

void SensMultiplier::onDisable() {
    Deafen(this, TurnDeltaEvent, &SensMultiplier::onTurnDeltaEvent)
    Module::onDisable();
}

void SensMultiplier::defaultConfig() {
    Module::defaultConfig("core");
    setDef("sensitivity", 1.0f);
}

void SensMultiplier::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSlider("Sensitivity Multiplier", "Multiplies your mouse sensitivity by this amount.", "sensitivity", 8.0f, 0.1f, false);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void SensMultiplier::onTurnDeltaEvent(TurnDeltaEvent &event) {
    if (this->isEnabled()) {
        float multiplier = getOps<float>("sensitivity");
        event.delta.x *= multiplier;
        event.delta.y *= multiplier;
    }
}