#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"


class NoHurtCam : public Module {
public:
    // 76 1C 66 0F 6E 00 | 40 F2 0F 11 03 8B 44 24 48 89 43 08 C6 43 0C 01
    // 8B 44 24 48 89 43 08 C6 patch to 8B 44 24 48 90 90 90 C6

    // TODO: restrict on hive

    const static inline uintptr_t sigOffset = Memory::findSig("8B 44 24 48 89 43 08 C6") + 4;
    static inline std::vector<uint8_t> originalCameraAngle;
    static inline uint8_t newCameraAngle[4] = {0x90, 0x90, 0x90};

    NoHurtCam() : Module("No Hurt Cam", "Disables hurt camera animation", R"(\Flarial\assets\reach.png)", "") {

        originalCameraAngle.resize(3);

        memcpy(originalCameraAngle.data(), (LPVOID) sigOffset, 3);

        Module::setup();

    };

    void onEnable() override {
        Memory::patchBytes((LPVOID) sigOffset, newCameraAngle, 3);
        Module::onEnable();
    }

    void onDisable() override {
        Memory::patchBytes((LPVOID) sigOffset, originalCameraAngle.data(), 3);
        Module::onDisable();
    }
};