#pragma once

#include "../../../../Client.hpp"

class AntiCheat : public Listener {
public:
    void onTick(TickEvent &event) {
        float* reach = Memory::getOffsetFromSig<float*>(GET_SIG_ADDRESS("InGamePlayScreen::_pick::gSurvivalPickRange"), 4);
        Logger::debug("Reach {}", *reach);
        DWORD oldProtect;
        VirtualProtect((LPVOID)reach, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);
        *reach = 3;
        VirtualProtect((LPVOID)reach, sizeof(float), oldProtect, &oldProtect);
    }

    AntiCheat() {
        Listen(this, TickEvent, &AntiCheat::onTick);
    }

    ~AntiCheat() {
        Deafen(this, TickEvent, &AntiCheat::onTick);
    }
};