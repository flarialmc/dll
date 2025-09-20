#include "../DXGI/CreateSwapchainForCoreWindowHook.hpp"
#include <winrt/base.h>
#include <d3d12.h>
#include <windows.h>

#if defined(__DEBUG__)

typedef HRESULT (WINAPI *PFN_D3D12CreateDevice)(
    IUnknown*,
    D3D_FEATURE_LEVEL,
    REFIID,
    void**);

static PFN_D3D12CreateDevice g_OrigD3D12CreateDevice = nullptr;

static void EnableD3D12DebugLayerIfAvailable() {
    winrt::com_ptr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.put())))) {
        debugController->EnableDebugLayer();
        Logger::success("D3D12 debug layer enabled");
    } else {
        Logger::debug("D3D12GetDebugInterface failed (Graphics Tools not installed?) — continuing without D3D12 debug layer");
    }
}

static HRESULT WINAPI Hook_D3D12CreateDevice(
    IUnknown* pAdapter,
    D3D_FEATURE_LEVEL MinimumFeatureLevel,
    REFIID riid,
    void** ppDevice)
{
    EnableD3D12DebugLayerIfAvailable();
    return g_OrigD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
}


void CreateSwapchainForCoreWindowHook::HookD3D12Exports() {
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