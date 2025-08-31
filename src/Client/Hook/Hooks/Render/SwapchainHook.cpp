#include "SwapchainHook.hpp"
#include <winrt/base.h>
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
using ::IUnknown;

#include "../../../Module/Modules/MotionBlur/MotionBlur.hpp"
#include "../../../../Assets/Assets.hpp"

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", 0) {}

winrt::com_ptr<ID3D12CommandQueue> SwapchainHook::queue = nullptr;
HANDLE SwapchainHook::fenceEvent = nullptr;

bool SwapchainHook::initImgui = false;
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

    winrt::com_ptr<IDXGIFactory2> pFactory;
    CreateDXGIFactory(IID_PPV_ARGS(pFactory.put()));
    if (!pFactory.get()) LOG_ERROR("Factory not created");

    CreateSwapchainForCoreWindowHook::hook(pFactory.get());

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


    // pFactory is now a smart pointer, no manual release needed

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
        queueReset = false;
        return DXGI_ERROR_DEVICE_RESET;
    }

    swapchain = pSwapChain;
    flagsreal = flags;

    UnderUIHooks::index = 0;

    if (D2D::context) MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);


    /* UNDER UI HOOK - CLEARDEPTHSTENCILVIEW */

    static bool hooker = false;

    if (!hooker && ((queue) || (!queue && context))) {
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

    // Get D3D11 device from swapchain
    if (FAILED(swapchain->GetDevice(IID_PPV_ARGS(d3d11Device.put())))) {
        Logger::error("Failed to get D3D11 device from swapchain");
        return;
    }

    // Get device context once
    d3d11Device->GetImmediateContext(context.put());
    if (!context.get()) {
        Logger::error("Failed to get D3D11 immediate context");
        return;
    }

    // Initialize D2D1
    const D2D1_CREATION_PROPERTIES properties = {
        D2D1_THREADING_MODE_MULTI_THREADED,
        D2D1_DEBUG_LEVEL_NONE,
        D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
    };

    winrt::com_ptr<IDXGISurface1> backBuffer;
    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(backBuffer.put())))) {
        Logger::error("Failed to get backbuffer");
        return;
    }

    // Create D2D context
    HRESULT hr = D2D1CreateDeviceContext(backBuffer.get(), properties, &D2D::context);
    if (FAILED(hr) || !D2D::context) {
        Logger::error("Failed to create D2D1 device context: {}", Logger::getHRESULTError(hr));
        backBuffer = nullptr;
        return;
    }

    // Create D2D1 bitmap for render target
    D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f
    );
    
    D2D::context->CreateBitmapFromDxgiSurface(backBuffer.get(), bitmapProps, D2D1Bitmap.put());
    // backBuffer is now a smart pointer, no manual release needed

    // Initialize ImGui once
    if (!initImgui && !imguiCleanupInProgress) {
        // Wait for any ongoing cleanup to complete
        while (imguiCleanupInProgress) {
            Sleep(1);
        }
        
        if (!ImGui::GetCurrentContext()) {
            ImGui::CreateContext();
        }
        
        // Check if Win32 backend is already initialized
        auto& io = ImGui::GetIO();
        if (!io.BackendPlatformUserData) {
            ImGui_ImplWin32_Init(window2);
        }
        
        // Check if DX11 backend is already initialized
        if (!io.BackendRendererUserData) {
            ImGui_ImplDX11_Init(d3d11Device.get(), context.get());
        }
        
        initImgui = true;
    }

    // Initialize blur and motion blur pipelines
    Blur::InitializePipeline();
    MotionBlur::initted = AvgPixelMotionBlurHelper::Initialize() && RealMotionBlurHelper::Initialize();

    SaveBackbuffer();
    init = true;
}


