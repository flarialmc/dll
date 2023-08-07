#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class GuiScaleListener : public Listener {

    bool enabled = false;
    static inline std::vector<uint8_t> Original;
    static inline std::vector<uint8_t> Patched;
    static inline uintptr_t real = Memory::findSig("00 00 ? ? 00 00 A0 40 00 00 C0 40");


    void onLocalTick(TickEvent& event) override {

        if(enabled != module->settings.getSettingByName<bool>("enabled")->value){
            enabled = module->settings.getSettingByName<bool>("enabled")->value;

            if (enabled) {
                patch();
                FlarialGUI::Notify("Resize Minecraft Window to apply.");
            }
            else unpatch();
        }
    }

    static void floatToAOB(float floatValue, unsigned char* aob) {
        unsigned char* floatBytes = reinterpret_cast<unsigned char*>(&floatValue);
        for (int i = 0; i < sizeof(float); i++) {
            aob[i] = floatBytes[i];
        }
    }

public:
    explicit GuiScaleListener(const char string[5], Module* module, std::vector<uint8_t> Original) {

        this->name = string;
        this->module = module;
        this->Original = Original;
        this->Patched = std::vector<uint8_t>();

        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);
        Patched.push_back(0x90);

    }

    static inline Module* module;


    static void patch() {

        float floatValue = module->settings.getSettingByName<float>("guiscale")->value; // Replace with your desired float value
        unsigned char aob[sizeof(float)];

        floatToAOB(floatValue, aob);

        DWORD oldProtect;
        VirtualProtect((LPVOID)real, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)real, &floatValue, sizeof(float));
        VirtualProtect((LPVOID)real, sizeof(float), oldProtect, &oldProtect);

        isPatched = false;
        Logger::debug("patched");


    }

    static void unpatch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID)real, Original.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)real, Original.data(), Original.size());
        VirtualProtect((LPVOID)real, Original.size(), oldProtect, &oldProtect);

        isPatched = false;

    }

    static inline bool isPatched = false;
};
