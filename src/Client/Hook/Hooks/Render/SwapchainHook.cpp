#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "d2d1_1.h"

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

void SwapchainHook::enableHook()
{
    auto swapchain_ptr = (void *)kiero::getMethodsTable()[140];
    this->manualHook(swapchain_ptr, swapchainCallback, (void **)&func_original);
}

bool init = false;

void SwapchainHook::swapchainCallback(IDXGISwapChain *pSwapChain, UINT syncInterval, UINT flags)
{

    if(!init) {
        ID3D12Device5 *device;
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&device)))) {

            device->RemoveDevice();
            init = true;
        }

        ID3D11Device *d3d11device = nullptr;
        ID3D11RenderTargetView *mainRenderTargetView = nullptr;

        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **) &d3d11device))) {

            ID3D11Texture2D *pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **) &pBackBuffer);
            d3d11device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);

            const D2D1_CREATION_PROPERTIES properties
                    {
                            D2D1_THREADING_MODE_SINGLE_THREADED,
                            D2D1_DEBUG_LEVEL_NONE,
                            D2D1_DEVICE_CONTEXT_OPTIONS_NONE
                    };


            ID2D1DeviceContext *d2dContext;

            IDXGISurface *eBackBuffer;
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&eBackBuffer));
            D2D1CreateDeviceContext(eBackBuffer, properties, &d2dContext);
            D2D::context = d2dContext;

            MC::windowSize.x = D2D::context->GetSize().width;
            MC::windowSize.y = D2D::context->GetSize().height;

            eBackBuffer->Release();
            pBackBuffer->Release();
            eBackBuffer = nullptr;
            pBackBuffer = nullptr;
            d3d11device->Release();
            mainRenderTargetView->Release();
            d3d11device = nullptr;
            mainRenderTargetView = nullptr;
        }

    }

    // Render Shit
    return func_original(pSwapChain, syncInterval, flags);


}