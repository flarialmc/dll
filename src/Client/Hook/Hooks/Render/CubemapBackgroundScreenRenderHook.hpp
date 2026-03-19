#pragma once

#include "../Hook.hpp"

// Hook for CubemapBackgroundScreen::render
// This function renders the panorama background on main menu screens.
// Address in 1.21.132: 0x1413A6750
// Signature: 48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F2 48 8B F9 0F 57 C0
//
// CRITICAL: This hook fires on the MAIN THREAD, not the render thread!
// The function uses Tessellator to record draw commands, but the actual
// D3D11 draw calls happen later on the render pool threads.
//
// DO NOT call D3D11 methods from this hook! It will cause race conditions
// with the render pool threads and crash in the graphics driver
// (igd10umt64xe.dll access violation at null + offset).
//
// The panorama shader module uses this hook to SKIP the vanilla panorama.
// The actual shader rendering happens at RenderEvent time (Present) on
// the render thread, where D3D11 calls are safe.

class CubemapBackgroundScreenRenderHook : public Hook {
private:
    // Callback signature: __int64 __fastcall func(__int64 this, __int64* screenContext)
    static __int64 __fastcall callback(void* _this, void* screenContext);

public:
    typedef __int64 (__fastcall *original)(void* _this, void* screenContext);

    static inline original funcOriginal = nullptr;

    CubemapBackgroundScreenRenderHook();

    void enableHook() override;
};
