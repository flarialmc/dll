#pragma once

#include <windows.h>
#include <algorithm>

#include "src/Client/Client.hpp"
#include "src/Client/Events/EventHandler.hpp"
#include "src/Client/GUI/D2D.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
#include <kiero.h>



DWORD WINAPI init(HMODULE real)
{
#ifdef _DEBUG
    
#endif

    AllocConsole();
    SetConsoleTitleA("Flarial-Debugger");
    FILE* out;
    freopen_s(&out, ("CONOUT$"), ("w"), stdout);

    Client::initialize();
    Logger::info("Initializing Client");

   
    
    while (true) {
        if (Client::disable) {
            break;
        } else {
            Sleep(50);
        }
    }

    Client::SaveSettings();

    Logger::info("Uninitializing Client");

    EventHandler::unregisterAll();

    ModuleManager::terminate();
    HookManager::terminate();

    kiero::shutdown();

    Logger::debug("Shut down Kiero.");

    ResizeHook::CleanShit();

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
    case DLL_PROCESS_DETACH:
        ModuleManager::terminate();
    }

    return TRUE;
}
//48 8B ? 55 53 56 57 41 ? 41 ? 48 8D ? ? ? ? ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 44 0F ? ? ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 4D 8B