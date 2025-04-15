#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/Render/RenderUnderUIEvent.hpp"
#include "d2d1.h"
#include "../../../Client.hpp"
#include <d3d11on12.h>
#include <algorithm>
#include <codecvt>
#include <windows.h>
#include <iostream>
#include <Psapi.h>
#include <tlhelp32.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_freetype.h>

#include "UnderUIHooks.hpp"
#include "CreateSwapchainForCoreWindowHook.hpp"
#include "ResizeHook.hpp"

#include "../../../Module/Modules/MotionBlur/MotionBlur.hpp"

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", 0) {}

ID3D12CommandQueue *SwapchainHook::queue = nullptr;
HANDLE SwapchainHook::fenceEvent = nullptr;

bool initImgui = false;
bool allfontloaded = false;
bool first = false;
bool imguiWindowInit = false;
BOOL _ = FALSE, $ = FALSE;

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
static std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();
//
auto window = (HWND) FindWindowA(nullptr, (LPCSTR) "Minecraft");

int SwapchainHook::currentBitmap;

bool unloadDll(const wchar_t *moduleName) {
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (hModule != nullptr) {
        if (FreeLibrary(hModule)) {
            Logger::debug("DLL unloaded");
            return true;
        }
        Logger::error("Failed to FreeLibrary");
        return false;
    }
    Logger::error("Failed to unload DLL");
    return false;
}

bool containsModule(const std::wstring &moduleName) {
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

HWND FindWindowByTitle(const std::string &titlePart) {
    HWND hwnd = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        char title[256];
        GetWindowTextA(hWnd, title, sizeof(title));
        if (strstr(title, reinterpret_cast<const char *>(lParam)) != nullptr) {
            *reinterpret_cast<HWND *>(lParam) = hWnd;
            return FALSE; // Stop enumeration when found
        }
        return TRUE; // Continue enumeration
    }, reinterpret_cast<LPARAM>(titlePart.c_str()));
    return hwnd;
}

void SwapchainHook::enableHook() {

    queueReset = Client::settings.getSettingByName<bool>("recreateAtStart")->value;
    if (Client::settings.getSettingByName<bool>("killdx")->value) queueReset = true;

    if (!window) {
        window = FindWindowByTitle("Minecraft");
    }

    if (!window) {
        window = FindWindowByTitle("Flarial");
    }

    if (kiero::getRenderType() == kiero::RenderType::D3D12) {
        kiero::bind(140, (void **) &funcOriginal, (void *) swapchainCallback);
    } else if (kiero::getRenderType() == kiero::RenderType::D3D11) {
        kiero::bind(8, (void **) &funcOriginal, (void *) swapchainCallback);
    }

    /* CREATE SWAPCHAIN FOR CORE WINDOW HOOK */

    IDXGIFactory2 *pFactory = NULL;
    CreateDXGIFactory(IID_PPV_ARGS(&pFactory));
    if (!pFactory) Logger::error("Factory not created");

    CreateSwapchainForCoreWindowHook::hook(pFactory);

    /* CREATE SWAPCHAIN FOR CORE WINDOW HOOK */


    /* FORCE DX11 ON INTEL DEVICES */

    winrt::com_ptr<IDXGIAdapter> adapter;
    pFactory->EnumAdapters(0, adapter.put());
    DXGI_ADAPTER_DESC desc;
    adapter->GetDesc(&desc);
    std::wstring gpuNameW(desc.Description);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string gpuName = converter.to_bytes(gpuNameW);
    MC::GPU = gpuName;
    Logger::info("GPU name: {}", gpuName.c_str());
    if (gpuName.contains("Intel") && Client::settings.getSettingByName<bool>("forceIntel")->value) {
        queueReset = true;
        Client::settings.getSettingByName<bool>("killdx")->value = true;
    }
    /* FORCE DX11 ON INTEL DEVICES */


    Memory::SafeRelease(pFactory);

    /* OVERLAYS CHECK */

    bool isRTSS = containsModule(L"RTSSHooks64.dll");

    if (isRTSS) {
        // if(!unloadDll(L"RTSSHooks64.dll")) { // causes a crash sometimes
        Logger::debug("[Swapchain] MSI Afterburner failed to unload!");
        Utils::MessageDialogW(L"Flarial: client failed to initialize, disable MSI Afterburner or RTSS!", L"Error!");
        ModuleManager::terminate();
        Client::disable = true;
        // }
    }

    /* OVERLAYS CHECK */

}

