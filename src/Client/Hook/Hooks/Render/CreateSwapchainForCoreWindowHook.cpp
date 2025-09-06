#include "CreateSwapchainForCoreWindowHook.hpp"
#include <winrt/base.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <windows.h>

using ::IUnknown;

#if defined(__DEBUG__)
// ==============================
// Forward declarations / typedefs
// ==============================
typedef HRESULT (WINAPI *PFN_D3D11CreateDevice)(
    IDXGIAdapter*,
    D3D_DRIVER_TYPE,
    HMODULE,
    UINT,
    const D3D_FEATURE_LEVEL*,
    UINT,
    UINT,
    ID3D11Device**,
    D3D_FEATURE_LEVEL*,
    ID3D11DeviceContext**);

typedef HRESULT (WINAPI *PFN_D3D11CreateDeviceAndSwapChain)(
    IDXGIAdapter*,
    D3D_DRIVER_TYPE,
    HMODULE,
    UINT,
    const D3D_FEATURE_LEVEL*,
    UINT,
    UINT,
    const DXGI_SWAP_CHAIN_DESC*,
    IDXGISwapChain**,
    ID3D11Device**,
    D3D_FEATURE_LEVEL*,
    ID3D11DeviceContext**);

typedef HRESULT (WINAPI *PFN_D3D12CreateDevice)(
    IUnknown*,
    D3D_FEATURE_LEVEL,
    REFIID,
    void**);

// ==============================
// Original function pointers
// ==============================
static PFN_D3D11CreateDevice g_OrigD3D11CreateDevice = nullptr;
static PFN_D3D11CreateDeviceAndSwapChain g_OrigD3D11CreateDeviceAndSwapChain = nullptr;
static PFN_D3D12CreateDevice g_OrigD3D12CreateDevice = nullptr;

// ==============================
// Helpers
// ==============================
static void EnableD3D12DebugLayerIfAvailable() {
    winrt::com_ptr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.put())))) {
        debugController->EnableDebugLayer();
        Logger::success("D3D12 debug layer enabled");
    } else {
        Logger::debug("D3D12GetDebugInterface failed (Graphics Tools not installed?) — continuing without D3D12 debug layer");
    }
}

static HRESULT TryD3D11CreateDeviceWithOptionalDebug(
    PFN_D3D11CreateDevice orig,
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext)
{
    UINT flagsWithDebug = Flags | D3D11_CREATE_DEVICE_DEBUG;
    Logger::debug("Attempting D3D11CreateDevice with debug layer");
    HRESULT hr = orig(
        pAdapter, DriverType, Software, flagsWithDebug,
        pFeatureLevels, FeatureLevels, SDKVersion,
        ppDevice, pFeatureLevel, ppImmediateContext);

    // If the debug layer isn't present, D3D11CreateDevice can return E_INVALIDARG.
    if (FAILED(hr)) {
        if (hr == E_INVALIDARG) {
            Logger::debug("D3D11 debug layer unavailable; retrying without debug flag");
            hr = orig(
                pAdapter, DriverType, Software, Flags,
                pFeatureLevels, FeatureLevels, SDKVersion,
                ppDevice, pFeatureLevel, ppImmediateContext);
        } else {
            Logger::error("D3D11CreateDevice failed with debug flag: {}", Logger::getHRESULTError(hr));
        }
    } else {
        Logger::success("D3D11 debug layer enabled");
    }

    return hr;
}

