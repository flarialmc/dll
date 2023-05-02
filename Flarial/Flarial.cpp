#include "Client/Client.h"
#include "Client/SDK/Minecraft.h"
#include "Client/Config/Config.h"

void init(HMODULE hModule) {

    // Logger::init();

    new Client("Flarial");

};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)init, hModule, 0, 0);
    
    return TRUE;

};