bool SwapchainHook::init = false;
bool SwapchainHook::currentVsyncState;


HRESULT SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {

    if (Client::disable) return funcOriginal(pSwapChain, syncInterval, flags);

    if (currentVsyncState != Client::settings.getSettingByName<bool>("vsync")->value) {
        queueReset = true;
    }

    if (queueReset) {
        init = false;
        initImgui = false;
        Logger::debug("Resetting SwapChain");
        ResizeHook::cleanShit(false);
        return DXGI_ERROR_DEVICE_RESET;
    }

    swapchain = pSwapChain;
    flagsreal = flags;

    UnderUIHooks::index = 0;

    if (D2D::context)
    MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);


    /* UNDER UI HOOK - CLEARDEPTHSTENCILVIEW */

    static bool hooker = false;

    if (!hooker && ((queue && DX12CommandLists) || (!queue && context))) {
        UnderUIHooks hook;
        hook.enableHook();
        hooker = true;
    }

    /* UNDER UI HOOK - CLEARDEPTHSTENCILVIEW */

    FPSMeasure();

    if (!init) {
        /* INIT START */

        if (queue == nullptr) {

            DX11Init();

        } else {

            DX12Init();

        }

        /* INIT END */

    } else {

        /* EACH FRAME STUFF */
        /* RENDERING START */


        /* FONT */
        Fonts();
        /* FONT */

        /* RENDER SYNC */
        RenderSync();
        /* RENDER SYNC */


        if (D2D::context != nullptr && !Client::disable) {

            if (queue != nullptr) {

                DX12Render();

            } else {

                DX11Render();

            }
        }
    }

    /* EACH FRAME STUFF */

    try {
        if (init && initImgui && !FlarialGUI::hasLoadedAll) { FlarialGUI::LoadAllImages(); FlarialGUI::hasLoadedAll = true; }
    } catch (const std::exception &ex) { Logger::error("Fail at loading all images: ", ex.what()); }


    if (currentVsyncState) {
        return funcOriginal(pSwapChain, 0, DXGI_PRESENT_ALLOW_TEARING);
    }

    return funcOriginal(pSwapChain, syncInterval, flags);

}

void SwapchainHook::DX11Init() {
    Logger::debug("Not a DX12 device, running DX11 procedures");

    const D2D1_CREATION_PROPERTIES properties
            {
                    D2D1_THREADING_MODE_MULTI_THREADED,
                    D2D1_DEBUG_LEVEL_NONE,
                    D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
            };

    swapchain->GetDevice(IID_PPV_ARGS(&d3d11Device));
    IDXGISurface1 *eBackBuffer;
    swapchain->GetBuffer(0, IID_PPV_ARGS(&eBackBuffer));

    if(!eBackBuffer) return;

    D2D1CreateDeviceContext(eBackBuffer, properties, &D2D::context);

    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0, 96.0);
    D2D::context->CreateBitmapFromDxgiSurface(eBackBuffer, props, &D2D1Bitmap);
    //ImGui Init

    if (!initImgui) {
        ImGui::CreateContext();

        d3d11Device->GetImmediateContext(&context);
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(d3d11Device, context);
        initImgui = true;

    }

    SaveBackbuffer();

    Blur::InitializePipeline();
    MotionBlur::initted = AvgPixelMotionBlurHelper::Initialize();
    MotionBlur::initted = RealMotionBlurHelper::Initialize();

    Memory::SafeRelease(eBackBuffer);
    init = true;
}


