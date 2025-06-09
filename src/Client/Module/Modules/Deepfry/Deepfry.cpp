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
    if (ModuleManager::initialized) Client::SaveSettings();
}

void Deepfry::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

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
