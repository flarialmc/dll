#pragma once
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
struct FrameBufferHandle {
    uint16_t id;
    bool Valid;
};


class MinecraftGame_Update : public Hook {
private:
    static void MinecraftGame_UpdateDetour(
        __int64 a1, FrameBufferHandle fbh, bool a3, bool a4);

public:
    typedef void (__thiscall* original)(
        __int64 a1, FrameBufferHandle fbh, bool a3, bool a4);

    static inline original funcOriginal = nullptr;
    static inline int index = 0;

    MinecraftGame_Update() : Hook("MinecraftGame::Update", GET_SIG_ADDRESS("bgfx::RenderContextD3D11::SetFrameBuffer")) {}

    void enableHook() override;
};