void SwapchainHook::DX12Init() {

    ID3D12Device5 *device;
    swapchain->GetDevice(IID_PPV_ARGS(&d3d12Device5));

    if (SUCCEEDED(swapchain->GetDevice(IID_PPV_ARGS(&device))) &&
        kiero::getRenderType() == kiero::RenderType::D3D12) {

        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        D3D11On12CreateDevice(device,
                              D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                              (IUnknown **) &queue, 1, 0, &SwapchainHook::d3d11Device, &context,
                              nullptr);

        if(!d3d11Device) return;

        d3d11Device->QueryInterface(IID_PPV_ARGS(&d3d11On12Device));

        D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
        ID2D1Factory7 *d2dFactory;
        D2D1_FACTORY_OPTIONS factoryOptions{};
        D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory7), &factoryOptions,
                          (void **) &d2dFactory);

        IDXGIDevice *dxgiDevice;
        d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void **) &dxgiDevice);

        ID2D1Device6 *device2;
        d2dFactory->CreateDevice(dxgiDevice, &device2);

        device2->CreateDeviceContext(deviceOptions, &D2D::context);

        DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
        swapchain->GetDesc1(&swapChainDescription);

        bufferCount = swapChainDescription.BufferCount;

        DXGISurfaces.resize(bufferCount, nullptr);
        D3D11Resources.resize(bufferCount, nullptr);
        D2D1Bitmaps.resize(bufferCount, nullptr);

        auto dpi = (float) GetDpiForSystem();

        D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

        D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorBackBuffers = {};
        heapDescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDescriptorBackBuffers.NumDescriptors = bufferCount;
        heapDescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDescriptorBackBuffers.NodeMask = 1;

        device->CreateDescriptorHeap(&heapDescriptorBackBuffers,
                                     IID_PPV_ARGS(&D3D12DescriptorHeap));

        uintptr_t rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

        for (int i = 0; i < bufferCount; i++) {

            ID3D12Resource *backBufferPtr;
            swapchain->GetBuffer(i, IID_PPV_ARGS(&backBufferPtr));
            device->CreateRenderTargetView(backBufferPtr, nullptr, rtvHandle);
            rtvHandle.ptr += rtvDescriptorSize;


            D3D11_RESOURCE_FLAGS d3d11_flags = {D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE};

            d3d11On12Device->CreateWrappedResource(backBufferPtr, &d3d11_flags,
                                                   D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                   D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(
                                                           &D3D11Resources[i]));
            D3D11Resources[i]->QueryInterface(&DXGISurfaces[i]);

            D2D1Bitmaps[i] = nullptr; // Initialize to nullptr

            D2D::context->CreateBitmapFromDxgiSurface(DXGISurfaces[i], props,
                                                      &(D2D1Bitmaps[i]));
            Memory::SafeRelease(backBufferPtr);

        }

        Memory::SafeRelease(device);
        Memory::SafeRelease(device2);
        Memory::SafeRelease(dxgiDevice);
        Memory::SafeRelease(d2dFactory);

        Blur::InitializePipeline();
        init = true;
    }
}

void SwapchainHook::DX11Render(bool underui) {

    DX11Blur();

    SaveBackbuffer(underui);

    D2D::context->BeginDraw();

    ID3D11RenderTargetView *mainRenderTargetView = nullptr;
    ID3D11Texture2D *pBackBuffer = nullptr;

    ID3D11RenderTargetView* originalRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    UINT numRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
    ID3D11DepthStencilView* originalDepthStencilView = nullptr;
    context->OMGetRenderTargets(numRenderTargets, originalRenderTargetViews, &originalDepthStencilView);




    if (context) {

        if (SUCCEEDED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer))) {

            if (SUCCEEDED(d3d11Device->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView))) {

                ImGui_ImplDX11_NewFrame();
                ImGui_ImplWin32_NewFrame();

                ImGui::NewFrame();
                ImGui::Begin("t", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);



                if (!underui) {

                    auto event = nes::make_holder<RenderEvent>();
                    event->RTV = mainRenderTargetView;
                    eventMgr.trigger(event);

                } else {

                    auto event = nes::make_holder<RenderUnderUIEvent>();
                    event->RTV = mainRenderTargetView;
                    eventMgr.trigger(event);

                }


                if (!first && SwapchainHook::init && ModuleManager::getModule("ClickGUI")) {
                    FlarialGUI::Notify(
                            "Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                    "keybind")->value + " to open the menu in-game.");

                    FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
                    first = true;
                }

                D2D::context->EndDraw();

                ImGui::End();
                ImGui::EndFrame();
                ImGui::Render();

                context->OMSetRenderTargets(1, &mainRenderTargetView, originalDepthStencilView);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                //context->Flush();
            }
        }
    }

    if (pBackBuffer) pBackBuffer->Release();

    if (mainRenderTargetView) mainRenderTargetView->Release();

    context->OMSetRenderTargets(numRenderTargets, originalRenderTargetViews, originalDepthStencilView);

    if (originalDepthStencilView) originalDepthStencilView->Release();
    for (UINT i = 0; i < numRenderTargets; ++i) {
        if (originalRenderTargetViews[i]) originalRenderTargetViews[i]->Release();
    }

}


