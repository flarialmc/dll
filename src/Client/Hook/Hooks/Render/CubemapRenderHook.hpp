#pragma once

#include "../Hook.hpp"
#include "DirectX/DXGI/SwapchainHook.hpp"
#include "Utils/Logger/Logger.hpp"

/**
 * Hook for the game's CubeMap render function.
 * This function renders the panorama background on menu screens.
 *
 * When PanoramaShader module is enabled and we're on a panorama screen,
 * we render our custom shader INSTEAD of the original panorama.
 * Since this happens during the game's frame rendering (before UI),
 * the game's menu UI will render ON TOP of our shader - exactly what we want!
 *
 * Note: This replaces LegacyCubemapRenderHook which hooked a deprecated function
 * that is no longer called in current game versions.
 */
class CubemapRenderHook : public Hook {
private:
    // The actual CubeMap render function takes 13 parameters
    // x64 calling convention: rcx, rdx, r8, r9 for first 4, rest on stack
    static __int64 __fastcall CubemapRenderCallback(
        __int64 a1,
        __int64 a2,
        __int64 a3,
        __int64 a4,
        __int64 a5,
        __int64 a6,
        __int64 a7,
        char a8,
        int a9,
        __int16 a10,
        __int16 a11,
        __int16 a12,
        int a13
    ) {
        static int callCounter = 0;
        bool shouldLog = (++callCounter % 60 == 1);

        if (shouldLog) {
            Logger::debug("[CubemapRenderHook] HOOK CALLED! isDX12={}", SwapchainHook::isDX12);
        }

        return funcOriginal(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
    }

public:
    typedef __int64 (__fastcall *original)(__int64, __int64, __int64, __int64, __int64, __int64, __int64, char, int, __int16, __int16, __int16, int);
    static inline original funcOriginal = nullptr;

    CubemapRenderHook() : Hook("CubemapRender", GET_SIG_ADDRESS("CubemapRender")) {
    }

    void enableHook() override {
        this->autoHook((void*)CubemapRenderCallback, (void**)&funcOriginal);
    }
};
