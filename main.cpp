#pragma once
#include <windows.h>
#include "src/Client/Client.hpp"
#include "src/Client/Events/EventHandler.hpp"

#include <kiero.h>

HMODULE real = nullptr;

DWORD WINAPI init()
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
    FreeConsole();
    EventHandler::unregisterAll();
    ModuleManager::terminate();
    HookManager::terminate();
    kiero::shutdown();

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    FreeLibraryAndExitThread(real, 1);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        real = hModule;
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, nullptr, 0, nullptr);
        break;
    }

    return TRUE;
}