void SwapchainHook::DX12Init() {
    Logger::debug("Initializing for DX12");
    
    // Get D3D12 device
    if (FAILED(swapchain->GetDevice(IID_PPV_ARGS(d3d12Device5.put())))) {
        Logger::error("Failed to get D3D12 device from swapchain");
        return;
    }
    
    // Create fence event for synchronization
    if (!fenceEvent) {
        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!fenceEvent) {
            Logger::error("Failed to create fence event");
            return;
        }
    }
    
    // Create D3D11on12 device for compatibility
    IUnknown* queueAsUnknown = queue.get();
    HRESULT hr = D3D11On12CreateDevice(
        d3d12Device5.get(),
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0,
        &queueAsUnknown, 1, 0,
        d3d11Device.put(), context.put(), nullptr
    );
    
    if (FAILED(hr) || !d3d11Device.get()) {
        Logger::error("Failed to create D3D11on12 device: {}", Logger::getHRESULTError(hr));
        return;
    }
    
    // Get D3D11on12 interface
    d3d11Device->QueryInterface(IID_PPV_ARGS(d3d11On12Device.put()));
    
    // Create D2D resources
    ID2D1Factory7* d2dFactory = nullptr;
    D2D1_FACTORY_OPTIONS factoryOptions = {};
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_MULTI_THREADED,
        __uuidof(ID2D1Factory7),
        &factoryOptions,
        (void**)&d2dFactory
    );
    
    if (FAILED(hr) || !d2dFactory) {
        Logger::error("Failed to create D2D1 factory: {}", Logger::getHRESULTError(hr));
        return;
    }
    
    // Create D2D device
    IDXGIDevice* dxgiDevice = nullptr;
    d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    
    ID2D1Device6* d2dDevice = nullptr;
    d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
    
    // Create D2D device context
    D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
    d2dDevice->CreateDeviceContext(deviceOptions, &D2D::context);
    
    // Get swapchain buffer count
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapchain->GetDesc1(&swapChainDesc);
    bufferCount = swapChainDesc.BufferCount;
    
    // Resize resource arrays
    DXGISurfaces.resize(bufferCount, nullptr);
    D3D11Resources.resize(bufferCount, nullptr);
    D2D1Bitmaps.resize(bufferCount, nullptr);
    frameContexts.resize(bufferCount);
    
    // Create persistent descriptor heaps
    if (!D3D12DescriptorHeap.get()) {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.NumDescriptors = bufferCount;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        
        d3d12Device5->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(D3D12DescriptorHeap.put()));
    }
    
    // Create consolidated descriptor heap for ImGui and images
    if (!d3d12DescriptorHeapImGuiRender.get()) {
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.NumDescriptors = TOTAL_CONSOLIDATED_DESCRIPTORS;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        srvHeapDesc.NodeMask = 0;
        
        d3d12Device5->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(d3d12DescriptorHeapImGuiRender.put()));
    }
    
    // Create command allocator
    if (!allocator.get()) {
        d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.put()));
    }
    
    // Create command list
    if (!d3d12CommandList.get()) {
        d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.get(), nullptr, IID_PPV_ARGS(d3d12CommandList.put()));
        d3d12CommandList->Close(); // Close it immediately as it starts in recording state
    }
    
    // Setup render targets and wrapped resources
    const UINT rtvDescriptorSize = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    
    const float dpi = static_cast<float>(GetDpiForSystem());
    D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        dpi, dpi
    );


    for (UINT i = 0; i < bufferCount; i++) {
        winrt::com_ptr<ID3D12Resource> backBuffer;
        swapchain->GetBuffer(i, IID_PPV_ARGS(backBuffer.put()));
        
        // Create RTV
        d3d12Device5->CreateRenderTargetView(backBuffer.get(), nullptr, rtvHandle);
        frameContexts[i].main_render_target_descriptor = rtvHandle;
        frameContexts[i].main_render_target_resource = backBuffer;
        frameContexts[i].commandAllocator = allocator; // Share allocator
        rtvHandle.ptr += rtvDescriptorSize;
        
        // Create wrapped D3D11 resource
        D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE };
        d3d11On12Device->CreateWrappedResource(
            backBuffer.get(), &d3d11Flags,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            IID_PPV_ARGS(D3D11Resources[i].put())
        );
        
        // Get DXGI surface
        D3D11Resources[i]->QueryInterface(IID_PPV_ARGS(DXGISurfaces[i].put()));
        
        // Create D2D bitmap
        D2D::context->CreateBitmapFromDxgiSurface(DXGISurfaces[i].get(), bitmapProps, D2D1Bitmaps[i].put());
        
        // Note: We don't release backBuffer here because frameContexts[i].main_render_target_resource holds a reference
    }
    
    // Initialize ImGui for DX12
    if (!initImgui && !imguiCleanupInProgress) {
        // Wait for any ongoing cleanup to complete
        while (imguiCleanupInProgress) {
            Sleep(1);
        }
        
        if (!ImGui::GetCurrentContext()) {
            ImGui::CreateContext();
        }
        
        // Check if Win32 backend is already initialized
        auto& io = ImGui::GetIO();
        if (!io.BackendPlatformUserData) {
            ImGui_ImplWin32_Init(window2);
        }
        
        // Modern ImGui D3D12 initialization
        ImGui_ImplDX12_InitInfo initInfo = {};
        initInfo.Device = d3d12Device5.get();
        initInfo.CommandQueue = queue.get();
        initInfo.NumFramesInFlight = bufferCount;
        initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
        initInfo.SrvDescriptorHeap = d3d12DescriptorHeapImGuiRender.get();
        
        // Set up descriptor allocation callbacks
        initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu) {
            std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
            
            UINT handle_increment = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            
            D3D12_CPU_DESCRIPTOR_HANDLE cpu = d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
            D3D12_GPU_DESCRIPTOR_HANDLE gpu = d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
            
            *out_cpu = cpu;
            *out_gpu = gpu;
        };
        
        initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) {
            // No-op for persistent font descriptor
        };
        
        // Check if DX12 backend is already initialized
        if (!io.BackendRendererUserData) {
            if (!ImGui_ImplDX12_Init(&initInfo)) {
                Logger::error("Failed to initialize ImGui DX12 backend");
                return;
            }
        }
        initImgui = true;
    }
    
    // Cleanup temporary resources
    Memory::SafeRelease(d2dDevice);
    Memory::SafeRelease(dxgiDevice);
    Memory::SafeRelease(d2dFactory);
    
    // Initialize blur and motion blur
    Blur::InitializePipeline();
    
    // Reset frame counter for clearing
    dx12FrameCount = 0;
    
    init = true;
}

