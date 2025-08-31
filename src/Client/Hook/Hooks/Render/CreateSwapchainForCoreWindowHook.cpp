#include "CreateSwapchainForCoreWindowHook.hpp"
#include <winrt/base.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <windows.h>

using ::IUnknown;

// ==============================
// Swapchain hook (always active)
// ==============================
HRESULT CreateSwapchainForCoreWindowHook::CreateSwapChainForCoreWindowCallback(
    IDXGIFactory2 *This,
    ::IUnknown *pDevice,
    IUnknown *pWindow,
    DXGI_SWAP_CHAIN_DESC1 *pDesc,
    IDXGISwapChain1 **ppSwapChain)
{
    winrt::com_ptr<ID3D12CommandQueue> pCommandQueue;
    Logger::debug("Recreating Swapchain");

    auto vsync = Client::settings.getSettingByName<bool>("vsync")->value;
    SwapchainHook::currentVsyncState = vsync;

    if (vsync) pDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    if (!SwapchainHook::queue.get()) {
        winrt::com_ptr<ID3D12CommandQueue> tempQueue;
        if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(tempQueue.put())))) {
            SwapchainHook::queue = tempQueue;
            Logger::debug("Captured D3D12 command queue from device");
        }
    }

    SwapchainHook::queueReset = false;
    HRESULT hr = funcOriginal(This, pDevice, pWindow, pDesc, nullptr, ppSwapChain);
    if (FAILED(hr)) {
        Logger::error("Failed to create swapchain: {}", Logger::getHRESULTError(hr));
    }

    return hr;
}

// ==============================
// Debug layer hooks (only in __DEBUG__)
// ==============================
#if defined(__DEBUG__)
typedef HRESULT(WINAPI *PFN_D3D11CreateDevice)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
    const D3D_FEATURE_LEVEL*, UINT, UINT,
    ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

typedef HRESULT(WINAPI *PFN_D3D11CreateDeviceAndSwapChain)(
    IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
    const D3D_FEATURE_LEVEL*, UINT, UINT,
    const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**,
    ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

typedef HRESULT(WINAPI *PFN_D3D12CreateDevice)(IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**);

static PFN_D3D11CreateDevice g_OrigD3D11CreateDevice = nullptr;
static PFN_D3D11CreateDeviceAndSwapChain g_OrigD3D11CreateDeviceAndSwapChain = nullptr;
static PFN_D3D12CreateDevice g_OrigD3D12CreateDevice = nullptr;

static void EnableD3D12DebugLayerIfAvailable() {
    winrt::com_ptr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.put())))) {
        debugController->EnableDebugLayer();
        Logger::success("D3D12 debug layer enabled");
    } else {
        Logger::debug("D3D12GetDebugInterface failed — continuing without debug layer");
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
    HRESULT hr = orig(pAdapter, DriverType, Software, flagsWithDebug,
                      pFeatureLevels, FeatureLevels, SDKVersion,
                      ppDevice, pFeatureLevel, ppImmediateContext);
    if (FAILED(hr) && hr == E_INVALIDARG) {
        Logger::debug("Debug layer unavailable; retrying without debug flag");
        hr = orig(pAdapter, DriverType, Software, Flags,
                  pFeatureLevels, FeatureLevels, SDKVersion,
                  ppDevice, pFeatureLevel, ppImmediateContext);
    }
    return hr;
}

static HRESULT WINAPI Hook_D3D11CreateDevice(
    IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
    UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
    UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext)
{
    return TryD3D11CreateDeviceWithOptionalDebug(
        g_OrigD3D11CreateDevice,
        pAdapter, DriverType, Software, Flags,
        pFeatureLevels, FeatureLevels, SDKVersion,
        ppDevice, pFeatureLevel, ppImmediateContext);
}

static HRESULT WINAPI Hook_D3D12CreateDevice(
    IUnknown* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel,
    REFIID riid, void** ppDevice)
{
    EnableD3D12DebugLayerIfAvailable();
    return g_OrigD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
}

static void HookD3D11Exports() {
    HMODULE hD3D11 = GetModuleHandleW(L"d3d11.dll");
    if (!hD3D11) hD3D11 = LoadLibraryW(L"d3d11.dll");
    if (!hD3D11) return;

    FARPROC pCreateDevice = GetProcAddress(hD3D11, "D3D11CreateDevice");
    if (pCreateDevice) {
        Memory::hookFunc(reinterpret_cast<LPVOID>(pCreateDevice),
                         reinterpret_cast<void*>(Hook_D3D11CreateDevice),
                         reinterpret_cast<void**>(&g_OrigD3D11CreateDevice),
                         "D3D11CreateDevice");
    }
}

static void HookD3D12Exports() {
    HMODULE hD3D12 = GetModuleHandleW(L"d3d12.dll");
    if (!hD3D12) hD3D12 = LoadLibraryW(L"d3d12.dll");
    if (!hD3D12) return;

    FARPROC pCreateDevice = GetProcAddress(hD3D12, "D3D12CreateDevice");
    if (pCreateDevice) {
        Memory::hookFunc(reinterpret_cast<LPVOID>(pCreateDevice),
                         reinterpret_cast<void*>(Hook_D3D12CreateDevice),
                         reinterpret_cast<void**>(&g_OrigD3D12CreateDevice),
                         "D3D12CreateDevice");
    }
}
#endif // __DEBUG__

// ==============================
// Install hooks
// ==============================
void CreateSwapchainForCoreWindowHook::hook(IDXGIFactory2 *pFactory) {
    // Hook CreateSwapChainForCoreWindow (always)
    Memory::hookFunc((*(LPVOID **)pFactory)[16],
                     (void*)(CreateSwapChainForCoreWindowCallback),
                     (void**)&funcOriginal,
                     "CreateSwapchainForCoreWindow");

#if defined(__DEBUG__)
    // Hook D3D11/D3D12 exports only in debug mode
    HookD3D11Exports();
    HookD3D12Exports();
#endif
}
