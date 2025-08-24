#include "FOVChanger.hpp"

void FOVChanger::onEnable() {
    Listen(this, FOVEvent, &FOVChanger::onGetFOV)
    Module::onEnable();
}

void FOVChanger::onDisable() {
    Deafen(this, FOVEvent, &FOVChanger::onGetFOV)
    Module::onDisable();
}

void FOVChanger::defaultConfig() {
    Module::defaultConfig("core");
    setDef("fovvalue", 60.00f);
    setDef("fovaffectshand", false);
}

void FOVChanger::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("FOV Changer");
    addSlider("FOV Value", "", "fovvalue", 359.0f, 0, false);
    addToggle("Affect Hand Size", "Keep normal hand size or not.", "fovaffectshand");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void FOVChanger::onGetFOV(FOVEvent &event) {
    if (!this->isEnabled()) return;
    if (auto zoom = ModuleManager::getModule("Zoom"); zoom && zoom->active) return;
    if (!getOps<bool>("fovaffectshand")) {
        if (event.getFOV() == 70) return;
    }

    bool inserver;

    std::string serverIP = SDK::getServerIP();

    if (serverIP.find("world") != std::string::npos) inserver = true;
    else inserver = false;

    auto fovSetting = getOps<float>("fovvalue");

    if (inserver) {
        if (fovSetting > 150) {
            if (!notified150Fov) {
                FlarialGUI::Notify("FOV Changer has been limmited to 150 on servers.");
                notified150Fov = true;
            }
            event.setFOV(150.0f);
        }
    }
    event.setFOV(fovSetting);
}
