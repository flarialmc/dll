#include "DisableMouseWheel.hpp"



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
    initSettingsPage();

    addKeybind("Keybind", "", "keybind", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void DisableMouseWheel::onMouse(MouseEvent &event) {
    if (
        this->isEnabled() &&
        event.getButton() == MouseButton::Scroll &&
        disableScrolling &&
        (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "zoom_screen" || SDK::getCurrentScreen() == "f3_screen")
    )
        event.setCancelled(true);
}

void DisableMouseWheel::onKey(KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) &&
        this->isKeyPartOfKeybind(event.key) &&
        event.getAction() == ActionType::Pressed
    ) {
        disableScrolling = !disableScrolling;
    }
}
