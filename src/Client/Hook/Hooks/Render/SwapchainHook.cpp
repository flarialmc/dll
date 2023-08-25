#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "d2d1.h"
#include "../../../Client.hpp"
#include "../../../Module/Modules/CPS/CPSListener.hpp"
#include <d3d11on12.h>
#include <wrl/client.h>
#include <algorithm>

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

ID3D12CommandQueue* SwapchainHook::queue = nullptr;

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = fpsclock.now();
static std::chrono::steady_clock::time_point previousFrameTime = fpsclock.now();

int SwapchainHook::currentBitmap;

void SwapchainHook::enableHook()
{
    void* swapchain_ptr;

    int index;

    if(kiero::getRenderType() == kiero::RenderType::D3D12)
        index = 140;
    else index = 8;

    swapchain_ptr = (void *)kiero::getMethodsTable()[index];

    if(Client::settings.getSettingByName<bool>("killdx")->value) {
        SwapchainHook::queue = nullptr;
    }

    this->manualHook(swapchain_ptr, swapchainCallback, (void **)&func_original);
}

bool SwapchainHook::init = false;

void SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {


    SwapchainHook::swapchain = pSwapChain;
    SwapchainHook::flagsreal = flags;

    std::chrono::duration<float> elapsed = fpsclock.now() - start;
    MC::frames += 1;


    if (elapsed.count() >= 0.5f) {
        // Calculate frame rate based on elapsed time
        MC::fps = static_cast<int>(MC::frames / elapsed.count());
        // Reset frame counter and update start time
        MC::frames = 0;
        start = fpsclock.now();
    }

    constexpr float targetFrameRate = 60.0f;

// Measure the elapsed frame time
    std::chrono::duration<float> frameTime = fpsclock.now() - previousFrameTime;
    previousFrameTime = fpsclock.now();

// Calculate the current frame rate
    float currentFrameRate = 1.0f / frameTime.count();

// Calculate the frame factor as a percentage
    FlarialGUI::frameFactor = targetFrameRate / currentFrameRate;

// Limit the frame factor to a maximum of 1.0
    FlarialGUI::frameFactor = min(FlarialGUI::frameFactor, 1.0f);

    if (!SwapchainHook::init) {
        if (SwapchainHook::queue == nullptr) {

            ID3D12Device5* d3d12device3;

            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d12device3)))) {
                Logger::debug("Removed d3d12 device");
                d3d12device3->RemoveDevice();

                return func_original(pSwapChain, syncInterval, flags);
            }

            Logger::debug("not a DX12 device, running dx11 procedures");

                    const D2D1_CREATION_PROPERTIES properties
                            {
                                    D2D1_THREADING_MODE_MULTI_THREADED,
                                    D2D1_DEBUG_LEVEL_NONE,
                                    D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
                            };

                    IDXGISurface1 *eBackBuffer;
                    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&eBackBuffer));

                    D2D1CreateDeviceContext(eBackBuffer, properties, &D2D::context);

                    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0, 96.0);
                    D2D::context->CreateBitmapFromDxgiSurface(eBackBuffer, props, &SwapchainHook::D2D1Bitmap);

                    Memory::SafeRelease(eBackBuffer);

                    SwapchainHook::init = true;


        } else {

            ID3D12Device5 *device;

            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&device))) &&
                kiero::getRenderType() == kiero::RenderType::D3D12) {
                ID3D11Device *d3d11device;
                D3D11On12CreateDevice(device,
                                              D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                      (IUnknown **) &SwapchainHook::queue, 1, 0, &d3d11device, &SwapchainHook::context,
                                      nullptr);

                d3d11device->QueryInterface(IID_PPV_ARGS(&SwapchainHook::d3d11On12Device));

                D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
                ID2D1Factory7 *d2dFactory;
                D2D1_FACTORY_OPTIONS factoryOptions{};
                D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory7), &factoryOptions,
                                  (void **) &d2dFactory);

                IDXGIDevice *dxgiDevice;
                SwapchainHook::d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void **) &dxgiDevice);

                ID2D1Device6 *device2;
                d2dFactory->CreateDevice(dxgiDevice, &device2);

                device2->CreateDeviceContext(deviceOptions, &D2D::context);

                Logger::debug("okay so far");

                DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
                pSwapChain->GetDesc1(&swapChainDescription);

                SwapchainHook::bufferCount = swapChainDescription.BufferCount;

                SwapchainHook::DXGISurfaces.resize(SwapchainHook::bufferCount, nullptr);
                SwapchainHook::D3D11Resources.resize(SwapchainHook::bufferCount, nullptr);
                SwapchainHook::D2D1Bitmaps.resize(SwapchainHook::bufferCount, nullptr);

                auto dpi = (float) GetDpiForSystem();

                D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
                        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

                D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorBackBuffers = {};
                heapDescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                heapDescriptorBackBuffers.NumDescriptors = SwapchainHook::bufferCount;
                heapDescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                heapDescriptorBackBuffers.NodeMask = 1;

                device->CreateDescriptorHeap(&heapDescriptorBackBuffers,
                                             IID_PPV_ARGS(&SwapchainHook::D3D12DescriptorHeap));

                uintptr_t rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = SwapchainHook::D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

                for (int i = 0; i < SwapchainHook::bufferCount; i++) {

                    ID3D12Resource *backBufferPtr;
                    pSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferPtr));
                    device->CreateRenderTargetView(backBufferPtr, nullptr, rtvHandle);
                    rtvHandle.ptr += rtvDescriptorSize;


                    D3D11_RESOURCE_FLAGS d3d11_flags = {D3D11_BIND_RENDER_TARGET};

                    SwapchainHook::d3d11On12Device->CreateWrappedResource(backBufferPtr, &d3d11_flags,
                                                                          D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                          D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(
                                                                                  &SwapchainHook::D3D11Resources[i]));
                    SwapchainHook::D3D11Resources[i]->QueryInterface(&SwapchainHook::DXGISurfaces[i]);

                    SwapchainHook::D2D1Bitmaps[i] = nullptr; // Initialize to nullptr

                    D2D::context->CreateBitmapFromDxgiSurface(SwapchainHook::DXGISurfaces[i], props,
                                                              &(SwapchainHook::D2D1Bitmaps[i]));
                    Memory::SafeRelease(backBufferPtr);

                }

                Memory::SafeRelease(device);
                Memory::SafeRelease(device2);
                Memory::SafeRelease(d3d11device);
                Memory::SafeRelease(dxgiDevice);
                Memory::SafeRelease(d2dFactory);

                SwapchainHook::init = true;
            }
        }



    } else {


        if(FlarialGUI::writeFactory == nullptr) {
            DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                reinterpret_cast<IUnknown **>(&FlarialGUI::writeFactory));
        }


        if(D2D::context != nullptr && !Client::disable && SwapchainHook::init && FlarialGUI::writeFactory != nullptr) {

            if(SwapchainHook::queue != nullptr) {

                SwapchainHook::currentBitmap = pSwapChain->GetCurrentBackBufferIndex();

                ID3D11Resource *resource = SwapchainHook::D3D11Resources[SwapchainHook::currentBitmap];
                SwapchainHook::d3d11On12Device->AcquireWrappedResources(&resource, 1);

                D2D::context->SetTarget(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap]);

                /* Blur Stuff */

                if(ModuleManager::doesAnyModuleHave("BlurEffect") && Client::settings.getSettingByName<float>("blurintensity")->value > 1 || !FlarialGUI::notifications.empty() && Client::settings.getSettingByName<float>("blurintensity")->value > 1) {
                    ID2D1Bitmap *bitmap = nullptr;

                    if (FlarialGUI::blur == nullptr) {
                        D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &FlarialGUI::blur);
                    }

                    if (SwapchainHook::queue != nullptr)
                        FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
                    else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

                    FlarialGUI::blur->SetInput(0, bitmap);

                    // Set blur intensity
                    FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
                    FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
                                               Client::settings.getSettingByName<float>("blurintensity")->value);
                    FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);


                    ID2D1Image *image;
                    FlarialGUI::blur->GetOutput(&image);
                    D2D1_IMAGE_BRUSH_PROPERTIES props = D2D1::ImageBrushProperties(
                            D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y));
                    D2D::context->CreateImageBrush(image, props, &FlarialGUI::blurbrush);



                    Memory::SafeRelease(image);
                    Memory::SafeRelease(bitmap);
                }

                /* Blur End */

                D2D::context->BeginDraw();

                MC::windowSize = Vec2<float>(D2D::context->GetSize().width, D2D::context->GetSize().height);

                RenderEvent event;
                EventHandler::onRender(event);


                D2D::context->EndDraw();

                D2D::context->SetTarget(nullptr);

                SwapchainHook::d3d11On12Device->ReleaseWrappedResources(&resource, 1);

                SwapchainHook::context->Flush();

            } else {

                /* Blur Stuff */

                if(ModuleManager::doesAnyModuleHave("BlurEffect") && Client::settings.getSettingByName<float>("blurintensity")->value > 1 || !FlarialGUI::notifications.empty() && Client::settings.getSettingByName<float>("blurintensity")->value > 1) {
                    ID2D1Bitmap *bitmap = nullptr;

                    if (FlarialGUI::blur == nullptr) {
                        D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &FlarialGUI::blur);
                    }

                    if (SwapchainHook::queue != nullptr)
                        FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
                    else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

                    FlarialGUI::blur->SetInput(0, bitmap);

                    // Set blur intensity
                    FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
                    FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
                                               Client::settings.getSettingByName<float>("blurintensity")->value);
                    FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);


                    ID2D1Image *image;
                    FlarialGUI::blur->GetOutput(&image);
                    D2D1_IMAGE_BRUSH_PROPERTIES props = D2D1::ImageBrushProperties(
                            D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y));
                    D2D::context->CreateImageBrush(image, props, &FlarialGUI::blurbrush);



                    Memory::SafeRelease(image);
                    Memory::SafeRelease(bitmap);
                }
                /* Blur End */

                D2D::context->BeginDraw();

                MC::windowSize = Vec2<float>(D2D::context->GetSize().width, D2D::context->GetSize().height);

                RenderEvent event;
                EventHandler::onRender(event);
                D2D::context->EndDraw();
            }

            Memory::SafeRelease(FlarialGUI::blurbrush);
            Memory::SafeRelease(FlarialGUI::blur);


        }
    }

    if (Client::settings.getSettingByName<bool>("vsync")->value) return func_original(pSwapChain, 0, 8);
    else return func_original(pSwapChain, syncInterval, flags);

}