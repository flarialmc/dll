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
#include <unknwn.h>
#include <iostream>
#include <Psapi.h>
#include <tlhelp32.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_freetype.h>
#include "unknwnbase.h"
#include "UnderUIHooks.hpp"
#include "CreateSwapchainForCoreWindowHook.hpp"
#include "ResizeHook.hpp"
#include <stb_image_write.h>
using ::IUnknown;

#include "../../../Module/Modules/MotionBlur/MotionBlur.hpp"
#include "../../../Module/Modules/DepthOfField/DepthOfFieldHelper.hpp"
#include "../../../../Assets/Assets.hpp"

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

HWND window2 = FindWindowA(nullptr, "Minecraft");

int SwapchainHook::currentBitmap;

bool unloadDll(const wchar_t *moduleName) {
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (hModule != nullptr) {
        if (FreeLibrary(hModule)) {
            Logger::debug("DLL unloaded");
            return true;
        }
        LOG_ERROR("Failed to FreeLibrary");
        return false;
    }
    LOG_ERROR("Failed to unload DLL");
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

    if (!window2) {
        window2 = FindWindowByTitle("Minecraft");
    }

    if (!window2) {
        window2 = FindWindowByTitle("Flarial");
    }

    if (kiero::getRenderType() == kiero::RenderType::D3D12) {
        kiero::bind(140, (void **) &funcOriginal, (void *) swapchainCallback);
    } else if (kiero::getRenderType() == kiero::RenderType::D3D11) {
        kiero::bind(8, (void **) &funcOriginal, (void *) swapchainCallback);
    }

    /* CREATE SWAPCHAIN FOR CORE WINDOW HOOK */

    IDXGIFactory2 *pFactory = NULL;
    CreateDXGIFactory(IID_PPV_ARGS(&pFactory));
    if (!pFactory) LOG_ERROR("Factory not created");

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

    if (D2D::context) MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);


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
    } catch (const std::exception &ex) { LOG_ERROR("Fail at loading all images: ", ex.what()); }


    if (currentVsyncState) {
        return funcOriginal(pSwapChain, 0, DXGI_PRESENT_ALLOW_TEARING);
    }

    return funcOriginal(pSwapChain, syncInterval, flags);

}

void SwapchainHook::DX11Init() {
    Logger::debug("Initializing for DX11");

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
        ImGui_ImplWin32_Init(window2);
        ImGui_ImplDX11_Init(d3d11Device, context);
        initImgui = true;

    }

    SaveBackbuffer();

    Blur::InitializePipeline();
    MotionBlur::initted = AvgPixelMotionBlurHelper::Initialize();
    MotionBlur::initted = RealMotionBlurHelper::Initialize();
    DepthOfFieldHelper::InitializePipeline();

    Memory::SafeRelease(eBackBuffer);
    init = true;
}


