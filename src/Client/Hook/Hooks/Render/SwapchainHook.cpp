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
using ::IUnknown;

#include "../../../Module/Modules/MotionBlur/MotionBlur.hpp"
#include "../../../../Assets/Assets.hpp"

// Initialize static members
std::atomic<bool> SwapchainHook::init{false};
std::atomic<bool> SwapchainHook::queueReset{true};
std::atomic<bool> SwapchainHook::currentVsyncState{false};
std::atomic<bool> SwapchainHook::hasResized{false};
std::atomic<int> SwapchainHook::currentBitmap{0};
std::atomic<UINT> SwapchainHook::flagsreal{0};
std::atomic<uintptr_t> SwapchainHook::bufferCount{0};
std::atomic<uint64_t> SwapchainHook::buffersCounts{0};
std::atomic<UINT> SwapchainHook::nextAvailableDescriptorIndex{SwapchainHook::IMGUI_FONT_DESCRIPTORS};
std::atomic<UINT> SwapchainHook::lastBackbufferWidth{0};
std::atomic<UINT> SwapchainHook::lastBackbufferHeight{0};

// Static resource pointers
std::shared_ptr<ID3D11Device> SwapchainHook::d3d11Device;
std::shared_ptr<ID3D11DeviceContext> SwapchainHook::context;
std::shared_ptr<ID3D11Texture2D> SwapchainHook::SavedD3D11BackBuffer;
std::shared_ptr<ID3D11Texture2D> SwapchainHook::ExtraSavedD3D11BackBuffer;
std::shared_ptr<ID3D11Texture2D> SwapchainHook::stageTex;
std::shared_ptr<ID2D1Bitmap1> SwapchainHook::D2D1Bitmap;

std::shared_ptr<ID3D12Device5> SwapchainHook::d3d12Device5;
std::shared_ptr<ID3D12CommandQueue> SwapchainHook::queue;
std::shared_ptr<ID3D12CommandAllocator> SwapchainHook::allocator;
std::shared_ptr<ID3D12GraphicsCommandList> SwapchainHook::d3d12CommandList;
std::shared_ptr<ID3D11On12Device> SwapchainHook::d3d11On12Device;
HANDLE SwapchainHook::fenceEvent = nullptr;

std::shared_ptr<IDXGISwapChain3> SwapchainHook::swapchain;
std::vector<std::shared_ptr<IDXGISurface1>> SwapchainHook::DXGISurfaces;
std::vector<std::shared_ptr<ID2D1Bitmap1>> SwapchainHook::D2D1Bitmaps;
std::vector<std::shared_ptr<ID3D11Resource>> SwapchainHook::D3D11Resources;

std::shared_ptr<ID3D12DescriptorHeap> SwapchainHook::D3D12DescriptorHeap;
std::shared_ptr<ID3D12DescriptorHeap> SwapchainHook::d3d12DescriptorHeapImGuiRender;
std::shared_ptr<ID3D12DescriptorHeap> SwapchainHook::d3d12DescriptorHeapBackBuffers;

std::vector<FrameContext> SwapchainHook::frameContexts;
std::mutex SwapchainHook::frameTransformsMtx;
std::queue<FrameTransform> SwapchainHook::FrameTransforms;
int SwapchainHook::transformDelay = 3;
std::mutex SwapchainHook::descriptorAllocationMutex;

// Global variables
bool initImgui = false;
bool allfontloaded = false;
bool first = false;
bool imguiWindowInit = false;
BOOL _ = FALSE, $ = FALSE;

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
static std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

HWND window2 = FindWindowA(nullptr, "Minecraft");

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", 0) {}