void SwapchainHook::DX11Render(bool underui) {
    if (!D2D::context || !context.get()) return;

    // Handle blur if needed
    DX11Blur();
    
    // Save backbuffer for effects
    SaveBackbuffer(underui);

    // Use static cached RTV from header
    static UINT lastBufferWidth = 0, lastBufferHeight = 0;
    
    // Get current backbuffer dimensions
    winrt::com_ptr<ID3D11Texture2D> backBuffer;
    if (FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), backBuffer.put_void()))) {
        return;
    }
    
    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    
    // Only recreate RTV if dimensions changed
    if (!cachedDX11RTV.get() || desc.Width != lastBufferWidth || desc.Height != lastBufferHeight) {
        cachedDX11RTV = nullptr;
        if (FAILED(d3d11Device->CreateRenderTargetView(backBuffer.get(), nullptr, cachedDX11RTV.put()))) {
            return;
        }
        lastBufferWidth = desc.Width;
        lastBufferHeight = desc.Height;
    }
    
    // backBuffer is now a smart pointer, no manual release needed

    // Save current render state
    ID3D11RenderTargetView* originalRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    ID3D11DepthStencilView* originalDSV = nullptr;
    context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, originalRTVs, &originalDSV);

    // Begin D2D drawing
    D2D::context->BeginDraw();

    // ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    ImGui::Begin("t", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | 
        ImGuiWindowFlags_NoDecoration);

    // Trigger render events
    if (!underui) {
        auto event = nes::make_holder<RenderEvent>();
        event->RTV = cachedDX11RTV.get();
        eventMgr.trigger(event);
    } else {
        auto event = nes::make_holder<RenderUnderUIEvent>();
        event->RTV = cachedDX11RTV.get();
        eventMgr.trigger(event);
    }

    // First-time notifications
    static bool notificationsShown = false;
    if (!notificationsShown && SwapchainHook::init) {
        if (auto clickGUI = ModuleManager::getModule("ClickGUI")) {
            FlarialGUI::Notify("Click " + 
                clickGUI->settings.getSettingByName<std::string>("keybind")->value + 
                " to open the menu in-game.");
            FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
            notificationsShown = true;
        }
    }

    // End D2D drawing
    D2D::context->EndDraw();

    // End ImGui frame
    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();

    // Render ImGui draw data
    ID3D11RenderTargetView* rtvPtr = cachedDX11RTV.get();
    context->OMSetRenderTargets(1, &rtvPtr, originalDSV);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Restore original render targets
    bool allValid = true;
    for (auto & originalRTV : originalRTVs) {
        if (originalRTV == nullptr) {
            allValid = false;
        }
    }

    if (allValid)
    context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, originalRTVs, originalDSV);

    // Release references
    Memory::SafeRelease(originalDSV);
    for (auto & originalRTV : originalRTVs) {
        Memory::SafeRelease(originalRTV);
    }
}


