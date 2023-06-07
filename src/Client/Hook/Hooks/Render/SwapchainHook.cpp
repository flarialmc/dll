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

        ID3D12Device *device;
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&device)))) {

            static_cast<ID3D12Device5 *>(device)->RemoveDevice();
        }


        ID3D11Device *d3d11device = nullptr;
        ID3D11DeviceContext *context = nullptr;
        ID3D11RenderTargetView *mainRenderTargetView = nullptr;

        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **) &d3d11device))) {
            d3d11device->GetImmediateContext(&context);

            HWND window = FindWindowA(nullptr, "Minecraft");

            ID3D11Texture2D *pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **) &pBackBuffer);
            d3d11device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);

            const D2D1_CREATION_PROPERTIES properties
                    {
                            D2D1_THREADING_MODE_SINGLE_THREADED,
                            D2D1_DEBUG_LEVEL_NONE,
                            D2D1_DEVICE_CONTEXT_OPTIONS_NONE
                    };

            ID2D1DeviceContext *d2dContext = NULL;
            IDXGISurface *eBackBuffer;
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&eBackBuffer));
            D2D1CreateDeviceContext(eBackBuffer, properties, &d2dContext);
            D2D::context = d2dContext;

            MC::windowSize = *new Vec2<float>(d2dContext->GetSize().width, d2dContext->GetSize().height);

            eBackBuffer->Release();

            init = true;
        }
    } else {
            D2D::context->BeginDraw();
            RenderEvent event;
            EventHandler::onRender(event);
            D2D::context->EndDraw();
    }

    // Render Shit

    return func_original(pSwapChain, syncInterval, flags);

}