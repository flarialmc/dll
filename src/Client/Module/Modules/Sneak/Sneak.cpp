#include "Sneak.hpp"

#include "Events/EventManager.hpp"

void Sneak::onEnable() {
    Listen(this, KeyEvent, &Sneak::onKey)
    Listen(this, MouseEvent, &Sneak::onMouse)
    Listen(this, TickEvent, &Sneak::onTick)
    Module::onEnable();
}

void Sneak::onDisable() {
    Deafen(this, KeyEvent, &Sneak::onKey)
    Deafen(this, MouseEvent, &Sneak::onMouse)
    Deafen(this, TickEvent, &Sneak::onTick)
    Module::onDisable();
}

void Sneak::defaultConfig() {
    getKeybind();
    Module::defaultConfig("core");
    setDef("status", false);
    setDef("textscale", 0.80f);
    setDef("always", false);
    
}

void Sneak::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Toggle Sneak");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Sneak::onKey(KeyEvent& event) {
    // TODO: it lets sneak key up through (flickers sneak once)
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) {
        toggleSneaking = !toggleSneaking;
    }
}

void Sneak::onMouse(MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press) {
        toggleSneaking = !toggleSneaking;
    }
}

void Sneak::onTick(TickEvent& event) {
    if (!this->isEnabled()) return;
    if (SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
            if (toggleSneaking) {
                handler->sneaking = true;
                handler->mInputState.mSneakDown = true;
                handler->mRawInputState.mSneakDown = true;
                toggled = true;
            }
            if (!toggleSneaking and toggled) {
                handler->sneaking = false;
                handler->mInputState.mSneakDown = false;
                handler->mRawInputState.mSneakDown = false;
                toggled = false;
            }
        }
    }
}
