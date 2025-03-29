
#include "CreateSwapchainForCoreWindowHook.hpp"

HRESULT CreateSwapchainForCoreWindowHook::CreateSwapChainForCoreWindowCallback(IDXGIFactory2 *This, IUnknown *pDevice, IUnknown *pWindow,
                                                    DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput,
                                                    IDXGISwapChain1 **ppSwapChain) {


    ID3D12CommandQueue *pCommandQueue = NULL;
    if (Client::settings.getSettingByName<bool>("killdx")->value) SwapchainHook::queue = nullptr;
    if (Client::settings.getSettingByName<bool>("killdx")->value && SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&pCommandQueue)))) {
        pCommandQueue->Release();
        SwapchainHook::queue = nullptr;
        Logger::success("Fell back to DX11");
        return DXGI_ERROR_INVALID_CALL;
    }

    pDesc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;


    if (Client::settings.getSettingByName<bool>("killdx")->value) {
        std::string bufferingMode = Client::settings.getSettingByName<std::string>("bufferingmode")->value;

        if (bufferingMode == "Double Buffering" && !SwapchainHook::queue) {
            pDesc->BufferCount = 2;
        } else if (bufferingMode == "Triple Buffering") {
            pDesc->BufferCount = 3;
        }

        std::string swapEffect = Client::settings.getSettingByName<std::string>("swapeffect")->value;

        if (swapEffect == "FLIP_SEQUENTIAL") {
            pDesc->SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        } else if (swapEffect == "FLIP_DISCARD") {
            pDesc->SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        }
    }

    auto vsync = Client::settings.getSettingByName<bool>("vsync")->value;
    SwapchainHook::currentVsyncState = vsync;

    if(vsync) pDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;


    SwapchainHook::queueReset = false;
    return funcOriginal(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}

void CreateSwapchainForCoreWindowHook::hook(IDXGIFactory2 *pFactory) {
    Memory::hookFunc((*(LPVOID **) pFactory)[16], (void*)(CreateSwapChainForCoreWindowCallback),
                     (void **) &funcOriginal, "CreateSwapchainForCoreWindow");
}
