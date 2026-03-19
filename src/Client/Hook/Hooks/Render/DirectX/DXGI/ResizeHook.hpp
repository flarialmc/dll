#pragma once
#include <dxgi.h>
#include <winrt/base.h>
#include "kiero/kiero.h"
#include "../../../Hook.hpp"

class ResizeHook : public Hook
{
private:
    static void
    resizeCallback(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height,
                   DXGI_FORMAT newFormat,
                   UINT flags);

    static void
    resizeCallback121120(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height,
                         DXGI_FORMAT newFormat,
                         UINT flags, const UINT* pCreationNodeMask,
                         IUnknown* const * ppPresentQueue);

public:
    typedef void (__thiscall *resizeOriginal)(IDXGISwapChain* pSwapChain, UINT bufferCount,
                                              UINT width, UINT height,
                                              DXGI_FORMAT newFormat, UINT flags);

    static inline resizeOriginal funcOriginal = nullptr;

    typedef void (__thiscall *resize121120Original)(IDXGISwapChain* pSwapChain, UINT bufferCount,
                                                    UINT width, UINT height,
                                                    DXGI_FORMAT newFormat, UINT flags,
                                                    const UINT* pCreationNodeMask,
                                                    IUnknown* const * ppPresentQueue);

    static inline resize121120Original funcOriginal121120 = nullptr;

    ResizeHook();

    void enableHook() override;

    // Re-hook ResizeBuffers for DX11 after Better Frames fallback
    // This is needed because initial hook uses DX12 indices from kiero
    static void rehookForDX11(IDXGISwapChain* pSwapChain);

    // Re-hook ResizeBuffers from a live swapchain's vtable (Phase 2 validation).
    // Ensures Resize hooks match the actual swapchain, not kiero's dummy vtable.
    static void rehookResizeFromSwapchain(IDXGISwapChain* pSwapChain, bool isDX12);

    static void cleanupResources(bool fullReset = false);

    static void call();
};