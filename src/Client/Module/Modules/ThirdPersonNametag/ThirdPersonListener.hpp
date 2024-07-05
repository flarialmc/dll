#pragma once

#include <format>
#include <utility>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class ThirdPersonListener : public Listener {

    Module *module;
    bool enabled = false;
    static inline std::vector<uint8_t> Original;
    static inline std::vector<uint8_t> Patched;
    static inline uintptr_t real;


    void onTick(TickEvent &event) override {

        if (enabled != module->isEnabled()) {
            enabled = module->isEnabled();

            if (enabled) patch();
            else unpatch();
        }
    }

    static void patch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID) real, Patched.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID) real, Patched.data(), Patched.size());
        VirtualProtect((LPVOID) real, Patched.size(), oldProtect, &oldProtect);

    }

public:
    explicit ThirdPersonListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

        real = Memory::findSig(GET_SIG("ThirdPersonNametag"));

        ThirdPersonListener::Original.resize(6);
        memcpy(ThirdPersonListener::Original.data(),
               (LPVOID) real, 6);

        ThirdPersonListener::Patched = std::vector<uint8_t>();

        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);

    }

    static void unpatch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID) real, Original.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID) real, Original.data(), Original.size());
        VirtualProtect((LPVOID) real, Original.size(), oldProtect, &oldProtect);

    }
};
