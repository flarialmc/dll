#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class LookListener : public Listener {

    Module* module;
private:
    Vec2<float> oldRotations;
    bool enabled;
    static inline uintptr_t yaw1 = Memory::findSig("F3 0F 11 30 F3 ? ? 78 ? 49 8B CE");
    static inline uintptr_t yaw2 = Memory::findSig("F3 0F 11 38 F3 ? ? 70 ? 48 8B 8C");
    static inline uintptr_t pitch = Memory::findSig("F3 0F 11 0E 48 89 9C");

    static inline std::vector<uint8_t> OriginalYaw1;
    static inline std::vector<uint8_t> PatchedYaw1;
    static inline std::vector<uint8_t> OriginalYaw2;
    static inline std::vector<uint8_t> PatchedYaw2;
    static inline std::vector<uint8_t> OriginalPitch;
    static inline std::vector<uint8_t> PatchedPitch;
public:

    void onLocalTick(TickEvent& event) override {

        if (module->settings.getSettingByName<bool>("enabled")->value && SDK::raknetConnector->JoinedIp.find("hive") !=  std::string::npos) {
            FlarialGUI::Notify("Can't use freelook on " + SDK::raknetConnector->JoinedIp);
            module->settings.getSettingByName<bool>("enabled")->value = false;
        }


        if (module->settings.getSettingByName<bool>("enabled")->value) {
            event.getActor()->getMovementProxyComponent()->movementProxy->SetRotation(&oldRotations);
      //      event.getActor()->getMovementProxyComponent()->movementProxy->SetYHeadRotation(oldRotations.y);
        }
        else {
            oldRotations = event.getActor()->actorRotationComponent->Rotation;
        }

        if(enabled != module->settings.getSettingByName<bool>("enabled")->value){
            enabled = module->settings.getSettingByName<bool>("enabled")->value;

            if (enabled) patch();
            else unpatch();
        }

    }

    static void patch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID)yaw1, PatchedYaw1.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)yaw1, PatchedYaw1.data(), PatchedYaw1.size());
        VirtualProtect((LPVOID)yaw1, PatchedYaw1.size(), oldProtect, &oldProtect);

        DWORD oldProtect2;
        VirtualProtect((LPVOID)yaw2, PatchedYaw2.size(), PAGE_EXECUTE_READWRITE, &oldProtect2);
        memcpy((LPVOID)yaw2, PatchedYaw2.data(), PatchedYaw2.size());
        VirtualProtect((LPVOID)yaw2, PatchedYaw2.size(), oldProtect, &oldProtect2);

        DWORD oldProtect3;
        VirtualProtect((LPVOID)pitch, PatchedPitch.size(), PAGE_EXECUTE_READWRITE, &oldProtect3);
        memcpy((LPVOID)pitch, PatchedPitch.data(), PatchedPitch.size());
        VirtualProtect((LPVOID)pitch, PatchedPitch.size(), oldProtect, &oldProtect3);

    }

    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;


        if (!module->IsKeybind(event.keys)) module->settings.getSettingByName<bool>("enabled")->value = false;

    };

public:
    explicit LookListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;

        OriginalYaw1.resize(4);
        memcpy(OriginalYaw1.data(), (LPVOID)yaw1, 4);

        OriginalYaw2.resize(4);
        memcpy(OriginalYaw2.data(), (LPVOID)yaw2, 4);

        OriginalPitch.resize(4);
        memcpy(OriginalPitch.data(), (LPVOID)pitch, 4);

        PatchedYaw1.push_back(0x90);
        PatchedYaw1.push_back(0x90);
        PatchedYaw1.push_back(0x90);
        PatchedYaw1.push_back(0x90);

        PatchedYaw2.push_back(0x90);
        PatchedYaw2.push_back(0x90);
        PatchedYaw2.push_back(0x90);
        PatchedYaw2.push_back(0x90);

        PatchedPitch.push_back(0x90);
        PatchedPitch.push_back(0x90);
        PatchedPitch.push_back(0x90);
        PatchedPitch.push_back(0x90);

    }

    static void unpatch() {

        DWORD oldProtect;
        VirtualProtect((LPVOID)yaw1, OriginalYaw1.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((LPVOID)yaw1, OriginalYaw1.data(), OriginalYaw1.size());
        VirtualProtect((LPVOID)yaw1, OriginalYaw1.size(), oldProtect, &oldProtect);

        DWORD oldProtect2;
        VirtualProtect((LPVOID)yaw2, OriginalYaw2.size(), PAGE_EXECUTE_READWRITE, &oldProtect2);
        memcpy((LPVOID)yaw2, OriginalYaw2.data(), OriginalYaw2.size());
        VirtualProtect((LPVOID)yaw2, OriginalYaw2.size(), oldProtect, &oldProtect2);

        DWORD oldProtect3;
        VirtualProtect((LPVOID)pitch, OriginalPitch.size(), PAGE_EXECUTE_READWRITE, &oldProtect3);
        memcpy((LPVOID)pitch, OriginalPitch.data(), OriginalPitch.size());
        VirtualProtect((LPVOID)pitch, OriginalPitch.size(), oldProtect, &oldProtect3);

    }
};
