#pragma once
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class MinecraftGame_Update : public Hook {
private:
    static void MinecraftGame_UpdateDetour(
        __int64 a1, unsigned __int16 a2, bool a3, bool a4);

public:
    typedef void (__thiscall* original)(
        __int64 a1, unsigned __int16 a2, bool a3, bool a4);

    static inline original funcOriginal = nullptr;

    MinecraftGame_Update() : Hook("MinecraftGame::Update", GET_SIG_ADDRESS("bgfx::RenderContextD3D11::SetFrameBuffer")) {}

    void enableHook() override;
};