#pragma once

#include "../Hook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class OnDeviceLostHook : public Hook {

private:
    static void callback(void* game);

public:
    typedef void(__thiscall* original)(void*);

    static inline original deviceLostOriginal = nullptr;

    OnDeviceLostHook() : Hook("onDeviceLostHook", GET_SIG_ADDRESS("MinecraftGame::onDeviceLost")) {}

    void enableHook() override;
};
