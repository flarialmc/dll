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
    typedef void(__thiscall *SwapchainOriginal)(IDXGISwapChain *, UINT, UINT);
    static inline SwapchainOriginal func_original = 0;
    static bool init;
    SwapchainHook();
    void enableHook() override;
    static inline ID3D12CommandQueue* queue;
    static inline std::vector<IDXGISurface1> DXGISurfaces;
    static inline std::vector<ID2D1Bitmap1*> D2D1Bitmaps;
    static inline ID3D12DescriptorHeap* D3D12DescriptorHeap;
    static inline std::vector<ID3D11RenderTargetView*> RenderTargetViews;

};