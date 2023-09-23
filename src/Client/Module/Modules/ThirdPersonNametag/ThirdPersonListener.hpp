#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class ThirdPersonListener : public Listener {

    Module* module;
    bool enabled = false;
    static inline std::vector<uint8_t> Original;
    static inline std::vector<uint8_t> Patched;
    static inline uintptr_t real = Memory::findSig("? ? ? ? ? ? 49 8B ? 48 8B ? E8 ? ? ? ? 84 C0 ? ? ? ? ? ? 49 8B ? 49 8b ? ? ? ? ? 49");


    void onLocalTick(TickEvent& event) override {

        if(enabled != module->settings.getSettingByName<bool>("enabled")->value){
            enabled = module->settings.getSettingByName<bool>("enabled")->value;

            if (enabled) patch();
            else unpatch();
        }
    }

    void patch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID)real, Patched.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)real, Patched.data(), Patched.size());
        VirtualProtect((LPVOID)real, Patched.size(), oldProtect, &oldProtect);

    }

public:
    explicit ThirdPersonListener(const char string[5], Module* module, std::vector<uint8_t> Original) {
        this->name = string;
        this->module = module;
        this->Original = Original;
        this->Patched = std::vector<uint8_t>();

        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);

    }

    static void unpatch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID)real, Original.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)real, Original.data(), Original.size());
        VirtualProtect((LPVOID)real, Original.size(), oldProtect, &oldProtect);

    }
};