void SwapchainHook::DX12Render(bool underui) {
    currentBitmap = (int) swapchain->GetCurrentBackBufferIndex();

    ID3D12Fence* fence;

    UINT64 fenceValue = 0;
    d3d12Device5->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

    ID3D11Resource *resource = D3D11Resources[currentBitmap];
    d3d11On12Device->AcquireWrappedResources(&resource, 1);

    SaveBackbuffer();
    D2D::context->SetTarget(D2D1Bitmaps[currentBitmap]);

    DX12Blur();

    D2D::context->BeginDraw();

    MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);

    DXGI_SWAP_CHAIN_DESC sdesc;
    swapchain->GetDesc(&sdesc);

    buffersCounts = sdesc.BufferCount;
    frameContexts.resize(buffersCounts);

    D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
    descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descriptorImGuiRender.NumDescriptors = buffersCounts;
    descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (d3d12DescriptorHeapImGuiRender or SUCCEEDED(d3d12Device5->CreateDescriptorHeap(&descriptorImGuiRender,
                                                                                       IID_PPV_ARGS(
                                                                                               &d3d12DescriptorHeapImGuiRender)))) {

        if (!allocator) d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));

        for (size_t i = 0; i < buffersCounts; i++) {
            frameContexts[i].commandAllocator = allocator;
        };

        if (!d3d12CommandList)
            d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL,
                                            IID_PPV_ARGS(&d3d12CommandList));
        if (d3d12CommandList) {

            D3D12_DESCRIPTOR_HEAP_DESC descriptorBackBuffers;
            descriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            descriptorBackBuffers.NumDescriptors = buffersCounts;
            descriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            descriptorBackBuffers.NodeMask = 1;

            if (SUCCEEDED(d3d12Device5->CreateDescriptorHeap(&descriptorBackBuffers,
                                                             IID_PPV_ARGS(&d3d12DescriptorHeapBackBuffers)))) {

                const auto rtvDescriptorSize = d3d12Device5->GetDescriptorHandleIncrementSize(
                        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

                for (size_t i = 0; i < buffersCounts; i++) {
                    ID3D12Resource *pBackBuffer = nullptr;

                    frameContexts[i].main_render_target_descriptor = rtvHandle;
                    swapchain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
                    d3d12Device5->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
                    frameContexts[i].main_render_target_resource = pBackBuffer;
                    rtvHandle.ptr += rtvDescriptorSize;

                    pBackBuffer->Release();
                };

                if (!initImgui) {
                    initImgui = true;

                    ImGui::CreateContext();

                    ImGui_ImplWin32_Init(window);

                    ImGui_ImplDX12_Init(d3d12Device5, buffersCounts,
                                        DXGI_FORMAT_R8G8B8A8_UNORM, d3d12DescriptorHeapImGuiRender,
                                        d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
                                        d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());
                }

                ImGui_ImplDX12_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();

                ImGui::Begin("t", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);

                ID3D11Texture2D *buffer2D = nullptr;
                D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(&buffer2D));

                ID3D11RenderTargetView *mainRenderTargetView;
                d3d11Device->CreateRenderTargetView(buffer2D, NULL, &mainRenderTargetView);

                if (!underui) {

                    auto event = nes::make_holder<RenderEvent>();
                    event->RTV = mainRenderTargetView;
                    eventMgr.trigger(event);

                } else {

                    auto event = nes::make_holder<RenderUnderUIEvent>();
                    event->RTV = mainRenderTargetView;
                    eventMgr.trigger(event);

                }

                if (!first && SwapchainHook::init && ModuleManager::getModule("ClickGUI")) {
                    FlarialGUI::Notify(
                            "Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                    "keybind")->value + " to open the menu in-game.");
                    FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
                    first = true;
                }


                D2D::context->EndDraw();

                D2D::context->SetTarget(nullptr);

                d3d11On12Device->ReleaseWrappedResources(&resource, 1);

                context->Flush();

                frameContexts[currentBitmap].commandAllocator->Reset();;

                D3D12_RESOURCE_BARRIER barrier;
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource = frameContexts[currentBitmap].main_render_target_resource;
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                d3d12CommandList->ResourceBarrier(1, &barrier);

                d3d12CommandList->Reset(frameContexts[swapchain->GetCurrentBackBufferIndex()].commandAllocator,
                                        nullptr);
                d3d12CommandList->ResourceBarrier(1, &barrier);
                d3d12CommandList->OMSetRenderTargets(1,
                                                     &frameContexts[currentBitmap].main_render_target_descriptor,
                                                     FALSE, nullptr);
                d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);

                ImGui::End();
                ImGui::EndFrame();
                ImGui::Render();

                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList);

                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

                d3d12CommandList->ResourceBarrier(1, &barrier);
                d3d12CommandList->Close();

                queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList *const *>(&d3d12CommandList));

                UINT64 currentFenceValue = ++fenceValue;
                queue->Signal(fence, currentFenceValue);
                if (fence->GetCompletedValue() < currentFenceValue) {
                    fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
                    WaitForSingleObject(fenceEvent, INFINITE);
                }

                Memory::SafeRelease(mainRenderTargetView);
                Memory::SafeRelease(buffer2D);
                Memory::SafeRelease(fence);
            }
        }
    }

    if (d3d12DescriptorHeapBackBuffers) {
        d3d12DescriptorHeapBackBuffers->Release();
        d3d12DescriptorHeapBackBuffers = nullptr;
    }

    if (!frameContexts.empty() && frameContexts.front().commandAllocator != nullptr) {
        frameContexts.front().commandAllocator = nullptr;
    }

    if (!frameContexts.empty() && frameContexts.front().main_render_target_resource != nullptr) {
        frameContexts.front().main_render_target_resource->Release();
        frameContexts.front().main_render_target_resource = nullptr;
    }

    frameContexts.resize(0);
}

