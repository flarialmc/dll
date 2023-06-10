#pragma once
#include <windows.h>
#include "src/Client/Client.hpp"
#include "src/Client/Events/EventHandler.hpp"
#include "src/Client/GUI/D2D.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
#include <kiero.h>

DWORD WINAPI init(HMODULE real)
{

    Client::initialize();
    Logger::info("Initializing Client");

    while (true) {
        if (Client::disable) {
            break;
        } else {
            Sleep(50);
        }
    }

    Logger::info("Uninitializing Client");

    EventHandler::unregisterAll();

    if(SwapchainHook::init && SwapchainHook::d3d11On12Device != nullptr) {

        Logger::debug("Releasing d3d12 resources");

        SwapchainHook::queue->Release();
        SwapchainHook::queue = nullptr;

        SwapchainHook::D3D12DescriptorHeap->Release();
        SwapchainHook::D3D12DescriptorHeap = nullptr;


        for (ID2D1Bitmap1* bitmap : SwapchainHook::D2D1Bitmaps)
        {
            bitmap->Release();
            bitmap = nullptr;
        }

        for (ID3D11Resource* resource : SwapchainHook::D3D11Resources)
        {
            SwapchainHook::d3d11On12Device->ReleaseWrappedResources(&resource, 1);
            resource->Release();
            resource = nullptr;
        }

        for (IDXGISurface1* surface : SwapchainHook::DXGISurfaces)
        {
            surface->Release();
            surface = nullptr;
        }

        SwapchainHook::D2D1Bitmaps.clear();
        SwapchainHook::D3D11Resources.clear();
        SwapchainHook::DXGISurfaces.clear();


        SwapchainHook::d3d11On12Device->Release();
        SwapchainHook::d3d11On12Device = nullptr;

        SwapchainHook::context->Flush();
        SwapchainHook::context->Release();
        SwapchainHook::context = nullptr;


    }

    if(SwapchainHook::init) {

        Logger::debug("Clearing normal stuff");

        D2D::context->Release();
        D2D::context = nullptr;

        SwapchainHook::init = false;
    }

    ModuleManager::terminate();
    HookManager::terminate();

    kiero::shutdown();

    Logger::debug("Shut down Kiero.");


    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    Logger::debug("Freeing Library.");

    Sleep(100);

    FreeLibraryAndExitThread(real, 1);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, instance, 0, nullptr);
        break;
    }

    return TRUE;
}
