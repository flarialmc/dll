#pragma once

#include "../Module.hpp"

class FreeLook : public Module {
private:
    static inline uintptr_t yaw1;
    static inline uintptr_t yaw2;
    static inline uintptr_t pitch;
    static inline uintptr_t movement;

    static inline uint8_t nop[4] = {0x90, 0x90, 0x90, 0x90};
    // TODO: this might cause a crash if version is updated
    static inline std::array<std::byte, 4> originalYaw1;
    static inline std::array<std::byte, 4> originalYaw2;
    static inline std::array<std::byte, 4> originalPitch;
    static inline std::array<std::byte, 4> originalMovement;
public:

    FreeLook() : Module("FreeLook",
                        "Freely move your camera in 3rd person mode\nwhile keeping the player rotation the same.",
                        IDR_FREELOOK_PNG, "F") {

        yaw1 = GET_SIG_ADDRESS("CameraYaw");
        yaw2 = GET_SIG_ADDRESS("CameraYaw2");

        originalYaw1 = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) yaw1);
        originalYaw2 = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) yaw2);

        if(WinrtUtils::checkBelowOrEqual(21,30)) {
            pitch = GET_SIG_ADDRESS("CameraPitch");
            movement = GET_SIG_ADDRESS("CameraMovement");

            originalPitch = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) pitch);
            originalMovement = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) movement);
        }

        Module::setup();
    };

    void onEnable() override {
        Listen(this, PerspectiveEvent, &FreeLook::onGetViewPerspective)
        Listen(this, UpdatePlayerEvent, &FreeLook::onUpdatePlayer)
        Listen(this, KeyEvent, &FreeLook::onKey)
        Module::onEnable();

    }

    void onDisable() override {
        unpatch();
        this->active = false;
        Deafen(this, PerspectiveEvent, &FreeLook::onGetViewPerspective)
        Deafen(this, UpdatePlayerEvent, &FreeLook::onUpdatePlayer)
        Deafen(this, KeyEvent, &FreeLook::onKey)
        Module::onDisable();
    }

    void patch() {
        if(this->active) return;
        this->active = true;
        Memory::patchBytes((void *) yaw1, nop, 4);
        Memory::patchBytes((void *) yaw2, nop, 4);
        if(WinrtUtils::checkBelowOrEqual(21,30)) {
            Memory::patchBytes((void *) pitch, nop, 4);
            Memory::patchBytes((void *) movement, nop, 4);
        }
    }

    void unpatch() {
        if(!this->active) return;
        this->active = false;
        Memory::patchBytes((void *) yaw1, originalYaw1.data(), 4);
        Memory::patchBytes((void *) yaw2, originalYaw2.data(), 4);
        if(WinrtUtils::checkBelowOrEqual(21,30)) {
            Memory::patchBytes((void *) pitch, originalPitch.data(), 4);
            Memory::patchBytes((void *) movement, originalMovement.data(), 4);
        }
    }

    void defaultConfig() override {

        if (settings.getSettingByName<std::string>("toggle") == nullptr) settings.addSetting("toggle", false);

        if (settings.getSettingByName<std::string>("mode") == nullptr)
            settings.addSetting("mode", (std::string) "3rd Person back");
    }

    void settingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addKeybind("Freelook Keybind", "Hold for 2 seconds!", getKeybind());

        this->addToggle("Toggleable Mode", "Click to toggle or Hold to keep enabled", this->settings.getSettingByName<bool>("toggle")->value);

        this->addDropdown("Freelook View Mode", "", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, this->settings.getSettingByName<std::string>("mode")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();

    }

    void onUpdatePlayer(UpdatePlayerEvent &event) {
        if (this->active) {
            event.cancel();
        }
    }

    void onGetViewPerspective(PerspectiveEvent &event) {
        if (this->active) {
            std::string setting = this->settings.getSettingByName<std::string>("mode")->value;
            // TODO: Let use F5 (perspective switch key)
            if(setting == "1st Person") {
                event.setPerspective(Perspective::FirstPerson);
            }
            if(setting == "3rd Person back") {
                event.setPerspective(Perspective::ThirdPersonBack);
            }
            if(setting == "3rd Person front") {
                event.setPerspective(Perspective::ThirdPersonFront);
            }
        }
    }
    // TODO: better restriction systtem !!!
    // TODO: cancel event if its ment for this module ?
    void onKey(KeyEvent &event) {
        if (this->isKeyPartOfKeybind(event.key)) {
            // TODO: for multiple key this might not work!!!
            if (this->isKeybind(event.keys)) { // key is defo pressed
                std::string serverIP = SDK::getServerIP();
                if ((serverIP.find("hive") != std::string::npos ||
                     serverIP.find("galaxite") != std::string::npos ||
                     serverIP.find("venity") != std::string::npos)) { // TODO: make it only show once per server switch?
                    FlarialGUI::Notify("Can't use freelook on " + serverIP); // TODO: move restrictions to API
                    this->restricted = true;
                } else {
                    this->restricted = false;
                }
                if (!this->restricted) {
                    if (this->settings.getSettingByName<bool>("toggle")->value) {
                        if (!this->active) {
                            patch();
                        }else{
                            unpatch();
                        }
                    } else {
                        patch();
                    }
                } else {
                    unpatch(); // module restricted
                }
            } else { // key released
                if (!this->settings.getSettingByName<bool>("toggle")->value)
                    unpatch();
            }
        }

    };
};