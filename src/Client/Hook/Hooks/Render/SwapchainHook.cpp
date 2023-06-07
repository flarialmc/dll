#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

void SwapchainHook::enableHook()
{
    auto swapchain_ptr = (void *)kiero::getMethodsTable()[140];
    this->manualHook(swapchain_ptr, swapchainCallback, (void **)&func_original);
}

bool init = false;
void SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags)
{

    if (!init)
    {
        ID3D12Device5 *d3d12device = nullptr;
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d12device))))
        {
            Logger::debug("Removing D3D12Device");

            d3d12device->RemoveDevice();
        }
        ID3D11Device *d3d11device = nullptr;
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d11device))))
        {

            ID3D11DeviceContext *context = nullptr;
            ID3D11Texture2D *back_buffer;
            ID3D11RenderTargetView *mainRenderTargetView = nullptr;
            d3d11device->GetImmediateContext(&context);
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
            d3d11device->CreateRenderTargetView(back_buffer, NULL, &mainRenderTargetView);

            // TODO: Create d2d resources
            D2D::context = reinterpret_cast<ID2D1DeviceContext *>(context);
            D2D::device = d3d11device;
            D2D::renderTarget = mainRenderTargetView;

            back_buffer->Release();

            init = true;
        }
    } else {

        RenderEvent event;
        EventHandler::onRender(event);

    }

    // Render Shit

    return func_original(pSwapChain, syncInterval, flags);
}