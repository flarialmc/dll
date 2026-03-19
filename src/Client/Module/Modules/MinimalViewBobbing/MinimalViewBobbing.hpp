#pragma once

#include "../Module.hpp"


class MinimalViewBobbing : public Module {
private:
    static inline std::vector<uint8_t> original;
    static inline uintptr_t address;
public:
    MinimalViewBobbing() : Module("Minimal View Bobbing", "Prevent camera shake when view bobbing is on.",
                           IDR_EYE_PNG, "", false) {

        address = GET_SIG_ADDRESS("MinimalViewBobbing");

        original.resize(6);
        Memory::copyBytes((LPVOID) address, original.data(), 6);

        
    };

    void onEnable() override {
        if (VersionUtils::checkAboveOrEqual(21, 130)) FlarialGUI::Notify("Minimal View Bobbing has been broken and will just tilt your camera instead. You may want to disable it.");
        patch();
        Module::onEnable();
    }

    void onDisable() override {
        unpatch();
        Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig("core");
    }

    static void patch() {
        Memory::nopBytes((void *)address, 6);
    }

    static void unpatch() {
        Memory::patchBytes((void *)address, original.data(), original.size());
    }
};
