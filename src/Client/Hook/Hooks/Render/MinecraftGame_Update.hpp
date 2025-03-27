#pragma once
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class MinecraftGame_Update : public Hook {
private:
    static void* MinecraftGame_UpdateDetour(void* _this);

public:
    typedef void* (__thiscall* original)(void* _this);

    static inline original funcOriginal = nullptr;

    MinecraftGame_Update() : Hook("MinecraftGame::Update", GET_SIG_ADDRESS("MinecraftGame::Update")) {}

    void enableHook() override;
};