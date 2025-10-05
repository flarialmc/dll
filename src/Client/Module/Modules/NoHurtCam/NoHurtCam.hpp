#pragma once

#include "../Module.hpp"
#include "Events/Game/RaknetTickEvent.hpp"
#include "Events/Game/TickEvent.hpp"
#include "../../../../Assets/Assets.hpp"


class NoHurtCam : public Module {

private:

    // 76 1C 66 0F 6E 00 | 40 F2 0F 11 03 8B 44 24 48 89 43 08 C6 43 0C 01
    // 8B 44 24 48 89 43 08 C6 patch to 8B 44 24 48 90 90 90 C6

    static inline bool patched = false;

    static inline uintptr_t sigOffset;
    static inline std::vector<uint8_t> originalCameraAngle;

public:

    NoHurtCam(): Module("No Hurt Cam", "Disables hurt camera animation",
        IDR_REACH_PNG, "") {
        int size;
        if (VersionUtils::checkAboveOrEqual(21, 30)) {
            size = 5;
        } else {
            size = 3;
        }

        originalCameraAngle.resize(size);

        if (sigOffset == NULL) {
            if (VersionUtils::checkAboveOrEqual(21, 30)) {
                sigOffset = GET_SIG_ADDRESS("CameraAssignAngle");
            } else {
                sigOffset = GET_SIG_ADDRESS("CameraAssignAngle") + 4;
            }
        }

        Memory::patchBytes(originalCameraAngle.data(), (LPVOID) sigOffset, size);
    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    static void patch();

    static void unpatch();

    void onRaknetTick(RaknetTickEvent &event);

    void onTick(TickEvent &event);
};