void SwapchainHook::DX11Blur() {

    /* Blur Stuff */
    prepareBlur();
    if (ModuleManager::initialized) {
        auto module = ModuleManager::getModule("Motion Blur");
        if (module) {
            if (module->isEnabled() || FlarialGUI::inMenu) FlarialGUI::needsBackBuffer = true;
            else FlarialGUI::needsBackBuffer = false;
        }
    }
    /* Blur End */

}

void SwapchainHook::DX12Blur() {

    /* Blur Stuff */
    prepareBlur();
    if (FlarialGUI::inMenu) FlarialGUI::needsBackBuffer = true;
    else FlarialGUI::needsBackBuffer = false;
    /* Blur End */

}

void SwapchainHook::RenderSync() {
    if (init && initImgui) {
        frameTransformsMtx.lock();
        while (FrameTransforms.size() > transformDelay) {
            MC::Transform = FrameTransforms.front();
            FrameTransforms.pop();
        }
        frameTransformsMtx.unlock();
    }
}

void SwapchainHook::prepareBlur() {
    /* Blur Stuff */

    auto blurIntensity = Client::settings.getSettingByName<float>("blurintensity")->value;

    if ((ModuleManager::doesAnyModuleHave("BlurEffect") &&
        blurIntensity > 1) && !FlarialGUI::inMenu) {

        FlarialGUI::PrepareBlur(blurIntensity);

        D2D1_IMAGE_BRUSH_PROPERTIES props = D2D1::ImageBrushProperties(
                D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y));
        D2D::context->CreateImageBrush(FlarialGUI::blur_bitmap_cache, props, &FlarialGUI::blurbrush);
        }
    /* Blur End */
}


