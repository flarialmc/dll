#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "d2d1.h"
#include "../../../Client.hpp"
#include "../../../Module/Modules/CPS/CPSListener.hpp"
#include <d3d11on12.h>
#include <algorithm>
#include <windows.h>
#include <iostream>
#include <Psapi.h>

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

ID3D12CommandQueue *SwapchainHook::queue = nullptr;

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
static std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

int SwapchainHook::currentBitmap;
bool unloadDll(const wchar_t* moduleName) {
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (hModule != nullptr) {
        if (FreeLibrary(hModule)) {
            Logger::debug("[UNLOAD DLL] DLL unloaded!");
            return true;
        } else {
            Logger::debug("[UNLOAD DLL] Failed to FreeLibrary!");
            return false;
        }
    } else {
        Logger::debug("[UNLOAD DLL] Failed to unload!");
        return false;
    }
}

bool containsModule(const std::wstring& moduleName) {
    // Get the handle to the current process
    HANDLE hProcess = GetCurrentProcess();
    HMODULE hMods[1024];
    DWORD cbNeeded;

    // Enumerate modules in the process
    if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                std::wstring baseModuleName = std::filesystem::path(szModName).filename().wstring();
                // Compare the base module name with the given module name
                if (moduleName == baseModuleName) {
                    return true;
                }
            }
        }
    }

    return false;
}

void SwapchainHook::enableHook() {


    if (kiero::getRenderType() == kiero::RenderType::D3D12) {
        kiero::bind(140, (void**)&funcOriginal, (void*)swapchainCallback);
    }
    else if (kiero::getRenderType() == kiero::RenderType::D3D11) {
        kiero::bind(8, (void**)&funcOriginal, (void*)swapchainCallback);
    }

    bool isRTSS = containsModule(L"RTSSHooks64.dll");

    if(isRTSS) {
        // if(!unloadDll(L"RTSSHooks64.dll")) { // causes a crash sometimes
        // Logger::debug("[Swapchain] MSI Afterburner failed to unload!");
        MessageBox(NULL, "Flarial: client failed to initialize, disable MSI Afterburner!", "", MB_OK);
        ModuleManager::terminate();
        Client::disable = true;
        // }
    }
}

bool SwapchainHook::init = false;

void prepareBlur() {
    /* Blur Stuff */

    auto blurIntensity = Client::settings.getSettingByName<float>("blurintensity")->value;

    if ((ModuleManager::doesAnyModuleHave("BlurEffect") &&
        blurIntensity > 1 ||
        !FlarialGUI::notifications.empty() &&
        blurIntensity > 1) && !FlarialGUI::inMenu) {

        FlarialGUI::PrepareBlur(blurIntensity);

        D2D1_IMAGE_BRUSH_PROPERTIES props = D2D1::ImageBrushProperties(
                D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y));
        D2D::context->CreateImageBrush(FlarialGUI::blur_bitmap_cache.get(), props, FlarialGUI::blurbrush.put());
    }
    /* Blur End */
}

HRESULT SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {

    SwapchainHook::swapchain = pSwapChain;
    SwapchainHook::flagsreal = flags;

    std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - start;
    MC::frames += 1;

    if (Client::settings.getSettingByName<bool>("killdx")->value) {
        SwapchainHook::queue = nullptr;
        ID3D12Device5* d3d12device3;
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d12device3)))) {
            Logger::debug("[SwapChain] Removed d3d12 device");
            pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            d3d12device3->RemoveDevice();

            return funcOriginal(pSwapChain, syncInterval, flags);
        }
    }

    if (elapsed.count() >= 0.5f) {
        // Calculate frame rate based on elapsed time
        MC::fps = static_cast<int>((float)MC::frames / elapsed.count());
        // Reset frame counter and update start time
        MC::frames = 0;
        start = std::chrono::high_resolution_clock::now();
    }

    constexpr float targetFrameRate = 60.0f;

// Measure the elapsed frame time
    std::chrono::duration<float> frameTime = std::chrono::high_resolution_clock::now() - previousFrameTime;
    previousFrameTime = std::chrono::high_resolution_clock::now();

// Calculate the current frame rate
    float currentFrameRate = 1.0f / frameTime.count();

// Calculate the frame factor as a percentage
    FlarialGUI::frameFactor = targetFrameRate / currentFrameRate;

