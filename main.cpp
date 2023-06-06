#include <windows.h>

#include "src/Client/Client.hpp"

Client* client;

void initialize()
{
    client->initialize();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        client = new Client();
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)initialize, nullptr, 0, nullptr);
        break;
    }

    return TRUE;
}
