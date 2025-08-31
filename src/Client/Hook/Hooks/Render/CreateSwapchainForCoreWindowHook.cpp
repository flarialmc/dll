
#include "CreateSwapchainForCoreWindowHook.hpp"
#include <winrt/base.h>
using ::IUnknown;

HRESULT CreateSwapchainForCoreWindowHook::CreateSwapChainForCoreWindowCallback(IDXGIFactory2 *This, ::IUnknown *pDevice, IUnknown *pWindow,
                                                    DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput,
                                                    IDXGISwapChain1 **ppSwapChain) {


    winrt::com_ptr<ID3D12CommandQueue> pCommandQueue;
    Logger::debug("Recreating Swapchain");
    if (Client::settings.getSettingByName<bool>("killdx")->value) SwapchainHook::queue = nullptr;
    if (Client::settings.getSettingByName<bool>("killdx")->value && SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(pCommandQueue.put())))) {
        SwapchainHook::queue = nullptr;
        Logger::success("Fell back to DX11");
        return DXGI_ERROR_INVALID_CALL;
    }

    auto vsync = Client::settings.getSettingByName<bool>("vsync")->value;
    SwapchainHook::currentVsyncState = vsync;

    if (vsync) pDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    // Try to get the command queue from the device for DX12
    if (!SwapchainHook::queue.get()) {
        winrt::com_ptr<ID3D12CommandQueue> tempQueue;
        if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(tempQueue.put())))) {
            SwapchainHook::queue = tempQueue;
            Logger::debug("Captured D3D12 command queue from device");
        }
    }
    
    SwapchainHook::queueReset = false;
    HRESULT hr = funcOriginal(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
    if (FAILED(hr)) {
        Logger::error("Failed to create swapchain: {}", Logger::getHRESULTError(hr));
    }

    return hr;

}

void CreateSwapchainForCoreWindowHook::hook(IDXGIFactory2 *pFactory) {
    Memory::hookFunc((*(LPVOID **) pFactory)[16], (void*)(CreateSwapChainForCoreWindowCallback),
                     (void **) &funcOriginal, "CreateSwapchainForCoreWindow");
}