SwapchainHook::~SwapchainHook() {
    Cleanup();
}

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
    HANDLE hProcess = GetCurrentProcess();
    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                std::wstring baseModuleName = std::filesystem::path(szModName).filename().wstring();
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
            return FALSE;
        }
        return TRUE;
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

    // CREATE SWAPCHAIN FOR CORE WINDOW HOOK
    IDXGIFactory2 *pFactory = NULL;
    CreateDXGIFactory(IID_PPV_ARGS(&pFactory));
    if (!pFactory) LOG_ERROR("Factory not created");

    CreateSwapchainForCoreWindowHook::hook(pFactory);

    // GPU detection for Intel compatibility
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

    // Overlay compatibility check
    bool isRTSS = containsModule(L"RTSSHooks64.dll");
    if (isRTSS) {
        Logger::debug("[Swapchain] MSI Afterburner failed to unload!");
        Utils::MessageDialogW(L"Flarial: client failed to initialize, disable MSI Afterburner or RTSS!", L"Error!");
        ModuleManager::terminate();
        Client::disable = true;
    }
}

HRESULT SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {
    if (Client::disable) return funcOriginal(pSwapChain, syncInterval, flags);

    if (currentVsyncState != Client::settings.getSettingByName<bool>("vsync")->value) {
        queueReset = true;
    }

    if (queueReset) {
        init = false;
        initImgui = false;
        Logger::debug("Resetting SwapChain");
        Cleanup();
        return DXGI_ERROR_DEVICE_RESET;
    }

    swapchain = MakeShared(pSwapChain);
    pSwapChain->AddRef(); // Ensure reference count is correct
    flagsreal = flags;

    UnderUIHooks::index = 0;

    if (D2D::context) MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);

    // Initialize UnderUI hooks
    static bool hooker = false;
    if (!hooker && ((queue && d3d12CommandList) || (!queue && context))) {
        UnderUIHooks hook;
        hook.enableHook();
        hooker = true;
    }

    FPSMeasure();

    if (!init) {
        if (!queue) {
            DX11Init();
        } else {
            DX12Init();
        }
    } else {
        Fonts();
        RenderSync();

        if (D2D::context != nullptr && !Client::disable) {
            if (queue != nullptr) {
                DX12Render();
            } else {
                DX11Render();
            }
        }
    }

    try {
        if (init && initImgui && !FlarialGUI::hasLoadedAll) { 
            FlarialGUI::LoadAllImages(); 
            FlarialGUI::hasLoadedAll = true; 
        }
    } catch (const std::exception &ex) { 
        LOG_ERROR("Fail at loading all images: ", ex.what()); 
    }

    if (currentVsyncState) {
        return funcOriginal(pSwapChain, 0, DXGI_PRESENT_ALLOW_TEARING);
    }

    return funcOriginal(pSwapChain, syncInterval, flags);
}

void SwapchainHook::DX11Init() {
    Logger::debug("Initializing for DX11");

    ID3D11Device* rawDevice = nullptr;
    if (FAILED(swapchain->GetDevice(IID_PPV_ARGS(&rawDevice))) || !rawDevice) {
        Logger::debug("Failed to get DX11 device");
        return;
    }
    d3d11Device = MakeShared(rawDevice);

    IDXGISurface1* backBuffer = nullptr;
    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || !backBuffer) {
        Logger::debug("Failed to get back buffer");
        return;
    }

    D2D1_CREATION_PROPERTIES properties = {};
    properties.threadingMode = D2D1_THREADING_MODE_MULTI_THREADED;
    properties.debugLevel = D2D1_DEBUG_LEVEL_NONE;
    properties.options = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;

    if (FAILED(D2D1CreateDeviceContext(backBuffer, properties, &D2D::context))) {
        Logger::debug("Failed to create D2D device context");
        Memory::SafeRelease(backBuffer);
        return;
    }

    const D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
    
    ID2D1Bitmap1* rawBitmap = nullptr;
    if (SUCCEEDED(D2D::context->CreateBitmapFromDxgiSurface(backBuffer, bitmapProps, &rawBitmap))) {
        D2D1Bitmap = MakeShared(rawBitmap);
    }

    if (!initImgui) {
        ImGui::CreateContext();
        
        ID3D11DeviceContext* rawContext = nullptr;
        d3d11Device->GetImmediateContext(&rawContext);
        if (rawContext) {
            context = MakeShared(rawContext);
            
            ImGui_ImplWin32_Init(window2);
            ImGui_ImplDX11_Init(d3d11Device.get(), context.get());
            initImgui = true;
        }
    }

    SaveBackbuffer();
    Blur::InitializePipeline();
    MotionBlur::initted = AvgPixelMotionBlurHelper::Initialize();
    MotionBlur::initted = RealMotionBlurHelper::Initialize();

    Memory::SafeRelease(backBuffer);
    init = true;
}

