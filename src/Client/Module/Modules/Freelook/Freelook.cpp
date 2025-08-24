#include "Freelook.hpp"

#include "Events/EventManager.hpp"

void FreeLook::onSetup() {
    keybindActions.clear();

    //enable action (key press)
    keybindActions.push_back([this](std::vector<std::any> args) -> std::any {

        std::string serverIP = SDK::getServerIP();
        if ((serverIP.find("hive") != std::string::npos ||
             serverIP.find("galaxite") != std::string::npos ||
             serverIP.find("venity") != std::string::npos)) { // TODO: make it only show once per server switch?
            FlarialGUI::Notify("Can't use freelook on " + serverIP); // TODO: move restrictions to API
            this->restricted = true;
        }
        else {
            this->restricted = false;
        }
        if (!this->restricted) {
            if (getOps<bool>("toggle")) {
                if (!this->active) {
                    patch();
                }
                else {
                    unpatch();
                }
            }
            else {
                patch();
            }
        }
        else {
            unpatch(); // module restricted
        }
        return {};


    });


    //disable action (key release)
    keybindActions.push_back([this](std::vector<std::any> args) -> std::any {

        if (!getOps<bool>("toggle"))
            unpatch();

        return {};

    });
}

void FreeLook::onEnable() {
    Listen(this, PerspectiveEvent, &FreeLook::onGetViewPerspective)
    Listen(this, UpdatePlayerEvent, &FreeLook::onUpdatePlayer)
    Listen(this, KeyEvent, &FreeLook::onKey)
    Listen(this, MouseEvent, &FreeLook::onMouse)
    Module::onEnable();

}

void FreeLook::onDisable() {
    unpatch();
    this->active = false;
    Deafen(this, PerspectiveEvent, &FreeLook::onGetViewPerspective)
    Deafen(this, UpdatePlayerEvent, &FreeLook::onUpdatePlayer)
    Deafen(this, KeyEvent, &FreeLook::onKey)
    Deafen(this, MouseEvent, &FreeLook::onMouse)
    Module::onDisable();
}

void FreeLook::patch() {
    if (this->active) return;
    this->active = true;
    Memory::patchBytes((void*)yaw1, nop, 4);
    Memory::patchBytes((void*)yaw2, nop, 4);
    if (!VersionUtils::checkAboveOrEqual(21, 40)) {
        Memory::patchBytes((void*)pitch, nop, 4);
        Memory::patchBytes((void*)movement, nop, 4);
    }
}

void FreeLook::unpatch() {
    if (!this->active) return;
    this->active = false;
    Memory::patchBytes((void*)yaw1, originalYaw1.data(), 4);
    Memory::patchBytes((void*)yaw2, originalYaw2.data(), 4);
    if (!VersionUtils::checkAboveOrEqual(21, 40)) {
        Memory::patchBytes((void*)pitch, originalPitch.data(), 4);
        Memory::patchBytes((void*)movement, originalMovement.data(), 4);
    }
}

void FreeLook::defaultConfig() {
    getKeybind();
    Module::defaultConfig("core");
    setDef("toggle", false);
    setDef("mode", (std::string)"3rd Person back");
    
}

void FreeLook::settingsRender(float settingsOffset) {

    initSettingsPage();

    addHeader("Freelook");
    addKeybind("Freelook Keybind", "Hold for 2 seconds!", "keybind", true);
    addToggle("Toggleable Mode", "Click to toggle or Hold to keep enabled", "toggle");
    addDropdown("Freelook View Mode", "",std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, "mode", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void FreeLook::onKey(KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeyPartOfKeybind(event.key) && (SDK::getCurrentScreen() == "hud_screen" || SDK::getCurrentScreen() == "f3_screen" || SDK::getCurrentScreen() == "zoom_screen")) {
        if (this->isKeybind(event.keys)) { // key is defo pressed
            keybindActions[0]({});
        }
        else { // key released
            keybindActions[1]({});
        }
    }

}

void FreeLook::onMouse(MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press) keybindActions[0]({});
    else if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Release) keybindActions[1]({});
}

void FreeLook::onUpdatePlayer(UpdatePlayerEvent& event) {
    if (this->active) {
        event.cancel();
    }
}

void FreeLook::onGetViewPerspective(PerspectiveEvent &event) {
    if (!this->isEnabled()) return;
    if (this->active) {
        std::string setting = getOps<std::string>("mode");
        // TODO: Let use F5 (perspective switch key)
        if (setting == "1st Person") {
            event.setPerspective(Perspective::FirstPerson);
        }
        if (setting == "3rd Person back") {
            event.setPerspective(Perspective::ThirdPersonBack);
        }
        if (setting == "3rd Person front") {
            event.setPerspective(Perspective::ThirdPersonFront);
        }
    }
}