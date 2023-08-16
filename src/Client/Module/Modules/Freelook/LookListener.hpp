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
    bool enabled = false;
    static inline uintptr_t yaw1 = Memory::findSig("F3 0F 11 30 F3 ? ? 78 ? 49 8B CE");
    static inline uintptr_t yaw2 = Memory::findSig("F3 0F 11 38 F3 ? ? 70 ? 48 8B 8C");
    static inline uintptr_t pitch = Memory::findSig("F3 0F 11 0E 48 89 9C");
    static inline uintptr_t rot = Memory::findSig("F3 0F ? ? 48 8D ? ? F3 0F ? ? ? ? F3 0F");

    static inline std::vector<uint8_t> OriginalYaw1;
    static inline std::vector<uint8_t> PatchedYaw1;
    static inline std::vector<uint8_t> OriginalYaw2;
    static inline std::vector<uint8_t> PatchedYaw2;
    static inline std::vector<uint8_t> OriginalPitch;
    static inline std::vector<uint8_t> PatchedPitch;
    static inline std::vector<uint8_t> OriginalRot;
    static inline std::vector<uint8_t> PatchedRot;
public:

    void onRender(RenderEvent& event) override {
        
        Module* gui = ModuleManager::getModule("MC GUI Scale");
        if (gui->settings.getSettingByName<bool>("enabled")->value){
        float percent = gui->settings.getSettingByName<float>("guiscale")->value;
        SDK::clientInstance->guiData->GuiScale = percent;
        SDK::clientInstance->guiData->ScreenSizeScaled = { SDK::clientInstance->guiData->ScreenSize.x * 1 / percent, SDK::clientInstance->guiData->ScreenSize.y * 1 / percent };
        SDK::clientInstance->guiData->scalingMultiplier = 1 / percent;
        }
    }

    void onLocalTick(TickEvent& event) override {



        if (enabled != module->settings.getSettingByName<bool>("enabled")->value) {
            enabled = module->settings.getSettingByName<bool>("enabled")->value;

            if (enabled) {
                //savedRotations = SDK::clientInstance->getLocalPlayer()->rotations;
                patch();
            }
            else unpatch();
        }

        if (module->settings.getSettingByName<bool>("enabled")->value) {
             event.getActor()->getMovementProxyComponent()->movementProxy->SetRotation(&oldRotations);
        }
        else {
            oldRotations = event.getActor()->actorRotationComponent->Rotation;
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


        DWORD oldProtect4;
        VirtualProtect((LPVOID)rot, PatchedRot.size(), PAGE_EXECUTE_READWRITE, &oldProtect4);
        memcpy((LPVOID)rot, PatchedRot.data(), PatchedRot.size());
        VirtualProtect((LPVOID)rot, PatchedRot.size(), oldProtect, &oldProtect4);
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

        OriginalRot.resize(4);
        memcpy(OriginalRot.data(), (LPVOID)rot, 4);

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

        PatchedRot.push_back(0x90);
        PatchedRot.push_back(0x90);
        PatchedRot.push_back(0x90);
        PatchedRot.push_back(0x90);

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

        DWORD oldProtect4;
        VirtualProtect((LPVOID)rot, OriginalRot.size(), PAGE_EXECUTE_READWRITE, &oldProtect4);
        memcpy((LPVOID)rot, OriginalRot.data(), OriginalRot.size());
        VirtualProtect((LPVOID)rot, OriginalRot.size(), oldProtect, &oldProtect4);
    }
};