void SwapchainHook::DX12Init() {
    Logger::debug("Initializing for DX12");
    
    ID3D12Device5* rawDevice = nullptr;
    if (FAILED(swapchain->GetDevice(IID_PPV_ARGS(&rawDevice))) || !rawDevice) {
        Logger::debug("Failed to get DX12 device");
        return;
    }
    d3d12Device5 = MakeShared(rawDevice);

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) {
        Logger::debug("Failed to create fence event");
        return;
    }

    ID3D11Device* raw11Device = nullptr;
    ID3D11DeviceContext* raw11Context = nullptr;
    IUnknown* rawQueues[] = { queue.get() };
    
    if (FAILED(D3D11On12CreateDevice(rawDevice, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                    rawQueues, 1, 0, &raw11Device, &raw11Context, nullptr)) || !raw11Device) {
        Logger::debug("Failed to create D3D11On12 device");
        return;
    }
    
    d3d11Device = MakeShared(raw11Device);
    context = MakeShared(raw11Context);

    ID3D11On12Device* rawOn12Device = nullptr;
    if (SUCCEEDED(d3d11Device->QueryInterface(IID_PPV_ARGS(&rawOn12Device)))) {
        d3d11On12Device = MakeShared(rawOn12Device);
    }

    ID2D1Factory7* d2dFactory = nullptr;
    D2D1_FACTORY_OPTIONS factoryOptions = {};
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory7), 
                                &factoryOptions, (void**)&d2dFactory))) {
        Logger::debug("Failed to create D2D1 factory");
        return;
    }

    IDXGIDevice* dxgiDevice = nullptr;
    ID2D1Device6* d2dDevice = nullptr;
    if (SUCCEEDED(d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)) &&
        SUCCEEDED(d2dFactory->CreateDevice(dxgiDevice, &d2dDevice))) {
        d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &D2D::context);
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapchain->GetDesc1(&swapChainDesc);
    bufferCount = swapChainDesc.BufferCount;

    DXGISurfaces.resize(bufferCount);
    D3D11Resources.resize(bufferCount);
    D2D1Bitmaps.resize(bufferCount);

    const float dpi = static_cast<float>(GetDpiForSystem());
    const D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = static_cast<UINT>(bufferCount);
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 1;

    ID3D12DescriptorHeap* rawHeap = nullptr;
    if (SUCCEEDED(rawDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rawHeap)))) {
        D3D12DescriptorHeap = MakeShared(rawHeap);
    }

    const UINT rtvDescriptorSize = rawDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < bufferCount; i++) {
        ID3D12Resource* backBuffer = nullptr;
        if (FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)))) continue;
        
        rawDevice->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);
        rtvHandle.ptr += rtvDescriptorSize;

        D3D11_RESOURCE_FLAGS d3d11Flags = {};
        d3d11Flags.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        ID3D11Resource* wrapped11Resource = nullptr;
        if (SUCCEEDED(d3d11On12Device->CreateWrappedResource(backBuffer, &d3d11Flags,
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                            D3D12_RESOURCE_STATE_PRESENT, 
                                                            IID_PPV_ARGS(&wrapped11Resource)))) {
            D3D11Resources[i] = MakeShared(wrapped11Resource);
            
            IDXGISurface1* surface = nullptr;
            if (SUCCEEDED(D3D11Resources[i]->QueryInterface(&surface))) {
                DXGISurfaces[i] = MakeShared(surface);
                
                ID2D1Bitmap1* bitmap = nullptr;
                if (SUCCEEDED(D2D::context->CreateBitmapFromDxgiSurface(surface, bitmapProps, &bitmap))) {
                    D2D1Bitmaps[i] = MakeShared(bitmap);
                }
            }
        }
        
        Memory::SafeRelease(backBuffer);
    }

    Memory::SafeRelease(d2dDevice);
    Memory::SafeRelease(dxgiDevice);
    Memory::SafeRelease(d2dFactory);

    Blur::InitializePipeline();
    init = true;
}

