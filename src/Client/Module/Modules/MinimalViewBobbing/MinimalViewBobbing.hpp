#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"


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
