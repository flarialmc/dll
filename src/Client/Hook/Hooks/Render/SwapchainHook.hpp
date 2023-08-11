#pragma once

#include "../Hook.hpp"
#include <dxgi1_6.h>

#include <d2d1_3.h>
#include <d3d12.h>
#include <d3d11.h>
#include <d3d11on12.h>

#include "kiero.h"

class SwapchainHook : public Hook
{
private:
    static void swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags);


public:
    typedef void(__thiscall *SwapchainOriginal)(IDXGISwapChain3*, UINT, UINT);
    static inline SwapchainOriginal func_original = 0;
    static bool init;
    SwapchainHook();
    void enableHook() override;
    static ID3D12CommandQueue* queue;
    static inline std::vector<IDXGISurface1*> DXGISurfaces;
    static inline std::vector<ID2D1Bitmap1*> D2D1Bitmaps;
    static inline std::vector<ID3D11Resource*> D3D11Resources;
    static inline IDXGISwapChain3* swapchain;
    static inline ID3D12DescriptorHeap* D3D12DescriptorHeap;
    static inline ID3D11On12Device* d3d11On12Device;
    static inline ID2D1Bitmap1* D2D1Bitmap;
    static inline ID3D11DeviceContext* context;
    static inline uintptr_t bufferCount;
    static bool hasResized;
    static int currentBitmap;
    static inline UINT flagsreal;
};