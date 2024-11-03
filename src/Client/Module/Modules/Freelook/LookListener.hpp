#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class LookListener : public Listener {

    Module *module;
private:
    //static inline uintptr_t rot = Memory::findSig("F3 0F 11 38 ? ? 7C 24 ? 48 81 C4");

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
    void patch() const {
        if(module->active) return;
        module->active = true;
        Memory::patchBytes((void *) yaw1, nop, 4);
        Memory::patchBytes((void *) yaw2, nop, 4);
        if(WinrtUtils::checkBelowOrEqual(21,30)) {
            Memory::patchBytes((void *) pitch, nop, 4);
            Memory::patchBytes((void *) movement, nop, 4);
        }
    }

    void unpatch() const {
        if(!module->active) return;
        module->active = false;
        Memory::patchBytes((void *) yaw1, originalYaw1.data(), 4);
        Memory::patchBytes((void *) yaw2, originalYaw2.data(), 4);
        if(WinrtUtils::checkBelowOrEqual(21,30)) {
            Memory::patchBytes((void *) pitch, originalPitch.data(), 4);
            Memory::patchBytes((void *) movement, originalMovement.data(), 4);
        }
    }

    void onUpdatePlayer(UpdatePlayerEvent &event) override {
        if (module->active) {
            event.cancel();
        }
    }

    void onGetViewPerspective(PerspectiveEvent &event) override {
        if (module->active) {
            std::string setting = module->settings.getSettingByName<std::string>("mode")->value;
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
    void onKey(KeyEvent &event) override {
        if (module->isKeyPartOfKeybind(event.key)) {
            // TODO: for multiple key this might not work!!!
            if (module->isKeybind(event.keys)) { // key is defo pressed
                std::string serverIP = SDK::getServerIP();
                if ((serverIP.find("hive") != std::string::npos ||
                     serverIP.find("galaxite") != std::string::npos ||
                     serverIP.find("venity") != std::string::npos)) { // TODO: make it only show once per server switch?
                    FlarialGUI::Notify("Can't use freelook on " + serverIP); // TODO: move restrictions to API
                    module->restricted = true;
                } else {
                    module->restricted = false;
                }
                if (!module->restricted) {
                    if (module->settings.getSettingByName<bool>("toggle")->value) {
                        if (!module->active) {
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
                if (!module->settings.getSettingByName<bool>("toggle")->value)
                    unpatch();
            }
        }

    };

    void onUnregister() override {
        unpatch();
        module->active = false;
    };

public:
    explicit LookListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

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
    }
};
