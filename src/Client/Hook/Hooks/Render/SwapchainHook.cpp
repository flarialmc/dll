#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "d2d1_1.h"
#include "../../../Client.hpp"

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

void SwapchainHook::enableHook()
{
    auto swapchain_ptr = (void *)kiero::getMethodsTable()[140];

    if(kiero::getRenderType() == kiero::RenderType::D3D11)
        swapchain_ptr = (void *)kiero::getMethodsTable()[8];


    this->manualHook(swapchain_ptr, swapchainCallback, (void **)&func_original);
}

bool SwapchainHook::init = false;

void SwapchainHook::swapchainCallback(IDXGISwapChain *pSwapChain, UINT syncInterval, UINT flags)
{

    if(!SwapchainHook::init) {

        ID3D12Device5 *device;
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&device)))) {

            device->RemoveDevice();

        }


        ID3D11Device *d3d11device;

        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d11device)))) {

            // Create the D2D factory
            ID2D1Factory* factory;
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);

            // Set up the D2D render target using the back buffer
            IDXGISurface* dxgiBackbuffer;
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackbuffer));
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
            factory->CreateDxgiSurfaceRenderTarget(dxgiBackbuffer, props, &D2D::context);
            dxgiBackbuffer->Release();
            dxgiBackbuffer = nullptr;

            factory->Release();
            factory = nullptr;

            MC::windowSize.x = D2D::context->GetSize().width;
            MC::windowSize.y = D2D::context->GetSize().height;

            Logger::debug(std::to_string(MC::windowSize.x));

            SwapchainHook::init = true;
        }

    } else {

        if(D2D::context != nullptr && !Client::disable) {

            D2D::context->BeginDraw();
            RenderEvent event;
            EventHandler::onRender(event);
            D2D::context->EndDraw();
        }

    }


    return func_original(pSwapChain, syncInterval, flags);


}