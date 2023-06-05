#include <windows.h>

#include "src/Client/Client.h"

Client client;

void initialize(LPVOID lparam)
{
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)initialize, new Client(), 0, nullptr);
        break;
    }

    return TRUE;
}