// Limit the frame factor to a maximum of 1.0
    FlarialGUI::frameFactor = std::min(FlarialGUI::frameFactor, 1.0f);

    if (!SwapchainHook::init) {

            if(SwapchainHook::queue == nullptr) {

                Logger::debug("[SwapChain] Not a DX12 device, running dx11 procedures");

                const D2D1_CREATION_PROPERTIES properties
                {
                    D2D1_THREADING_MODE_MULTI_THREADED,
                    D2D1_DEBUG_LEVEL_NONE,
                    D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
                };

                winrt::com_ptr<IDXGISurface1> eBackBuffer;
                pSwapChain->GetBuffer(0, IID_PPV_ARGS(eBackBuffer.put()));

                D2D1CreateDeviceContext(eBackBuffer.get(), properties, D2D::context.put());

                D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
                        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0, 96.0);
                D2D::context->CreateBitmapFromDxgiSurface(eBackBuffer.get(), props, SwapchainHook::D2D1Bitmap.put());

                SwapchainHook::init = true;
            } else {

                winrt::com_ptr<ID3D12Device5> device;

            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(device.put()))) &&
                kiero::getRenderType() == kiero::RenderType::D3D12) {
                winrt::com_ptr<ID3D11Device> d3d11device;
                D3D11On12CreateDevice(device.get(),
                                      D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                      (IUnknown **) &SwapchainHook::queue, 1, 0, d3d11device.put(), SwapchainHook::context.put(),
                                      nullptr);

                d3d11device->QueryInterface(IID_PPV_ARGS(SwapchainHook::d3d11On12Device.put()));

                D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
                winrt::com_ptr<ID2D1Factory7> d2dFactory;
                D2D1_FACTORY_OPTIONS factoryOptions{};
                D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory7), &factoryOptions,
                                  (void **) d2dFactory.put());

                winrt::com_ptr<IDXGIDevice> dxgiDevice;
                SwapchainHook::d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void **) dxgiDevice.put());

                winrt::com_ptr<ID2D1Device6> device2;
                d2dFactory->CreateDevice(dxgiDevice.get(), device2.put());

                device2->CreateDeviceContext(deviceOptions, D2D::context.put());

                Logger::debug("[SwapChain] Prepared.");

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
                                             IID_PPV_ARGS(SwapchainHook::D3D12DescriptorHeap.put()));

                uintptr_t rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = SwapchainHook::D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

                for (int i = 0; i < SwapchainHook::bufferCount; i++) {

                    winrt::com_ptr<ID3D12Resource> backBufferPtr;
                    pSwapChain->GetBuffer(i, IID_PPV_ARGS(backBufferPtr.put()));
                    device->CreateRenderTargetView(backBufferPtr.get(), nullptr, rtvHandle);
                    rtvHandle.ptr += rtvDescriptorSize;


                    D3D11_RESOURCE_FLAGS d3d11_flags = {D3D11_BIND_RENDER_TARGET};

                    SwapchainHook::d3d11On12Device->CreateWrappedResource(backBufferPtr.get(), &d3d11_flags,
                                                                          D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                          D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(
                                                                                  SwapchainHook::D3D11Resources[i].put()));
                    SwapchainHook::D3D11Resources[i]->QueryInterface(SwapchainHook::DXGISurfaces[i].put());

                    SwapchainHook::D2D1Bitmaps[i] = nullptr; // Initialize to nullptr

                    D2D::context->CreateBitmapFromDxgiSurface(SwapchainHook::DXGISurfaces[i].get(), props,
                                                              SwapchainHook::D2D1Bitmaps[i].put());
                }
                SwapchainHook::init = true;
            }
        }


    } else {

        frameTransformsMtx.lock();
        while(FrameTransforms.size() > transformDelay)
        {
            MC::Transform = FrameTransforms.front();
            FrameTransforms.pop();
        }
        frameTransformsMtx.unlock();

        if (FlarialGUI::writeFactory == nullptr) {
            DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                reinterpret_cast<IUnknown **>(FlarialGUI::writeFactory.put()));
        }

        if (D2D::context != nullptr && !Client::disable && FlarialGUI::writeFactory != nullptr) {

            if (SwapchainHook::queue != nullptr) {

                SwapchainHook::currentBitmap = (int)pSwapChain->GetCurrentBackBufferIndex();

                auto resource = SwapchainHook::D3D11Resources[SwapchainHook::currentBitmap].get();
                SwapchainHook::d3d11On12Device->AcquireWrappedResources(&resource, 1);

                D2D::context->SetTarget(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap].get());

                prepareBlur();

                D2D::context->BeginDraw();

                MC::windowSize = Vec2<float>(D2D::context->GetSize().width, D2D::context->GetSize().height);

                RenderEvent event{};
                EventHandler::onRender(event);


                D2D::context->EndDraw();

                D2D::context->SetTarget(nullptr);

                SwapchainHook::d3d11On12Device->ReleaseWrappedResources(&resource, 1);

                SwapchainHook::context->Flush();

            } else {

                prepareBlur();

                D2D::context->BeginDraw();

                MC::windowSize = Vec2<float>(D2D::context->GetSize().width, D2D::context->GetSize().height);

                RenderEvent event{};
                EventHandler::onRender(event);
                D2D::context->EndDraw();

            }
        }
    }

    if (Client::settings.getSettingByName<bool>("vsync")->value) {
        return funcOriginal(pSwapChain, 0, DXGI_PRESENT_DO_NOT_WAIT);
    } else return funcOriginal(pSwapChain, syncInterval, flags);

}