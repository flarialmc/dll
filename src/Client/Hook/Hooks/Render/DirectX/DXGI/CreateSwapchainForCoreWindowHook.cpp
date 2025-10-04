#include "CreateSwapchainForCoreWindowHook.hpp"
#include <winrt/base.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <windows.h>
#include "ResizeHook.hpp"
#include "Events/Events.hpp"
using ::IUnknown;


// ==============================
// CreateSwapChainForCoreWindow hook
// ==============================

HRESULT CreateSwapchainForCoreWindowHook::CreateSwapChainForCoreWindowCallback(
    IDXGIFactory2 *This,
    ::IUnknown *pDevice,
    IUnknown *pWindow,
    DXGI_SWAP_CHAIN_DESC1 *pDesc,
    IDXGIOutput *pRestrictToOutput,
    IDXGISwapChain1 **ppSwapChain)
{
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
    ResizeHook::cleanShit(true);
    SwapchainHook::queue = nullptr;
    SwapchainHook::swapchain = nullptr;

    winrt::com_ptr<ID3D12CommandQueue> pCommandQueue;
    Logger::debug("Recreating Swapchain");
    if (Client::settings.getSettingByName<bool>("killdx")->value) SwapchainHook::queue = nullptr;
    if (Client::settings.getSettingByName<bool>("killdx")->value && SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(pCommandQueue.put())))) {
        SwapchainHook::queue = nullptr;
        SwapchainHook::isDX12 = false;
        Logger::success("Fell back to DX11");
        return DXGI_ERROR_INVALID_CALL;
    }

    auto vsync = Client::settings.getSettingByName<bool>("vsync")->value;
    SwapchainHook::currentVsyncState = vsync;

    if (vsync) pDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
//
    SwapchainHook::recreate = false;
    HRESULT hr = funcOriginal(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
    if (FAILED(hr)) {
        Logger::error("Failed to create swapchain: {}", Logger::getHRESULTError(hr));
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