void SwapchainHook::DX11Render(bool underui) {
    if (!D2D::context || !context || !d3d11Device) return;
    
    DX11Blur();
    SaveBackbuffer(underui);
    D2D::context->BeginDraw();

    ID3D11RenderTargetView* originalRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    ID3D11DepthStencilView* originalDSV = nullptr;
    UINT numRTVs = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
    context->OMGetRenderTargets(numRTVs, originalRTVs, &originalDSV);

    ID3D11Texture2D* backBuffer = nullptr;
    ID3D11RenderTargetView* mainRTV = nullptr;
    
    if (SUCCEEDED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) && backBuffer &&
        SUCCEEDED(d3d11Device->CreateRenderTargetView(backBuffer, nullptr, &mainRTV)) && mainRTV) {
        
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("t", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);

        if (!underui) {
            auto event = nes::make_holder<RenderEvent>();
            event->RTV = mainRTV;
            eventMgr.trigger(event);
        } else {
            auto event = nes::make_holder<RenderUnderUIEvent>();
            event->RTV = mainRTV;
            eventMgr.trigger(event);
        }

        if (!first && SwapchainHook::init && ModuleManager::getModule("ClickGUI")) {
            FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("keybind")->value + " to open the menu in-game.");
            FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
            first = true;
        }

        D2D::context->EndDraw();
        ImGui::End();
        ImGui::EndFrame();
        ImGui::Render();

        context->OMSetRenderTargets(1, &mainRTV, originalDSV);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    context->OMSetRenderTargets(numRTVs, originalRTVs, originalDSV);

    Memory::SafeRelease(backBuffer);
    Memory::SafeRelease(mainRTV);
    Memory::SafeRelease(originalDSV);
    for (UINT i = 0; i < numRTVs; ++i) {
        Memory::SafeRelease(originalRTVs[i]);
    }
}

