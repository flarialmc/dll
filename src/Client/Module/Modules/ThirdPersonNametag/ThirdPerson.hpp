#pragma once

#include "../Module.hpp"


class ThirdPerson : public Module {
private:
    static inline std::vector<uint8_t> Original;
    static inline std::vector<uint8_t> Patched;
    static inline uintptr_t real;
public:
    ThirdPerson() : Module("Nametag", "Shows your nametag for you while\nin 3rd person mode.",
                           IDR_NAMETAG_PNG, "") {

        real = GET_SIG_ADDRESS("ThirdPersonNametag");

        Original.resize(6);
        memcpy(Original.data(), (LPVOID) real, 6);

        Patched = std::vector<uint8_t>();

        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
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

    static void patch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID) real, Patched.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID) real, Patched.data(), Patched.size());
        VirtualProtect((LPVOID) real, Patched.size(), oldProtect, &oldProtect);

    }

    static void unpatch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID) real, Original.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID) real, Original.data(), Original.size());
        VirtualProtect((LPVOID) real, Original.size(), oldProtect, &oldProtect);

    }
};