#include "../DXGI/CreateSwapchainForCoreWindowHook.hpp"
#include <winrt/base.h>
#include <d3d11.h>
#include <windows.h>

#if defined(__DEBUG__)

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

static PFN_D3D11CreateDevice g_OrigD3D11CreateDevice = nullptr;
static PFN_D3D11CreateDeviceAndSwapChain g_OrigD3D11CreateDeviceAndSwapChain = nullptr;

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

// Install hooks (D3D11)
// ==============================

void CreateSwapchainForCoreWindowHook::HookD3D11Exports() {
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
#endif