void SwapchainHook::DX12Render(bool underui) {
    if (!D2D::context || !d3d12Device5 || !queue || !d3d11On12Device) return;
    
    currentBitmap = static_cast<int>(swapchain->GetCurrentBackBufferIndex());
    if (currentBitmap >= D3D11Resources.size() || !D3D11Resources[currentBitmap]) return;

    ID3D12Fence* fence = nullptr;
    UINT64 fenceValue = 1;
    if (FAILED(d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))) {
        return;
    }

    ID3D11Resource* resource = D3D11Resources[currentBitmap].get();
    d3d11On12Device->AcquireWrappedResources(&resource, 1);
    
    SaveBackbuffer();
    D2D::context->SetTarget(D2D1Bitmaps[currentBitmap].get());
    MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);

    DX12Blur();
    D2D::context->BeginDraw();

    if (!d3d12DescriptorHeapImGuiRender) {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.NumDescriptors = TOTAL_CONSOLIDATED_DESCRIPTORS;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        
        ID3D12DescriptorHeap* rawHeap = nullptr;
        if (SUCCEEDED(d3d12Device5->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rawHeap)))) {
            d3d12DescriptorHeapImGuiRender = MakeShared(rawHeap);
        }
    }

    if (!allocator) {
        ID3D12CommandAllocator* rawAllocator = nullptr;
        if (SUCCEEDED(d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&rawAllocator)))) {
            allocator = MakeShared(rawAllocator);
        }
    }

    if (!d3d12CommandList && allocator) {
        ID3D12GraphicsCommandList* rawCommandList = nullptr;
        if (SUCCEEDED(d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.get(), nullptr, IID_PPV_ARGS(&rawCommandList)))) {
            d3d12CommandList = MakeShared(rawCommandList);
        }
    }

    if (!initImgui && d3d12DescriptorHeapImGuiRender) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window2);

        ImGui_ImplDX12_InitInfo initInfo = {};
        initInfo.Device = d3d12Device5.get();
        initInfo.CommandQueue = queue.get();
        initInfo.NumFramesInFlight = static_cast<int>(bufferCount);
        initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
        initInfo.SrvDescriptorHeap = d3d12DescriptorHeapImGuiRender.get();
        initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* cpu, D3D12_GPU_DESCRIPTOR_HANDLE* gpu) {
            const UINT increment = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            *cpu = d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
            *gpu = d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
        };
        initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE) {};

        initImgui = ImGui_ImplDX12_Init(&initInfo);
    }

    if (!initImgui || !d3d12CommandList || !allocator) {
        d3d11On12Device->ReleaseWrappedResources(&resource, 1);
        Memory::SafeRelease(fence);
        return;
    }

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("t", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);

    ID3D11Texture2D* buffer2D = nullptr;
    ID3D11RenderTargetView* mainRTV = nullptr;
    
    if (SUCCEEDED(D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(&buffer2D))) &&
        SUCCEEDED(d3d11Device->CreateRenderTargetView(buffer2D, nullptr, &mainRTV))) {
        
        if (!underui) {
            auto event = nes::make_holder<RenderEvent>();
            event->RTV = mainRTV;
            eventMgr.trigger(event);
        } else {
            auto event = nes::make_holder<RenderUnderUIEvent>();
            event->RTV = mainRTV;
            eventMgr.trigger(event);
        }

        if (!first && SwapchainHook::init && ModuleManager::getModule("ClickGUI")) {
            FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>("keybind")->value + " to open the menu in-game.");
            FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
            first = true;
        }
    }

    D2D::context->EndDraw();
    D2D::context->SetTarget(nullptr);
    d3d11On12Device->ReleaseWrappedResources(&resource, 1);
    context->Flush();

    if (allocator) allocator->Reset();
    if (d3d12CommandList) {
        d3d12CommandList->Reset(allocator.get(), nullptr);
        
        ID3D12Resource* backBuffer = nullptr;
        if (SUCCEEDED(swapchain->GetBuffer(currentBitmap, IID_PPV_ARGS(&backBuffer)))) {
            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = backBuffer;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            
            d3d12CommandList->ResourceBarrier(1, &barrier);
            
            if (D3D12DescriptorHeap) {
                const UINT rtvSize = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                rtvHandle.ptr += currentBitmap * rtvSize;
                
                d3d12CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
            }
            
            if (d3d12DescriptorHeapImGuiRender) {
                ID3D12DescriptorHeap* heaps[] = { d3d12DescriptorHeapImGuiRender.get() };
                d3d12CommandList->SetDescriptorHeaps(1, heaps);
            }
            
            ImGui::End();
            ImGui::EndFrame();
            ImGui::Render();
            
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList.get());
            
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            d3d12CommandList->ResourceBarrier(1, &barrier);
            d3d12CommandList->Close();
            
            ID3D12CommandList* commandLists[] = { d3d12CommandList.get() };
            queue->ExecuteCommandLists(1, commandLists);
            
            queue->Signal(fence, fenceValue);
            if (fence->GetCompletedValue() < fenceValue) {
                fence->SetEventOnCompletion(fenceValue, fenceEvent);
                WaitForSingleObject(fenceEvent, INFINITE);
            }
            
            Memory::SafeRelease(backBuffer);
        }
    }

    Memory::SafeRelease(mainRTV);
    Memory::SafeRelease(buffer2D);
    Memory::SafeRelease(fence);
}