static HRESULT TryD3D11CreateDeviceAndSwapChainWithOptionalDebug(
    PFN_D3D11CreateDeviceAndSwapChain orig,
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext)
{
    UINT flagsWithDebug = Flags | D3D11_CREATE_DEVICE_DEBUG;
    Logger::debug("Attempting D3D11CreateDeviceAndSwapChain with debug layer");
    HRESULT hr = orig(
        pAdapter, DriverType, Software, flagsWithDebug,
        pFeatureLevels, FeatureLevels, SDKVersion,
        pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);

    if (FAILED(hr)) {
        if (hr == E_INVALIDARG) {
            Logger::debug("D3D11 debug layer unavailable; retrying without debug flag");
            hr = orig(
                pAdapter, DriverType, Software, Flags,
                pFeatureLevels, FeatureLevels, SDKVersion,
                pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
        } else {
            Logger::error("D3D11CreateDeviceAndSwapChain failed with debug flag: {}", Logger::getHRESULTError(hr));
        }
    } else {
        Logger::success("D3D11 debug layer enabled (CreateDeviceAndSwapChain)");
    }

    return hr;
}

// ==============================
// Hooked functions
// ==============================

static HRESULT WINAPI Hook_D3D11CreateDevice(
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext)
{
    return TryD3D11CreateDeviceWithOptionalDebug(
        g_OrigD3D11CreateDevice,
        pAdapter, DriverType, Software, Flags,
        pFeatureLevels, FeatureLevels, SDKVersion,
        ppDevice, pFeatureLevel, ppImmediateContext);
}

static HRESULT WINAPI Hook_D3D11CreateDeviceAndSwapChain(
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext)
{
    return TryD3D11CreateDeviceAndSwapChainWithOptionalDebug(
        g_OrigD3D11CreateDeviceAndSwapChain,
        pAdapter, DriverType, Software, Flags,
        pFeatureLevels, FeatureLevels, SDKVersion,
        pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
}

static HRESULT WINAPI Hook_D3D12CreateDevice(
    IUnknown* pAdapter,
    D3D_FEATURE_LEVEL MinimumFeatureLevel,
    REFIID riid,
    void** ppDevice)
{
    // Enable the D3D12 debug layer BEFORE creating the device
    EnableD3D12DebugLayerIfAvailable();
    return g_OrigD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
}

// ==============================
// Install hooks
// ==============================

static void HookD3D11Exports() {
    HMODULE hD3D11 = GetModuleHandleW(L"d3d11.dll");
    if (!hD3D11) {
        hD3D11 = LoadLibraryW(L"d3d11.dll");
    }

    if (hD3D11) {
        FARPROC pCreateDevice = GetProcAddress(hD3D11, "D3D11CreateDevice");
        FARPROC pCreateDeviceAndSwapChain = GetProcAddress(hD3D11, "D3D11CreateDeviceAndSwapChain");

        if (pCreateDevice) {
            Memory::hookFunc(reinterpret_cast<LPVOID>(pCreateDevice),
                             reinterpret_cast<void*>(Hook_D3D11CreateDevice),
                             reinterpret_cast<void**>(&g_OrigD3D11CreateDevice),
                             "D3D11CreateDevice");
            Logger::success("Hooked D3D11CreateDevice");
        } else {
            Logger::debug("Failed to get proc address for D3D11CreateDevice");
        }

        if (pCreateDeviceAndSwapChain) {
            Memory::hookFunc(reinterpret_cast<LPVOID>(pCreateDeviceAndSwapChain),
                             reinterpret_cast<void*>(Hook_D3D11CreateDeviceAndSwapChain),
                             reinterpret_cast<void**>(&g_OrigD3D11CreateDeviceAndSwapChain),
                             "D3D11CreateDeviceAndSwapChain");
            Logger::success("Hooked D3D11CreateDeviceAndSwapChain");
        } else {
            Logger::debug("Failed to get proc address for D3D11CreateDeviceAndSwapChain");
        }
    } else {
        Logger::debug("d3d11.dll not loaded (yet); D3D11 hooks not installed");
    }
}

static void HookD3D12Exports() {
    HMODULE hD3D12 = GetModuleHandleW(L"d3d12.dll");
    if (!hD3D12) {
        hD3D12 = LoadLibraryW(L"d3d12.dll");
    }

    if (hD3D12) {
        FARPROC pCreateDevice = GetProcAddress(hD3D12, "D3D12CreateDevice");
        if (pCreateDevice) {
            Memory::hookFunc(reinterpret_cast<LPVOID>(pCreateDevice),
                             reinterpret_cast<void*>(Hook_D3D12CreateDevice),
                             reinterpret_cast<void**>(&g_OrigD3D12CreateDevice),
                             "D3D12CreateDevice");
            Logger::success("Hooked D3D12CreateDevice");
        } else {
            Logger::debug("Failed to get proc address for D3D12CreateDevice");
        }
    } else {
        Logger::debug("d3d12.dll not loaded (yet); D3D12 hooks not installed");
    }
}
#endif

// ==============================
// CreateSwapChainForCoreWindow hook
// ==============================

HRESULT CreateSwapchainForCoreWindowHook::CreateSwapChainForCoreWindowCallback(
    IDXGIFactory2 *This,
    ::IUnknown *pDevice,
    IUnknown *pWindow,
    DXGI_SWAP_CHAIN_DESC1 *pDesc,
    IDXGIOutput *pRestrictToOutput,
    IDXGISwapChain1 **ppSwapChain)
{
    winrt::com_ptr<ID3D12CommandQueue> pCommandQueue;
    Logger::debug("Recreating Swapchain");
    if (Client::settings.getSettingByName<bool>("killdx")->value) SwapchainHook::queue = nullptr;
    if (Client::settings.getSettingByName<bool>("killdx")->value && SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(pCommandQueue.put())))) {
        SwapchainHook::queue = nullptr;
        Logger::success("Fell back to DX11");
        return DXGI_ERROR_INVALID_CALL;
    }

    auto vsync = Client::settings.getSettingByName<bool>("vsync")->value;
    SwapchainHook::currentVsyncState = vsync;

    if (vsync) pDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    /* EXTRA RELEASING PRECAUTIONS */

    if (SwapchainHook::d3d11On12Device && !SwapchainHook::D3D11Resources.empty()) {
        std::vector<ID3D11Resource*> toRelease;
        toRelease.reserve(SwapchainHook::D3D11Resources.size());
        for (auto& res : SwapchainHook::D3D11Resources) if (res) toRelease.push_back(res.get());
        if (!toRelease.empty()) SwapchainHook::d3d11On12Device->ReleaseWrappedResources(toRelease.data(), static_cast<UINT>(toRelease.size()));
    }
    if (SwapchainHook::context) {
        SwapchainHook::context->ClearState();
        SwapchainHook::context->Flush();
    }
    if (D2D::context) {
        D2D::context->SetTarget(nullptr);
        D2D::context->Flush();
    }
    if (SwapchainHook::d3d12Device5 && SwapchainHook::queue) {
        winrt::com_ptr<ID3D12Fence> fence;
        if (SUCCEEDED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
            const UINT64 value = 1;
            HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (evt) {
                SwapchainHook::queue->Signal(fence.get(), value);
                if (fence->GetCompletedValue() < value) {
                    fence->SetEventOnCompletion(value, evt);
                    WaitForSingleObject(evt, 1000);
                }
                CloseHandle(evt);
            }
        }
    }

    /* EXTRA RELEASING PRECAUTIONS */

    SwapchainHook::queueReset = false;
    HRESULT hr = funcOriginal(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
    if (FAILED(hr)) {
        Logger::error("Failed to create swapchain: {}", Logger::getHRESULTError(hr));
    }

    return hr;
}

void CreateSwapchainForCoreWindowHook::hook(IDXGIFactory2 *pFactory) {
    // Hook CreateSwapChainForCoreWindow (existing)
    Memory::hookFunc((*(LPVOID **) pFactory)[16],
                     (void*)(CreateSwapChainForCoreWindowCallback),
                     (void **) &funcOriginal,
                     "CreateSwapchainForCoreWindow");

#if defined(__DEBUG__)
    // Also hook D3D11 & D3D12 CreateDevice exports so we can enable debug layers BEFORE device creation
    // HookD3D11Exports();
    // HookD3D12Exports();
#endif
}