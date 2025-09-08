#include "NoHurtCam.hpp"

#include "Events/EventManager.hpp"


void NoHurtCam::onEnable() {
    Listen(this, RaknetTickEvent, &NoHurtCam::onRaknetTick)
    Listen(this, TickEvent, &NoHurtCam::onTick)
    Module::onEnable();
}

void NoHurtCam::onDisable() {
    if (patched) unpatch();
    Deafen(this, RaknetTickEvent, &NoHurtCam::onRaknetTick)
    Deafen(this, TickEvent, &NoHurtCam::onTick)
    Module::onDisable();
}

void NoHurtCam::defaultConfig() {
    Module::defaultConfig("core");
}

void NoHurtCam::patch() {
    if (patched) return;
    patched = true;
    int size;
    if (VersionUtils::checkAboveOrEqual(21, 70)) {
        size = 1;
    } else if (VersionUtils::checkAboveOrEqual(21, 30)) {
        size = 5;
    } else {
        size = 3;
    }
    if (VersionUtils::checkAboveOrEqual(21, 70)) {
        std::byte jump{0xEB};
        Memory::patchBytes((LPVOID) sigOffset, &jump, size);
    } else {
        Memory::nopBytes((LPVOID) sigOffset, size);
    }
}

void NoHurtCam::unpatch() {
    if (!patched) return;
    patched = false;
    int size;
    if (VersionUtils::checkAboveOrEqual(21, 70)) {
        size = 1;
    } else if (VersionUtils::checkAboveOrEqual(21, 30)) {
        size = 5;
    } else {
        size = 3;
    }
    Memory::patchBytes((LPVOID) sigOffset, originalCameraAngle.data(), size);
}

void NoHurtCam::onRaknetTick(RaknetTickEvent &event) {
    if (this->isEnabled()) {
        std::string serverIP = SDK::getServerIP();
        if (serverIP.find("hive") != std::string::npos) {
            if (!this->restricted) {
                FlarialGUI::Notify("Can't use No Hurt Cam on " + serverIP); // TODO: move restrictions to API
                this->restricted = true;
            }
        } else {
            this->restricted = false;
        }
    }
}

void NoHurtCam::onTick(TickEvent &event) {
    if (!this->restricted) {
        patch();
    } else {
        unpatch();
    }
}