void SwapchainHook::DX11Blur() {
    prepareBlur();
    if (ModuleManager::initialized) {
        auto module = ModuleManager::getModule("Motion Blur");
        if (module) {
            if (module->isEnabled() || FlarialGUI::inMenu) FlarialGUI::needsBackBuffer = true;
            else FlarialGUI::needsBackBuffer = false;
        }
    }
}

void SwapchainHook::DX12Blur() {
    prepareBlur();
    if (FlarialGUI::inMenu) FlarialGUI::needsBackBuffer = true;
    else FlarialGUI::needsBackBuffer = false;
}

void SwapchainHook::RenderSync() {
    if (init && initImgui) {
        std::lock_guard<std::mutex> lock(frameTransformsMtx);
        while (FrameTransforms.size() > transformDelay) {
            MC::Transform = FrameTransforms.front();
            FrameTransforms.pop();
        }
    }
}

void SwapchainHook::prepareBlur() {
    auto blurIntensity = Client::settings.getSettingByName<float>("blurintensity")->value;

    if ((ModuleManager::doesAnyModuleHave("BlurEffect") && blurIntensity > 1) && !FlarialGUI::inMenu) {
        FlarialGUI::PrepareBlur(blurIntensity);
        D2D1_IMAGE_BRUSH_PROPERTIES props = D2D1::ImageBrushProperties(
            D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y));
        D2D::context->CreateImageBrush(FlarialGUI::blur_bitmap_cache, props, &FlarialGUI::blurbrush);
    }
}