void SwapchainHook::DX12Render(bool underui) {
    if (!D2D::context || !d3d11On12Device.get() || !queue.get()) return;
    
    currentBitmap = swapchain->GetCurrentBackBufferIndex();
    
    // Acquire wrapped resources
    ID3D11Resource* resource = D3D11Resources[currentBitmap].get();
    d3d11On12Device->AcquireWrappedResources(&resource, 1);
    
    // Set D2D render target
    D2D::context->SetTarget(D2D1Bitmaps[currentBitmap].get());
    MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);
    
    // Save backbuffer and handle blur
    SaveBackbuffer();
    DX12Blur();
    
    // Begin D2D drawing
    D2D::context->BeginDraw();
    
    // Begin ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    ImGui::Begin("t", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | 
        ImGuiWindowFlags_NoDecoration);
    
    // Create RTV for events (cache per buffer)
    if (cachedDX12RTVs.size() != bufferCount) {

        for (auto rtv : cachedDX12RTVs) {
            rtv = nullptr;
        }
        cachedDX12RTVs.resize(bufferCount, nullptr);
    }
    
    if (!cachedDX12RTVs[currentBitmap].get()) {
        winrt::com_ptr<ID3D11Texture2D> buffer2D;
        D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(buffer2D.put()));
        d3d11Device->CreateRenderTargetView(buffer2D.get(), nullptr, cachedDX12RTVs[currentBitmap].put());
    }
    
    // Trigger render events
    if (!underui) {
        auto event = nes::make_holder<RenderEvent>();
        event->RTV = cachedDX12RTVs[currentBitmap].get();
        eventMgr.trigger(event);
    } else {
        auto event = nes::make_holder<RenderUnderUIEvent>();
        event->RTV = cachedDX12RTVs[currentBitmap].get();
        eventMgr.trigger(event);
    }
    
    // First-time notifications
    static bool notificationsShown = false;
    if (!notificationsShown && SwapchainHook::init) {
        if (auto clickGUI = ModuleManager::getModule("ClickGUI")) {
            FlarialGUI::Notify("Click " + 
                clickGUI->settings.getSettingByName<std::string>("keybind")->value + 
                " to open the menu in-game.");
            FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
            notificationsShown = true;
        }
    }
    
    // End D2D drawing
    D2D::context->EndDraw();
    D2D::context->SetTarget(nullptr);
    
    // Release wrapped resources
    d3d11On12Device->ReleaseWrappedResources(&resource, 1);
    context->Flush();
    
    // Reset command allocator
    frameContexts[currentBitmap].commandAllocator->Reset();
    
    // Reset command list
    d3d12CommandList->Reset(frameContexts[currentBitmap].commandAllocator.get(), nullptr);
    
    // Transition resource to render target
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = frameContexts[currentBitmap].main_render_target_resource.get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    
    d3d12CommandList->ResourceBarrier(1, &barrier);
    
    // Set render target
    d3d12CommandList->OMSetRenderTargets(1, 
        &frameContexts[currentBitmap].main_render_target_descriptor,
        FALSE, nullptr);
    
    // Clear render target on first few frames to avoid garbage data
    if (dx12FrameCount < 5) {
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        d3d12CommandList->ClearRenderTargetView(
            frameContexts[currentBitmap].main_render_target_descriptor,
            clearColor, 0, nullptr);
        dx12FrameCount++;
    }
    
    // Set descriptor heap
    ID3D12DescriptorHeap* heapPtr = d3d12DescriptorHeapImGuiRender.get();
    d3d12CommandList->SetDescriptorHeaps(1, &heapPtr);
    
    // End ImGui frame and render
    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList.get());
    
    // Transition back to present
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    d3d12CommandList->ResourceBarrier(1, &barrier);
    
    // Close and execute command list
    d3d12CommandList->Close();
    ID3D12CommandList* cmdListPtr = d3d12CommandList.get();
    queue->ExecuteCommandLists(1, &cmdListPtr);
    
    // Use cached fence for synchronization
    if (!cachedDX12Fence.get()) {
        d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(cachedDX12Fence.put()));
    }
    
    const UINT64 currentFenceValue = ++cachedDX12FenceValue;
    queue->Signal(cachedDX12Fence.get(), currentFenceValue);
    
    // Wait if necessary
    if (cachedDX12Fence->GetCompletedValue() < currentFenceValue) {
        cachedDX12Fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }
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