void SwapchainHook::Fonts() {
    /* IMPORTANT FONT STUFF */
    if (ImGui::GetCurrentContext()) {


        if (FlarialGUI::DoLoadFontLater) {

            FontKey fontK = FlarialGUI::LoadFontLater;
            if (Client::settings.getSettingByName<bool>("overrideFontWeight")->value) fontK.weight = FlarialGUI::GetFontWeightFromString(Client::settings.getSettingByName<std::string>("fontWeight")->value);
                if (!FlarialGUI::FontMap[fontK]) {
                    FlarialGUI::LoadFontFromFontFamily(fontK);
                }
            FlarialGUI::DoLoadFontLater = false;

        }

        //std::cout << FlarialGUI::WideToNarrow(FlarialGUI::GetFontFilePath(L"Dosis", DWRITE_FONT_WEIGHT_EXTRA_BLACK)).c_str() << std::endl;
        /*
        if(!allfontloaded) {
            FlarialGUI::LoadFonts(FlarialGUI::FontMap);

            allfontloaded = true;
        }
        */

        auto& io = ImGui::GetIO();

        if (FlarialGUI::HasAFontLoaded) {
            io.Fonts->Build();
            if (!queue) {
                ImGui_ImplDX11_InvalidateDeviceObjects();
                ImGui_ImplDX11_CreateDeviceObjects();
            }
            else {
                ImGui_ImplDX12_InvalidateDeviceObjects();
                ImGui_ImplDX12_CreateDeviceObjects();
            }

            FlarialGUI::HasAFontLoaded = false;
        }
    }

    /* IMPORTANT FONT STUFF */
}
void SwapchainHook::FPSMeasure() {

    std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - start;
    MC::frames += 1;


    if (elapsed.count() >= 0.5f) {
        MC::fps = static_cast<int>((float) MC::frames / elapsed.count());
        MC::frames = 0;
        start = std::chrono::high_resolution_clock::now();
    }

    constexpr float targetFrameRate = 60.0f;

    std::chrono::duration<float> frameTime = std::chrono::high_resolution_clock::now() - previousFrameTime;
    previousFrameTime = std::chrono::high_resolution_clock::now();

    float currentFrameRate = 1.0f / frameTime.count();

    FlarialGUI::frameFactor = targetFrameRate / currentFrameRate;

    FlarialGUI::frameFactor = std::min(FlarialGUI::frameFactor, 1.0f);

}


ID3D11Texture2D *SwapchainHook::GetBackbuffer() {
    return SavedD3D11BackBuffer;
}

void SwapchainHook::SaveBackbuffer(bool underui) {

    Memory::SafeRelease(SavedD3D11BackBuffer);
    Memory::SafeRelease(ExtraSavedD3D11BackBuffer);
    if (!SwapchainHook::queue) {

        SwapchainHook::swapchain->GetBuffer(0, IID_PPV_ARGS(&SavedD3D11BackBuffer));

        if (FlarialGUI::needsBackBuffer) {

            if (!ExtraSavedD3D11BackBuffer) {
                D3D11_TEXTURE2D_DESC textureDesc = {};
                SavedD3D11BackBuffer->GetDesc(&textureDesc);
                textureDesc.Usage = D3D11_USAGE_DEFAULT;
                textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                textureDesc.CPUAccessFlags = 0;

                SwapchainHook::d3d11Device->CreateTexture2D(&textureDesc, nullptr, &ExtraSavedD3D11BackBuffer);
            }

            if (underui) {

                if (UnderUIHooks::bgfxCtx->m_msaart) {
                    context->ResolveSubresource(ExtraSavedD3D11BackBuffer, 0, UnderUIHooks::bgfxCtx->m_msaart, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                } else {
                    context->CopyResource(ExtraSavedD3D11BackBuffer, SavedD3D11BackBuffer);
                }

            } else {
                context->CopyResource(ExtraSavedD3D11BackBuffer, SavedD3D11BackBuffer);
            }
        }


    }
    else {
        HRESULT hr;

        hr = D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(&SavedD3D11BackBuffer));
        if (FAILED(hr)) std::cout << "Failed to query interface: " << std::hex << hr << std::endl;
    }
}