void SwapchainHook::DX12Init() {


    Logger::debug("Initializing for DX12");
    ID3D12Device5 *device;
    swapchain->GetDevice(IID_PPV_ARGS(&d3d12Device5));

    if (SUCCEEDED(swapchain->GetDevice(IID_PPV_ARGS(&device))) &&
        kiero::getRenderType() == kiero::RenderType::D3D12) {

        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        D3D11On12CreateDevice(device,
                              D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
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
        DepthOfFieldHelper::InitializePipeline();
        init = true;
    }
}

void SwapchainHook::DX11Render(bool underui) {

    DX11Blur();

    SaveBackbuffer(underui);

    if (!D2D::context) return;

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
    if (!D2D::context) return;
    SaveBackbuffer();
    D2D::context->SetTarget(D2D1Bitmaps[currentBitmap]);
    MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);

    DX12Blur();


    D2D::context->BeginDraw();


    DXGI_SWAP_CHAIN_DESC sdesc;
    swapchain->GetDesc(&sdesc);

    buffersCounts = sdesc.BufferCount;
    frameContexts.resize(buffersCounts);

    D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
    descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descriptorImGuiRender.NumDescriptors = TOTAL_CONSOLIDATED_DESCRIPTORS; // Consolidated heap for ImGui + Images
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

                    ImGui_ImplWin32_Init(window2);

                    // Set up modern ImGui D3D12 initialization with consolidated descriptor heap
                    ImGui_ImplDX12_InitInfo init_info = {};
                    init_info.Device = d3d12Device5;
                    init_info.CommandQueue = queue;
                    init_info.NumFramesInFlight = buffersCounts;
                    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
                    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
                    init_info.SrvDescriptorHeap = d3d12DescriptorHeapImGuiRender;
                    
                    // Set up descriptor allocation callbacks for consolidated heap
                    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
                        std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
                        
                        // ImGui font descriptor is always allocated at index 0 (reserved)
                        UINT fontDescriptorIndex = 0;
                        UINT handle_increment = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                        
                        D3D12_CPU_DESCRIPTOR_HANDLE cpu = d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
                        cpu.ptr += (handle_increment * fontDescriptorIndex);
                        
                        D3D12_GPU_DESCRIPTOR_HANDLE gpu = d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
                        gpu.ptr += (handle_increment * fontDescriptorIndex);
                        
                        *out_cpu_handle = cpu;
                        *out_gpu_handle = gpu;
                        
                        Logger::custom(fg(fmt::color::green), "DescriptorAlloc", "Allocated ImGui font descriptor at reserved index {}", fontDescriptorIndex);
                    };
                    
                    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
                        // ImGui font descriptor is persistent, no need to free
                        Logger::custom(fg(fmt::color::cyan), "DescriptorFree", "ImGui font descriptor freed (no-op)");
                    };

                    if (!ImGui_ImplDX12_Init(&init_info)) {
                        Logger::custom(fg(fmt::color::red), "ImGui", "ERROR: Failed to initialize ImGui DX12 backend with consolidated heap");
                    } else {
                        Logger::custom(fg(fmt::color::green), "ImGui", "ImGui DX12 backend initialized successfully with consolidated heap");
                    }
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
                // Bind consolidated descriptor heap (contains both ImGui and image descriptors)
                d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);
                
                static bool loggedOnce = false;
                if (!loggedOnce) {
                    loggedOnce = true;
                    Logger::custom(fg(fmt::color::green), "D3D12Render", "Using consolidated descriptor heap for Intel GPU compatibility");
                    Logger::custom(fg(fmt::color::cyan), "D3D12Render", "Consolidated heap: 0x{:X}, Total descriptors: {}", 
                                   reinterpret_cast<uintptr_t>(d3d12DescriptorHeapImGuiRender), TOTAL_CONSOLIDATED_DESCRIPTORS);
                }

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
        // Debug: Check if this function is being called
        static int callCount = 0;
        if (callCount < 5) {
            OutputDebugStringA(("SwapchainHook::DX11Blur called, count: " + std::to_string(callCount) + "\n").c_str());
            callCount++;
        }
        auto motionBlurModule = ModuleManager::getModule("Motion Blur");
        auto depthOfFieldModule = ModuleManager::getModule("Depth Of Field");

        // Debug: Check if modules are found
        static bool loggedModules = false;
        if (!loggedModules) {
            OutputDebugStringA(("Found Motion Blur: " + std::to_string(motionBlurModule != nullptr) +
                               ", Found Depth Of Field: " + std::to_string(depthOfFieldModule != nullptr) + "\n").c_str());
            if (depthOfFieldModule) {
                OutputDebugStringA(("Depth Of Field enabled: " + std::to_string(depthOfFieldModule->isEnabled()) + "\n").c_str());
            }
            loggedModules = true;
        }

        bool needsBackBuffer = FlarialGUI::inMenu;
        if (motionBlurModule && motionBlurModule->isEnabled()) needsBackBuffer = true;
        if (depthOfFieldModule && depthOfFieldModule->isEnabled()) {
            needsBackBuffer = true;
            // Add a one-time debug message when DepthOfField sets the flag
            static bool loggedDepthOfField = false;
            if (!loggedDepthOfField) {
                OutputDebugStringA("SwapchainHook: DepthOfField enabled, setting needsBackBuffer = true\n");
                loggedDepthOfField = true;
            }
        }

        FlarialGUI::needsBackBuffer = needsBackBuffer;
    }
    /* Blur End */

}

