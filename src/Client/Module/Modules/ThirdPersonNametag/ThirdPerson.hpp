#pragma once

#include "../Module.hpp"


class ThirdPerson : public Module {
private:
    static inline std::vector<uint8_t> original;
    static inline uintptr_t address;
public:
    ThirdPerson() : Module("Nametag", "Shows your nametag for you while\nin 3rd person mode.",
                           IDR_NAMETAG_PNG, "") {

        address = GET_SIG_ADDRESS("ThirdPersonNametag");

        original.resize(6);
        Memory::copyBytes((LPVOID) address, original.data(), 6);

        Module::setup();
    };

    void defaultConfig() override {
        Module::defaultConfig("core");
    }

    void onEnable() override {
        patch();
        Module::onEnable();
    }

    void onDisable() override {
        unpatch();
        Module::onDisable();
    }

    static void patch() {
        Memory::nopBytes((void *)address, 6);
    }

    static void unpatch() {
        Memory::patchBytes((void *)address, original.data(), original.size());
    }
};