winrt::com_ptr<ID3D11Texture2D> SwapchainHook::GetBackbuffer() {
    return SavedD3D11BackBuffer;
}

void SwapchainHook::SaveBackbuffer(bool underui) {

    SavedD3D11BackBuffer = nullptr;
    ExtraSavedD3D11BackBuffer = nullptr;
    if (!SwapchainHook::queue.get()) {

        SwapchainHook::swapchain->GetBuffer(0, IID_PPV_ARGS(SavedD3D11BackBuffer.put()));

        if (FlarialGUI::needsBackBuffer) {

            if (!ExtraSavedD3D11BackBuffer.get()) {
                D3D11_TEXTURE2D_DESC textureDesc = {};
                SavedD3D11BackBuffer->GetDesc(&textureDesc);
                textureDesc.Usage = D3D11_USAGE_DEFAULT;
                textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                textureDesc.CPUAccessFlags = 0;

                SwapchainHook::d3d11Device->CreateTexture2D(&textureDesc, nullptr, ExtraSavedD3D11BackBuffer.put());
            }

            if (underui) {

                if (UnderUIHooks::bgfxCtx->m_msaart) {
                    context->ResolveSubresource(ExtraSavedD3D11BackBuffer.get(), 0, UnderUIHooks::bgfxCtx->m_msaart, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                } else {
                    context->CopyResource(ExtraSavedD3D11BackBuffer.get(), SavedD3D11BackBuffer.get());
                }

            } else {
                context->CopyResource(ExtraSavedD3D11BackBuffer.get(), SavedD3D11BackBuffer.get());
            }
        }


    }
    else {
        HRESULT hr;

        hr = D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(SavedD3D11BackBuffer.put()));
        if (FAILED(hr)) std::cout << "Failed to query interface: " << std::hex << hr << std::endl;
    }
}

// Consolidated descriptor heap management functions
bool SwapchainHook::AllocateImageDescriptor(UINT imageId, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
    std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
    
    if (!d3d12DescriptorHeapImGuiRender.get() || !d3d12Device5.get()) {
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