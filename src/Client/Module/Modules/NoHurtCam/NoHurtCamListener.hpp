#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class NoHurtCamListener : public Listener {
    Module* module;

    // 76 1C 66 0F 6E 00 | 40 F2 0F 11 03 8B 44 24 48 89 43 08 C6 43 0C 01
    // 8B 44 24 48 89 43 08 C6 patch to 8B 44 24 48 90 90 90 C6

    static inline bool patched = false;

    static inline uintptr_t sigOffset;
    static inline std::vector<uint8_t> originalCameraAngle;
    static inline uint8_t newCameraAngle[3] = {0x90, 0x90, 0x90};

public:

    static void patch() {
        if(patched) return;
        patched = true;
        Memory::patchBytes((LPVOID) sigOffset, newCameraAngle, 3);
    }

    static void unpatch() {
        if(!patched) return;
        patched = false;
        Memory::patchBytes((LPVOID) sigOffset, originalCameraAngle.data(), 3);
    }

    void onRaknetTick(RaknetTickEvent &event) override {
        if (module->isEnabled()) {
            std::string serverIP = SDK::getServerIP();
            if (serverIP.find("hive") != std::string::npos) {
                if (!module->restricted) {
                    FlarialGUI::Notify("Can't use No Hurt Cam on " + serverIP); // TODO: move restrictions to API
                    module->restricted = true;
                }
            } else {
                module->restricted = false;
            }
        }
    }

    void onTick(TickEvent &event) override {
        if (!module->restricted) {
            patch();
        } else {
            unpatch();
        }
    }

    void onUnregister() override {
        unpatch();
    }

    NoHurtCamListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

        originalCameraAngle.resize(3);

        if(sigOffset == NULL) {
            sigOffset = GET_SIG_ADDRESS("CameraAssignAngle") + 4;
        }

        Memory::patchBytes( originalCameraAngle.data(), (LPVOID)sigOffset, 3);
    }
};