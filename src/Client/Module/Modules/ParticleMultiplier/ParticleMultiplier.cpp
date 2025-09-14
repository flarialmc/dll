#include "ParticleMultiplier.hpp"
#include "Events/EventManager.hpp"
#include "GUI/Engine/EngineImGui.hpp"


void ParticleMultiplier::onEnable() {
    Listen(this, RenderEvent, &ParticleMultiplier::onRender)
    Module::onEnable();
}

void ParticleMultiplier::onDisable() {
    Deafen(this, RenderEvent, &ParticleMultiplier::onRender)
    Module::onDisable();
}

void ParticleMultiplier::defaultConfig() {
    Module::defaultConfig("core");
    setDef("intensity", 50.f);
}

void ParticleMultiplier::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSlider("Intensity", "", "intensity", 500.f);
    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ParticleMultiplier::onRender(RenderEvent &event) {
}
