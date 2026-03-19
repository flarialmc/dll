#include "CreateSwapChainForHwndHook.hpp"
#include "SwapchainHook.hpp"
#include "Client.hpp"
#include "ResizeHook.hpp"
#include <d3d12.h>
#include <winrt/base.h>

HRESULT CreateSwapChainForHwndHook::hkCreateSwapChainForHwnd(IDXGIFactory2 *pFactory, IUnknown *pDevice, HWND hWnd,
                                                             DXGI_SWAP_CHAIN_DESC1 *pDesc,
                                                             DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
                                                             IDXGIOutput *pRestrictToOutput,
                                                             IDXGISwapChain1 **ppSwapChain)
{
    // Detect if the device creating this swapchain is DX12
    winrt::com_ptr<ID3D12CommandQueue> pCommandQueue;
    bool isDX12Device = SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(pCommandQueue.put())));

    auto vsync = Client::settings.getSettingByName<bool>("vsync")->value;
    SwapchainHook::currentVsyncState = vsync;

    if (vsync)
        pDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    HRESULT hr = oCreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

    if (SUCCEEDED(hr)) {
        Logger::debug("[Swapchain] CreateSwapChainForHwnd succeeded (isDX12={})", isDX12Device);

        // Capture DX12 command queue if not already captured.
        // This mirrors what CreateSwapchainForCoreWindowHook does and is critical for
        // the case where kiero detected DX11 but the game is actually DX12 —
        // without the queue, DX12Init would fail silently.
        if (isDX12Device && pCommandQueue && !SwapchainHook::queue) {
            SwapchainHook::queue = pCommandQueue;
            Logger::success("[Swapchain] Captured DX12 command queue from CreateSwapChainForHwnd");
        }

        // Phase 2: Re-hook Present and Resize from the live swapchain's vtable.
        if (ppSwapChain && *ppSwapChain) {
            SwapchainHook::rehookPresentFromSwapchain(*ppSwapChain);
            ResizeHook::rehookResizeFromSwapchain(*ppSwapChain, isDX12Device);
        }
    }

    return hr;
}

void CreateSwapChainForHwndHook::Hook(IDXGIFactory2 *pFactory)
{
    Memory::hookFunc((*(LPVOID **)pFactory)[15], (void *)(hkCreateSwapChainForHwnd), (void **)&oCreateSwapChainForHwnd,
                     "CreateSwapChainForHwnd");
}
