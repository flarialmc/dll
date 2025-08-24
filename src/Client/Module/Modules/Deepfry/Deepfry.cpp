#include "Deepfry.hpp"

class RenderEvent;

void Deepfry::onEnable() {
    Listen(this, RenderEvent, &Deepfry::onRender)
    Module::onEnable();
}

void Deepfry::onDisable() {
    Deafen(this, RenderEvent, &Deepfry::onRender)
    Module::onDisable();
}

void Deepfry::defaultConfig() {
    Module::defaultConfig("core");
    setDef("paint", false);
    
}

void Deepfry::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Deepfry");
    addToggle("Paint Effect", "", "paint");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void Deepfry::onRender(RenderEvent& event) {
    if (this->isEnabled()) {
        if (getOps<bool>("paint")) FlarialGUI::ApplyPaintEffect(10.0f);
        else FlarialGUI::ApplyCombinedDeepFry();

    }
}
