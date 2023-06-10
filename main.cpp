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

        Memory::SafeRelease(SwapchainHook::queue);

        Memory::SafeRelease(SwapchainHook::D3D12DescriptorHeap);

        for (ID2D1Bitmap1* bitmap : SwapchainHook::D2D1Bitmaps)
        {
            Memory::SafeRelease(bitmap);
        }

        for (ID3D11Resource* resource : SwapchainHook::D3D11Resources)
        {
            Memory::SafeRelease(resource);
        }

        for (IDXGISurface1* surface : SwapchainHook::DXGISurfaces)
        {
            Memory::SafeRelease(surface);
        }

        SwapchainHook::D2D1Bitmaps.clear();
        SwapchainHook::D3D11Resources.clear();
        SwapchainHook::DXGISurfaces.clear();

        SwapchainHook::context->Flush();
        Memory::SafeRelease(SwapchainHook::context);


    }

    if(SwapchainHook::init) {

        Memory::SafeRelease(D2D::context);

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