void SwapchainHook::DX12Blur() {

    /* Blur Stuff */
    prepareBlur();
    if (ModuleManager::initialized) {
        auto motionBlurModule = ModuleManager::getModule("Motion Blur");
        auto depthOfFieldModule = ModuleManager::getModule("Depth Of Field");

        bool needsBackBuffer = FlarialGUI::inMenu;
        if (motionBlurModule && motionBlurModule->isEnabled()) needsBackBuffer = true;
        if (depthOfFieldModule && depthOfFieldModule->isEnabled()) {
            needsBackBuffer = true;
            // Add a one-time debug message when DepthOfField sets the flag
            static bool loggedDepthOfField = false;
            if (!loggedDepthOfField) {
                OutputDebugStringA("SwapchainHook: DepthOfField enabled, setting needsBackBuffer = true\n");
                loggedDepthOfField = true;
            }
        }

        FlarialGUI::needsBackBuffer = needsBackBuffer;
    }
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
        static bool fontRebuildQueued = false;

        if (FlarialGUI::DoLoadFontLater) {
            FontKey fontK = FlarialGUI::LoadFontLater;
            if (Client::settings.getSettingByName<bool>("overrideFontWeight")->value) {
                fontK.weight = FlarialGUI::GetFontWeightFromString(Client::settings.getSettingByName<std::string>("fontWeight")->value);
            }
            
            if (!FlarialGUI::FontMap[fontK]) {
                FlarialGUI::LoadFontFromFontFamily(fontK);
            }
            FlarialGUI::DoLoadFontLater = false;
        }

        auto& io = ImGui::GetIO();

        // Batch font operations to reduce expensive device object recreation
        if (FlarialGUI::HasAFontLoaded) {
            fontRebuildQueued = true;
            FlarialGUI::HasAFontLoaded = false;
        }

        // Only rebuild when actually needed and defer expensive operations
        if (fontRebuildQueued) {
            io.Fonts->Build();
            
            // Use a static frame counter to defer expensive device object recreation
            static int frameDelay = 0;
            if (++frameDelay >= 3) { // Wait 3 frames to batch multiple font loads
                if (!queue) {
                    ImGui_ImplDX11_InvalidateDeviceObjects();
                    ImGui_ImplDX11_CreateDeviceObjects();
                } else {
                    ImGui_ImplDX12_InvalidateDeviceObjects();
                    ImGui_ImplDX12_CreateDeviceObjects();
                }
                fontRebuildQueued = false;
                frameDelay = 0;
            }
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

    MC::frameTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(frameTime).count();

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

// Consolidated descriptor heap management functions
bool SwapchainHook::AllocateImageDescriptor(UINT imageId, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
    std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
    
    if (!d3d12DescriptorHeapImGuiRender || !d3d12Device5) {
        Logger::custom(fg(fmt::color::red), "DescriptorAlloc", "ERROR: Consolidated descriptor heap or device not available");
        return false;
    }
    
    // Calculate descriptor index for this image (imageId - 100 + IMGUI_FONT_DESCRIPTORS)
    UINT descriptorIndex = (imageId - 100) + IMGUI_FONT_DESCRIPTORS;
    
    if (descriptorIndex >= TOTAL_CONSOLIDATED_DESCRIPTORS) {
        Logger::custom(fg(fmt::color::red), "DescriptorAlloc", "ERROR: Image ID {} would exceed descriptor heap capacity (index {} >= {})", 
                       imageId, descriptorIndex, TOTAL_CONSOLIDATED_DESCRIPTORS);
        return false;
    }
    
    UINT handle_increment = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
    cpu.ptr += (handle_increment * descriptorIndex);
    
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
    gpu.ptr += (handle_increment * descriptorIndex);
    
    *out_cpu_handle = cpu;
    *out_gpu_handle = gpu;
    
    // Update next available index if this is beyond current usage
    if (descriptorIndex >= nextAvailableDescriptorIndex) {
        nextAvailableDescriptorIndex = descriptorIndex + 1;
    }
    

    return true;
}

void SwapchainHook::FreeImageDescriptor(UINT imageId) {
    std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
    
    // For now, we don't actually free descriptors as images are persistent
    // This could be enhanced later with proper free list management
    Logger::custom(fg(fmt::color::cyan), "DescriptorFree", "Freed descriptor for image ID {} (no-op for persistent images)", imageId);
}

void SwapchainHook::ResetDescriptorAllocation() {
    std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
    
    nextAvailableDescriptorIndex = IMGUI_FONT_DESCRIPTORS; // Reset to start after ImGui font
    Logger::custom(fg(fmt::color::yellow), "DescriptorAlloc", "Reset descriptor allocation, next available index: {}", nextAvailableDescriptorIndex);
}

// PlayerHead Descriptor Management Functions

bool SwapchainHook::AllocatePlayerHeadDescriptor(const std::string& playerName, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle, UINT* out_descriptor_id) {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);
    
    if (!d3d12DescriptorHeapImGuiRender || !d3d12Device5) {
        Logger::custom(fg(fmt::color::red), "PlayerHeadDescriptor", "ERROR: Descriptor heap or device not available");
        return false;
    }
    
    // Check if we already have a descriptor for this player
    for (auto& [descriptorId, info] : playerHeadDescriptors) {
        if (info.playerName == playerName && info.inUse) {
            info.lastUsed = std::chrono::steady_clock::now();
            
            // Calculate descriptor handles - convert playerhead ID to heap index
            UINT heapIndex = descriptorId - PLAYERHEAD_DESCRIPTOR_START + IMGUI_FONT_DESCRIPTORS + MAX_IMAGE_DESCRIPTORS;
            
            UINT handle_increment = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            
            D3D12_CPU_DESCRIPTOR_HANDLE cpu = d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
            cpu.ptr += (handle_increment * heapIndex);
            
            D3D12_GPU_DESCRIPTOR_HANDLE gpu = d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
            gpu.ptr += (handle_increment * heapIndex);
            
            *out_cpu_handle = cpu;
            *out_gpu_handle = gpu;
            *out_descriptor_id = descriptorId;
            
            Logger::custom(fg(fmt::color::green), "PlayerHeadDescriptor", "Reusing descriptor {} (heap index {}) for player '{}'", descriptorId, heapIndex, playerName);
            return true;
        }
    }
    
    UINT descriptorId;
    
    // Try to reuse a freed descriptor first
    if (!freePlayerHeadDescriptors.empty()) {
        descriptorId = freePlayerHeadDescriptors.front();
        freePlayerHeadDescriptors.pop();
        Logger::custom(fg(fmt::color::cyan), "PlayerHeadDescriptor", "Reusing freed descriptor {} for player '{}'", descriptorId, playerName);
    } else {
        // Allocate new descriptor
        static bool initialized = false;
        if (!initialized) {
            nextPlayerHeadDescriptorId = PLAYERHEAD_DESCRIPTOR_START;
            initialized = true;
        }
        
        if (nextPlayerHeadDescriptorId >= PLAYERHEAD_DESCRIPTOR_END) {
            Logger::custom(fg(fmt::color::red), "PlayerHeadDescriptor", "ERROR: No more playerhead descriptors available (reached limit of {})", MAX_PLAYERHEAD_DESCRIPTORS);
            return false;
        }
        
        descriptorId = nextPlayerHeadDescriptorId++;
        Logger::custom(fg(fmt::color::blue), "PlayerHeadDescriptor", "Allocating new descriptor {} for player '{}'", descriptorId, playerName);
    }
    
    // Calculate descriptor handles - convert playerhead ID to heap index
    UINT heapIndex = descriptorId - PLAYERHEAD_DESCRIPTOR_START + IMGUI_FONT_DESCRIPTORS + MAX_IMAGE_DESCRIPTORS;
    
    if (heapIndex >= TOTAL_CONSOLIDATED_DESCRIPTORS) {
        Logger::custom(fg(fmt::color::red), "PlayerHeadDescriptor", "ERROR: Calculated heap index {} exceeds total descriptors {} (descriptor ID {})", heapIndex, TOTAL_CONSOLIDATED_DESCRIPTORS, descriptorId);
        return false;
    }
    
    Logger::custom(fg(fmt::color::cyan), "PlayerHeadDescriptor", "Using descriptor {} -> heap index {} for player '{}'", descriptorId, heapIndex, playerName);
    
    UINT handle_increment = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
    cpu.ptr += (handle_increment * heapIndex);
    
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
    gpu.ptr += (handle_increment * heapIndex);
    
    // Store descriptor info
    playerHeadDescriptors[descriptorId] = {
        playerName,
        std::chrono::steady_clock::now(),
        true
    };
    
    *out_cpu_handle = cpu;
    *out_gpu_handle = gpu;
    *out_descriptor_id = descriptorId;
    
    return true;
}

void SwapchainHook::FreePlayerHeadDescriptor(UINT descriptorId) {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);
    
    auto it = playerHeadDescriptors.find(descriptorId);
    if (it != playerHeadDescriptors.end()) {
        Logger::custom(fg(fmt::color::yellow), "PlayerHeadDescriptor", "Freeing descriptor {} for player '{}'", descriptorId, it->second.playerName);
        it->second.inUse = false;
        freePlayerHeadDescriptors.push(descriptorId);
        playerHeadDescriptors.erase(it);
    }
}

