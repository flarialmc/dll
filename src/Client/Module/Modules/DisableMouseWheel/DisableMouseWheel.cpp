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
    initSettingsPage();

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
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) &&
        this->isKeyPartOfKeybind(event.key) &&
        event.getAction() == ActionType::Pressed
    ) {
        disableScrolling = !disableScrolling;
    }
}
