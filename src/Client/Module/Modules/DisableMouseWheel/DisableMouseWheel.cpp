#include "DisableMouseWheel.hpp"

#include "Events/EventManager.hpp"

void DisableMouseWheel::onEnable() {
    Listen(this, MouseEvent, &DisableMouseWheel::onMouse)
    Listen(this, KeyEvent, &DisableMouseWheel::onKey)
    Module::onEnable();
}

void DisableMouseWheel::onDisable() {
    Deafen(this, MouseEvent, &DisableMouseWheel::onMouse)
    Deafen(this, KeyEvent, &DisableMouseWheel::onKey)
    Module::onDisable();
}

void DisableMouseWheel::defaultConfig() {
    getKeybind();
    Module::defaultConfig("core");
}

void DisableMouseWheel::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Disable Mouse Wheel");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void DisableMouseWheel::onMouse(MouseEvent &event) {
    if (this->isEnabled() && event.getButton() == MouseButton::Scroll && SDK::getCurrentScreen() == "hud_screen" && disableScrolling)
        event.setCancelled(true);
}

void DisableMouseWheel::onKey(KeyEvent &event) {
    if (this->isKeybind(event.keys) &&
        this->isKeyPartOfKeybind(event.key) &&
        event.getAction() == ActionType::Pressed
    ) {
        disableScrolling = !disableScrolling;
    }
}