void SwapchainHook::Fonts() {
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

        if (FlarialGUI::HasAFontLoaded) {
            fontRebuildQueued = true;
            FlarialGUI::HasAFontLoaded = false;
        }

        if (fontRebuildQueued) {
            io.Fonts->Build();
            
            static int frameDelay = 0;
            if (++frameDelay >= 3) {
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

std::shared_ptr<ID3D11Texture2D> SwapchainHook::GetBackbuffer() {
    return SavedD3D11BackBuffer;
}

void SwapchainHook::SaveBackbuffer(bool underui) {
    SafeReset(SavedD3D11BackBuffer);
    SafeReset(ExtraSavedD3D11BackBuffer);
    
    if (!queue) {
        ID3D11Texture2D* rawBackBuffer = nullptr;
        swapchain->GetBuffer(0, IID_PPV_ARGS(&rawBackBuffer));
        SavedD3D11BackBuffer = MakeShared(rawBackBuffer);

        if (FlarialGUI::needsBackBuffer) {
            if (!ExtraSavedD3D11BackBuffer) {
                D3D11_TEXTURE2D_DESC textureDesc = {};
                SavedD3D11BackBuffer->GetDesc(&textureDesc);
                textureDesc.Usage = D3D11_USAGE_DEFAULT;
                textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                textureDesc.CPUAccessFlags = 0;

                ID3D11Texture2D* rawExtraBuffer = nullptr;
                d3d11Device->CreateTexture2D(&textureDesc, nullptr, &rawExtraBuffer);
                ExtraSavedD3D11BackBuffer = MakeShared(rawExtraBuffer);
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
    } else {
        if (currentBitmap < D3D11Resources.size() && D3D11Resources[currentBitmap]) {
            ID3D11Texture2D* rawSavedBuffer = nullptr;
            HRESULT hr = D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(&rawSavedBuffer));
            if (SUCCEEDED(hr)) {
                SavedD3D11BackBuffer = MakeShared(rawSavedBuffer);
            } else {
                std::cout << "Failed to query interface: " << std::hex << hr << std::endl;
            }
        }
    }
}

bool SwapchainHook::AllocateImageDescriptor(UINT imageId, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
    std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
    
    if (!d3d12DescriptorHeapImGuiRender || !d3d12Device5) {
        Logger::custom(fg(fmt::color::red), "DescriptorAlloc", "ERROR: Consolidated descriptor heap or device not available");
        return false;
    }
    
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
    
    if (descriptorIndex >= nextAvailableDescriptorIndex) {
        nextAvailableDescriptorIndex = descriptorIndex + 1;
    }

    return true;
}

void SwapchainHook::FreeImageDescriptor(UINT imageId) {
    std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
    Logger::custom(fg(fmt::color::cyan), "DescriptorFree", "Freed descriptor for image ID {} (no-op for persistent images)", imageId);
}

void SwapchainHook::ResetDescriptorAllocation() {
    std::lock_guard<std::mutex> lock(descriptorAllocationMutex);
    nextAvailableDescriptorIndex = IMGUI_FONT_DESCRIPTORS;
    Logger::custom(fg(fmt::color::yellow), "DescriptorAlloc", "Reset descriptor allocation, next available index: {}", nextAvailableDescriptorIndex.load());
}

void SwapchainHook::Cleanup() {
    Logger::debug("Cleaning up SwapchainHook resources");
    
    // Properly shutdown ImGui before cleaning up resources
    if (ImGui::GetCurrentContext()) {
        Logger::debug("Shutting down ImGui");
        
        // Clear font atlas and FlarialGUI font cache
        ImGui::GetIO().Fonts->Clear();
        FlarialGUI::FontMap.clear();
        
        // Shutdown platform/renderer backends
        ImGui_ImplWin32_Shutdown();
        
        if (queue.get()) {
            // DX12 path
            ImGui_ImplDX12_Shutdown();
        } else {
            // DX11 path  
            ImGui_ImplDX11_Shutdown();
        }
        
        // Destroy ImGui context
        ImGui::DestroyContext();
        Logger::debug("ImGui shutdown complete");
    }
    
    // Reset all shared_ptr resources
    SafeReset(d3d11Device);
    SafeReset(context);
    SafeReset(SavedD3D11BackBuffer);
    SafeReset(ExtraSavedD3D11BackBuffer);
    SafeReset(stageTex);
    SafeReset(D2D1Bitmap);
    
    SafeReset(d3d12Device5);
    SafeReset(allocator);
    SafeReset(d3d12CommandList);
    SafeReset(d3d11On12Device);
    
    SafeReset(swapchain);
    DXGISurfaces.clear();
    D2D1Bitmaps.clear();
    D3D11Resources.clear();
    
    SafeReset(D3D12DescriptorHeap);
    SafeReset(d3d12DescriptorHeapImGuiRender);
    SafeReset(d3d12DescriptorHeapBackBuffers);
    
    frameContexts.clear();
    
    if (fenceEvent) {
        CloseHandle(fenceEvent);
        fenceEvent = nullptr;
    }
    
    // Clear frame transforms
    std::lock_guard<std::mutex> lock(frameTransformsMtx);
    while (!FrameTransforms.empty()) {
        FrameTransforms.pop();
    }
    
    // Clean up UI images and reset image loading state
    Logger::debug("Cleaning up UI images");
    
    // Clean up static image upload resources (important for D3D12)
    try {
        FlarialGUI::CleanupImageResources();
    } catch (const std::exception &ex) {
        Logger::debug("Exception during image resource cleanup: {}", ex.what());
    }
    
    // Clear all image caches - they'll be reloaded after swapchain recreation
    FlarialGUI::cachedBitmaps.clear();
    
    // Clear ImGui texture containers
    ImagesClass::ImguiDX11Images.clear();
    ImagesClass::ImguiDX12Images.clear();
    ImagesClass::ImguiDX12Textures.clear();
    ImagesClass::eimages.clear();
    ImagesClass::images.clear();
    
    // Reset image loading state so images get reloaded after recreation
    FlarialGUI::hasLoadedAll = false;
    Logger::debug("Image loading state reset - images will reload after swapchain recreation");
    
    // Reset state flags
    initImgui = false;
    first = false; // Reset first-time notification flag
    
    ResetDescriptorAllocation();
}