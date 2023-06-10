#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "d2d1.h"
#include "../../../Client.hpp"
#include <d3d11on12.h>
#include <wrl/client.h>

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

ID3D12CommandQueue* SwapchainHook::queue = nullptr;

void SwapchainHook::enableHook()
{
    void* swapchain_ptr;

    swapchain_ptr = (void *)kiero::getMethodsTable()[140];

    this->manualHook(swapchain_ptr, swapchainCallback, (void **)&func_original);
}

bool SwapchainHook::init = false;

void SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags)
{

    if(!SwapchainHook::init) {



        if(SwapchainHook::queue == nullptr) {

            Logger::debug("not a DX12 device, running dx11 procedures");

            ID3D12Device5 *device;
            if(SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&device)))) {
                Logger::debug("yes");
                device->RemoveDevice();
            }

            ID3D11Device *d3d11device;

            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d11device)))) {

                ID3D11Texture2D* pBackBuffer;
                pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

                const D2D1_CREATION_PROPERTIES properties
                        {
                                D2D1_THREADING_MODE_SINGLE_THREADED,
                                D2D1_DEBUG_LEVEL_NONE,
                                D2D1_DEVICE_CONTEXT_OPTIONS_NONE
                        };

                IDXGISurface* eBackBuffer;
                pSwapChain->GetBuffer(0, IID_PPV_ARGS(&eBackBuffer));
                D2D1CreateDeviceContext(eBackBuffer, properties, &D2D::context);

                MC::windowSize.x = (float) D2D::context->GetSize().width;
                MC::windowSize.y = (float) D2D::context->GetSize().height;

                Logger::debug(std::to_string(MC::windowSize.x));
                pBackBuffer->Release();
                eBackBuffer->Release();

                SwapchainHook::init = true;

            } else {
                SwapchainHook::init = false;

            }

        }

            ID3D12Device5 *device;

            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&device))) && kiero::getRenderType() == kiero::RenderType::D3D12) {

                Logger::debug("Running initial proceedures");
                Logger::debug(std::to_string(kiero::getRenderType()));

                ID3D11Device *d3d11device;
                D3D11On12CreateDevice(
                        device,
                        D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_SINGLETHREADED |
                        D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                        nullptr,
                        0,
                        (IUnknown **) &SwapchainHook::queue,
                        1,
                        0,
                        &d3d11device,
                        &SwapchainHook::context,
                        nullptr
                );

                d3d11device->QueryInterface(IID_PPV_ARGS(&SwapchainHook::d3d11On12Device));

                D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
                ID2D1Factory7 *d2dFactory; // Update to ID2D1Factory7
                D2D1_FACTORY_OPTIONS factoryOptions{}; // Add factoryOptions
                D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory7), &factoryOptions,
                                  (void **) &d2dFactory); // Update to ID2D1Factory7

                IDXGIDevice *dxgiDevice;
                SwapchainHook::d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice),
                                                               (void **) &dxgiDevice); // Update to ID2D1Factory7

                ID2D1Device6 *device2;
                d2dFactory->CreateDevice(dxgiDevice, &device2);

                device2->CreateDeviceContext(deviceOptions, &D2D::context);

                DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
                pSwapChain->GetDesc1(&swapChainDescription);

                SwapchainHook::bufferCount = swapChainDescription.BufferCount;

                SwapchainHook::DXGISurfaces.resize(bufferCount);
                SwapchainHook::D3D11Resources.resize(bufferCount);
                D2D1Bitmaps.resize(bufferCount);

                auto dpi = (float) GetDpiForSystem();

                D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
                        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                        dpi,
                        dpi
                );

                D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorBackBuffers = {};
                heapDescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                heapDescriptorBackBuffers.NumDescriptors = bufferCount;
                heapDescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                heapDescriptorBackBuffers.NodeMask = 1;

                device->CreateDescriptorHeap(&heapDescriptorBackBuffers,
                                             IID_PPV_ARGS(&SwapchainHook::D3D12DescriptorHeap));

                uintptr_t rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = SwapchainHook::D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

                Logger::debug("this ran 1");

                for (int i = 0; i < bufferCount; i++) {
                    ID3D12Resource *backBufferPtr;
                    pSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferPtr));
                    device->CreateRenderTargetView(backBufferPtr, nullptr, rtvHandle);
                    rtvHandle.ptr += rtvDescriptorSize;

                    D3D12_RESOURCE_DESC backBufferDescriptor = backBufferPtr->GetDesc();
                    MC::windowSize.x = (float) backBufferDescriptor.Width;
                    MC::windowSize.y = (float) backBufferDescriptor.Height;

                    Logger::debug("this ran 2");

                    D3D11_RESOURCE_FLAGS d3d11_flags = {D3D11_BIND_RENDER_TARGET};
                    if (!SUCCEEDED(SwapchainHook::d3d11On12Device->CreateWrappedResource(backBufferPtr, &d3d11_flags,
                                                                                         D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                                         D3D12_RESOURCE_STATE_PRESENT,
                                                                                         IID_PPV_ARGS(
                                                                                                 &SwapchainHook::D3D11Resources[i]))))
                        Logger::debug("didnt succeed");

                    Logger::debug("this ran 3");

                    SwapchainHook::D3D11Resources[i]->QueryInterface(&SwapchainHook::DXGISurfaces[i]);

                    Logger::debug("this ran 4");

                    SwapchainHook::D2D1Bitmaps[i] = nullptr; // Initialize to nullptr

                    D2D::context->CreateBitmapFromDxgiSurface(
                            SwapchainHook::DXGISurfaces[i],
                            props,
                            &(SwapchainHook::D2D1Bitmaps[i])
                    );
                }

                if (D2D::context != nullptr) {
                    Logger::debug("success");
                }

                SwapchainHook::init = true;
            } else {


                Logger::debug("device not found");

            }

    } else {

        if(D2D::context != nullptr && !Client::disable) {

            if(SwapchainHook::queue != nullptr) {

                Logger::debug("this ran");

                int i = pSwapChain->GetCurrentBackBufferIndex();

                ID3D11Resource *resource = SwapchainHook::D3D11Resources[i];
                SwapchainHook::d3d11On12Device->AcquireWrappedResources(&resource, 1);

                D2D::context->SetTarget(SwapchainHook::D2D1Bitmaps[i]);

                D2D::context->BeginDraw();
                RenderEvent event;
                EventHandler::onRender(event);
                D2D::context->EndDraw();

                SwapchainHook::d3d11On12Device->ReleaseWrappedResources(&resource, 1);
                SwapchainHook::context->Flush();
            } else {

                Logger::debug("this ran 2");
                D2D::context->BeginDraw();
                RenderEvent event;
                EventHandler::onRender(event);
                D2D::context->EndDraw();

            }
        }

    }


    return func_original(pSwapChain, syncInterval, flags);


}