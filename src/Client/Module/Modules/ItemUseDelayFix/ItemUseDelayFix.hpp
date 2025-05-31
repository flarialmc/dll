#pragma once

#include "../Module.hpp"


class ItemUseDelayFix : public Module {
private:
    static inline std::vector<uint8_t> original;
    static inline uintptr_t address;
public:
    ItemUseDelayFix() : Module("Item Use Delay Fix", "Removes 200ms delay after attack on using items (e.g projectiles).",
                           IDR_NAMETAG_PNG, "") {

        address = GET_SIG_ADDRESS("ClientInputCallbacks::handleBuildAction_onAttack_setNoBlockBreakUntil_CallPatch");

        original.resize(6);
        Memory::copyBytes((LPVOID) address, original.data(), 6);

        Module::setup();
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
        Memory::nopBytes((void *)address, original.size());
    }

    static void unpatch() {
        Memory::patchBytes((void *)address, original.data(), original.size());
    }
};