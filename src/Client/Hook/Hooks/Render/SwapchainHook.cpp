#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "d2d1.h"
#include "../../../Client.hpp"
#include <d3d11on12.h>
#include <wrl/client.h>
#include <amp_short_vectors.h>

ID3D12CommandQueue* SwapchainHook::queue;

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

void SwapchainHook::enableHook()
{
    void* swapchain_ptr;

    swapchain_ptr = (void *)kiero::getMethodsTable()[140];

    this->manualHook(swapchain_ptr, swapchainCallback, (void **)&func_original);
}

bool SwapchainHook::init = false;

void SwapchainHook::swapchainCallback(IDXGISwapChain *pSwapChain, UINT syncInterval, UINT flags)
{

    if(!SwapchainHook::init) {

        Logger::debug("Running initial proceedures");

        ID3D12Device5 *device;
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&device))) && kiero::getRenderType() == kiero::RenderType::D3D12 && SwapchainHook::queue != nullptr) {

           ID3D11Device* d3d11device;
           ID3D11DeviceContext* context;

                D3D11On12CreateDevice(
                        device,
                        D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_SINGLETHREADED
                        | D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                        nullptr,
                        0,
                        (IUnknown**)&SwapchainHook::queue,
                        1,
                        0,
                        &d3d11device,
                        &context,
                        nullptr);

            ID3D11On12Device* d3d11On12Device;
            d3d11device->QueryInterface(IID_PPV_ARGS(&d3d11On12Device));

            D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
            ID2D1Factory3* d2dFactory;
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);

            IDXGIDevice* dxgiDevice;
            d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));

            ID2D1Device2* device2;
            d2dFactory->CreateDevice(dxgiDevice, &device2);

            device2->CreateDeviceContext(deviceOptions, &D2D::context);

            DXGI_SWAP_CHAIN_DESC swapChainDescription;
            pSwapChain->GetDesc(&swapChainDescription);

            int bufferCount = swapChainDescription.BufferCount;
            float dpi = GetDpiForSystem();

            D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpi, dpi);



            // DESCRIPTOR HEAP FOR RTVHANDLE

            D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorBackBuffers = {};
            heapDescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDescriptorBackBuffers.NumDescriptors = bufferCount;
            heapDescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDescriptorBackBuffers.NodeMask = 1;

            device->CreateDescriptorHeap(&heapDescriptorBackBuffers, IID_PPV_ARGS(&SwapchainHook::D3D12DescriptorHeap));

            unsigned int rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = SwapchainHook::D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();



            for (int i = 0; i < bufferCount; i++) {

            ID3D12Resource* backBufferPtr;
            pSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferPtr));

            D2D::context->CreateBitmapFromDxgiSurface(
            &SwapchainHook::DXGISurfaces[i], props, &SwapchainHook::D2D1Bitmaps[i]);

            }

            if(D2D::context != nullptr) Logger::debug("success");

            SwapchainHook::init = true;
        }


        /*
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

            SwapchainHook::init = true;

        } else {
            SwapchainHook::init = false;

        } */

    } else {

        if(D2D::context != nullptr && !Client::disable) {

            Logger::debug("omw");

            MC::windowSize.x = D2D::context->GetSize().width;
            MC::windowSize.y = D2D::context->GetSize().height;

            Logger::debug(std::to_string(MC::windowSize.x));

            D2D::context->BeginDraw();
            RenderEvent event;
            EventHandler::onRender(event);
            D2D::context->EndDraw();
        }

    }


    return func_original(pSwapChain, syncInterval, flags);


}