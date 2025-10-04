#pragma once

#include "../Module.hpp"
#include "Assets/Assets.hpp"
#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Utils/Memory/Memory.hpp"


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

        
    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    static void patch();

    static void unpatch();
};