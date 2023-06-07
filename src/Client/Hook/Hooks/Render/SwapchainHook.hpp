#pragma once

#include "../Hook.hpp"
#include <dxgi1_6.h>

#include <d2d1_3.h>
#include <d3d12.h>
#include <d3d11.h>

#include "kiero.h"

class SwapchainHook : public Hook
{
private:
    static void swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags);

public:
    typedef void(__thiscall *SwapchainOriginal)(IDXGISwapChain3 *, UINT, UINT);
    static inline SwapchainOriginal func_original = 0;

    SwapchainHook();
    void enableHook() override;
};