#include "CreateSwapchainForCoreWindowHook.hpp"
#include <winrt/base.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <windows.h>
#include "ResizeHook.hpp"
using ::IUnknown;


// ==============================
// CreateSwapChainForCoreWindow hook
// ==============================

// Track if we've already attempted DX12 rejection (to prevent infinite retry loops)
static bool dx12FallbackAttempted = false;

HRESULT CreateSwapchainForCoreWindowHook::CreateSwapChainForCoreWindowCallback(
    IDXGIFactory2 *This,
    ::IUnknown *pDevice,
    IUnknown *pWindow,
    DXGI_SWAP_CHAIN_DESC1 *pDesc,
    IDXGIOutput *pRestrictToOutput,
    IDXGISwapChain1 **ppSwapChain)
{
    // First, check if this is a DX12 device and capture the command queue EARLY
    // We need to do this before any cleanup that might affect the queue
    winrt::com_ptr<ID3D12CommandQueue> pCommandQueue;
    bool isDX12Device = SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(pCommandQueue.put())));

    bool killdxEnabled = Client::settings.getSettingByName<bool>("killdx")->value;

    Logger::debug(
        "[Swapchain] CreateSwapChainForCoreWindow called - killdx={}, fallbackAttempted={}, isDX12Device={}",
        killdxEnabled, dx12FallbackAttempted, isDX12Device);

    // Note: Better Frames (killdx) is now handled via RemoveDevice() in DX12Init()
    // which works on both UWP and GDK platforms

    // If we reach here, we're proceeding with swapchain creation (either DX11 or DX12 after fallback failed)
    // Now do the cleanup

    /* EXTRA RELEASING PRECAUTIONS */
    if (SwapchainHook::d3d11On12Device && !SwapchainHook::D3D11Resources.empty()) {
        std::vector<ID3D11Resource*> toRelease;
        toRelease.reserve(SwapchainHook::D3D11Resources.size());
        for (auto& res : SwapchainHook::D3D11Resources) if (res) toRelease.push_back(res.get());
        if (!toRelease.empty()) SwapchainHook::d3d11On12Device->ReleaseWrappedResources(toRelease.data(), static_cast<UINT>(toRelease.size()));
    }
    if (SwapchainHook::context) {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }
    if (D2D::context) {
        D2D::context->SetTarget(nullptr);
        D2D::context->Flush();
    }
    if (SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
        winrt::com_ptr<ID3D12Fence> fence;
        if (SUCCEEDED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
            const UINT64 value = 1;
            HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (evt) {
                SwapchainHook::queue->Signal(fence.get(), value);
                if (fence->GetCompletedValue() < value) {
                    fence->SetEventOnCompletion(value, evt);
                    WaitForSingleObject(evt, 1000);
                }
                CloseHandle(evt);
            }
        }
    }
    /* EXTRA RELEASING PRECAUTIONS */

    SwapchainHook::init = false;
    ResizeHook::cleanupResources(true);
    SwapchainHook::swapchain = nullptr;
    // Note: Don't null out queue here - we'll set it from pCommandQueue if DX12

    // Capture the command queue for DX12
    if (isDX12Device && pCommandQueue)
    {
        SwapchainHook::queue = pCommandQueue;
        Logger::success("[Swapchain] Captured DX12 command queue");
    }
    else
    {
        // Only null queue if this is NOT a DX12 device (i.e., DX11 fallback worked)
        SwapchainHook::queue = nullptr;
    }

    auto vsync = Client::settings.getSettingByName<bool>("vsync")->value;
    SwapchainHook::currentVsyncState = vsync;

    if (vsync) pDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    SwapchainHook::recreate = false;
    Logger::debug("[Swapchain] Calling original CreateSwapChainForCoreWindow...");
    HRESULT hr = funcOriginal(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);

    if (SUCCEEDED(hr))
    {
        Logger::success("[Swapchain] Created successfully (isDX12={})", isDX12Device);

        // Phase 2: Re-hook Present and Resize from the live swapchain's vtable.
        // This ensures our hooks intercept the game's actual functions, not the
        // dummy vtable addresses from kiero which may differ on some systems.
        if (ppSwapChain && *ppSwapChain) {
            SwapchainHook::rehookPresentFromSwapchain(*ppSwapChain);
            ResizeHook::rehookResizeFromSwapchain(*ppSwapChain, isDX12Device);
        }
    }
    else
    {
        Logger::error("[Swapchain] Failed to create: {} (0x{:08X})", Logger::getHRESULTError(hr),
                      static_cast<unsigned int>(hr));
        Client::settings.setValue<bool>("killdx", false);
        Client::settings.setValue<bool>("vsync", false);
    }

    return hr;
}

void CreateSwapchainForCoreWindowHook::hook(IDXGIFactory2 *pFactory) {
    // Hook CreateSwapChainForCoreWindow (existing)
    Memory::hookFunc((*(LPVOID **) pFactory)[16],
                     (void*)(CreateSwapChainForCoreWindowCallback),
                     (void **) &funcOriginal,
                     "CreateSwapchainForCoreWindow");

#if defined(__DEBUG__)
    // Also hook D3D11 & D3D12 CreateDevice exports so we can enable debug layers BEFORE device creation
    // HookD3D11Exports();
    // HookD3D12Exports();
#endif
}