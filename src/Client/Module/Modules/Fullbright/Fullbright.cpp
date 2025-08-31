#include "Fullbright.hpp"

#include "Events/EventManager.hpp"

void Fullbright::onEnable() {
    Listen(this, GammaEvent, &Fullbright::onGetGamma)
    Module::onEnable();
}

void Fullbright::onDisable() {
    Deafen(this, GammaEvent, &Fullbright::onGetGamma)
    Module::onDisable();
}

void Fullbright::defaultConfig() {
    Module::defaultConfig("core");
    setDef("gamma", 25.f);
    
}

void Fullbright::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Fullbright");
    addSlider("Brightness", "", "gamma", 25.0f);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void Fullbright::onGetGamma(GammaEvent &event) {
    if (!this->isEnabled()) return;
    event.setGamma(getOps<float>("gamma"));
}