void SwapchainHook::CleanupOldPlayerHeads(size_t maxCached) {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);
    
    if (playerHeadDescriptors.size() <= maxCached) {
        return; // No need to cleanup
    }
    
    // Collect descriptors sorted by last used time
    std::vector<std::pair<UINT, std::chrono::steady_clock::time_point>> descriptorAges;
    for (const auto& [descriptorId, info] : playerHeadDescriptors) {
        if (info.inUse) {
            descriptorAges.emplace_back(descriptorId, info.lastUsed);
        }
    }
    
    // Sort by oldest first
    std::sort(descriptorAges.begin(), descriptorAges.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
    
    // Free oldest descriptors to get under the limit
    size_t toRemove = playerHeadDescriptors.size() - maxCached;
    size_t removed = 0;
    
    for (const auto& [descriptorId, _] : descriptorAges) {
        if (removed >= toRemove) break;
        
        auto it = playerHeadDescriptors.find(descriptorId);
        if (it != playerHeadDescriptors.end()) {
            Logger::custom(fg(fmt::color::orange), "PlayerHeadDescriptor", "Cleaning up old descriptor {} for player '{}'", descriptorId, it->second.playerName);
            freePlayerHeadDescriptors.push(descriptorId);
            playerHeadDescriptors.erase(it);
            removed++;
        }
    }
    
    Logger::custom(fg(fmt::color::green), "PlayerHeadDescriptor", "Cleaned up {} old playerhead descriptors, {} remaining", removed, playerHeadDescriptors.size());
}

void SwapchainHook::ResetPlayerHeadDescriptors() {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);
    
    playerHeadDescriptors.clear();
    while (!freePlayerHeadDescriptors.empty()) {
        freePlayerHeadDescriptors.pop();
    }
    nextPlayerHeadDescriptorId = PLAYERHEAD_DESCRIPTOR_START;
    
    Logger::custom(fg(fmt::color::cyan), "PlayerHeadDescriptor", "Reset all playerhead descriptors");
}