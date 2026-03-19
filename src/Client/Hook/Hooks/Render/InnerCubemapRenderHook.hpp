#pragma once

#include "../Hook.hpp"

// Hook for the INNER cubemap render function (sub_1413A6BC0 in 1.21.130)
// This is called from CubemapBackgroundScreen::render AFTER the render target is set up.
//
// By hooking HERE instead of the parent function, we ensure:
// 1. The render target and other state is properly initialized
// 2. We can fill with magenta BEFORE the cubemap renders
// 3. The parent function's fade overlay still works
//
// This enables the greenscreen approach for proper under-UI rendering.

class InnerCubemapRenderHook : public Hook {
private:
    // Callback signature based on IDA: sub_1413A6BC0(__int64 a1, __int64 a2, char a3)
    static unsigned __int64 __fastcall callback(void* a1, void* screenContext, char useHDR);

public:
    typedef unsigned __int64 (__fastcall *original)(void* a1, void* screenContext, char useHDR);

    static inline original funcOriginal = nullptr;

    InnerCubemapRenderHook();

    void enableHook() override;
};
