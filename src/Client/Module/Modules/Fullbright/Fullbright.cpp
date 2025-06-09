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
    if (ModuleManager::initialized) Client::SaveSettings();
}

void Fullbright::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Fullbright");
    addSlider("Brightness", "", "gamma", 25.0f);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void Fullbright::onGetGamma(GammaEvent &event) {
    if (!this->isEnabled()) return;
    event.setGamma(getOps<float>("gamma"));
}