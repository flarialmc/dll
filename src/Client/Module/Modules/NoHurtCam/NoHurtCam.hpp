#pragma once

#include "../Module.hpp"


class NoHurtCam : public Module {
private:
    // 76 1C 66 0F 6E 00 | 40 F2 0F 11 03 8B 44 24 48 89 43 08 C6 43 0C 01
    // 8B 44 24 48 89 43 08 C6 patch to 8B 44 24 48 90 90 90 C6

    static inline bool patched = false;

    static inline uintptr_t sigOffset;
    static inline std::vector<uint8_t> originalCameraAngle;
public:
    NoHurtCam() : Module("No Hurt Cam", "Disables hurt camera animation", IDR_REACH_PNG, "", false, false) {
        int size;
        if (VersionUtils::checkAboveOrEqual(21, 30)) {
            size = 5;
        } else {
            size = 3;
        }

        originalCameraAngle.resize(size);

        if(sigOffset == NULL) {
            if (VersionUtils::checkAboveOrEqual(21, 30)) {
                sigOffset = GET_SIG_ADDRESS("CameraAssignAngle");
            } else {
                sigOffset = GET_SIG_ADDRESS("CameraAssignAngle") + 4;
            }
        }

        Memory::patchBytes( originalCameraAngle.data(), (LPVOID)sigOffset, size);
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RaknetTickEvent, &NoHurtCam::onRaknetTick)
        Listen(this, TickEvent, &NoHurtCam::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RaknetTickEvent, &NoHurtCam::onRaknetTick)
        Deafen(this, TickEvent, &NoHurtCam::onTick)
        Module::onDisable();
    }

    static void patch() {
        if(patched) return;
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

    static void unpatch() {
        if(!patched) return;
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

    void onRaknetTick(RaknetTickEvent &event) {
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

    void onTick(TickEvent &event) {
        if (!this->restricted) {
            patch();
        } else {
            unpatch();
        }
    }
};