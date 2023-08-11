#include "getViewPerspective.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/SnapLook/SnapLook.hpp"


uintptr_t findSig(const char* pattern) {
    MODULEINFO info;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandleA("Minecraft.Windows.exe"), &info, sizeof(MODULEINFO));

    MEMORY_BASIC_INFORMATION mbi{ 0 };
    uintptr_t protectFlags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

    uintptr_t firstMatch = 0;
    auto start = (uintptr_t)GetModuleHandleA("Minecraft.Windows.exe");
    auto end = start + info.SizeOfImage;
    auto pat = pattern;

    for (uintptr_t i = start; i < end; i++)
    {
        if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi)))
        {
            if (mbi.Protect & protectFlags || !(mbi.State & MEM_COMMIT))
            {
                i += mbi.RegionSize;
                continue;
            }
            for (uintptr_t j = (uintptr_t)mbi.BaseAddress; j < (uintptr_t)mbi.BaseAddress + mbi.RegionSize; j++)
            {
                if (!*pat)
                {
                    return firstMatch;
                }
                if (*(PBYTE)pat == '\?' || *(BYTE*)j == get_byte(pat))
                {
                    if (!firstMatch)
                        firstMatch = j;
                    if (!pat[2])
                    {
                        return firstMatch;
                    }
                    if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
                        pat += 3;
                    else
                        pat += 2;
                }
                else
                {
                    pat = pattern;
                    firstMatch = 0;
                }
            }
        }
    }
    return 0;
}
//if (module->settings.getSettingByName<bool>("enabled")->value) {
int getViewPerspectiveHook::callback(uintptr_t* a1) {



    auto mod = ModuleManager::getModule("SnapLook");
    auto freemod = ModuleManager::getModule("FreeLook");

    if(freemod != nullptr) {
        if (freemod->settings.getSettingByName<bool>("enabled")->value &&
            SDK::raknetConnector->JoinedIp.find("hive") != std::string::npos) {
            FlarialGUI::Notify("Can't use freelook on " + SDK::raknetConnector->JoinedIp);
            freemod->settings.getSettingByName<bool>("enabled")->value = false;
        }
    }

        if (mod != nullptr && SDK::CurrentScreen == "hud_screen") {

        if (mod->settings.getSettingByName<bool>("enabled")->value) {
            return 2;
        }

        if(freemod != nullptr)
            if (freemod->settings.getSettingByName<bool>("enabled")->value) {
                return 1;
            }
    }
	return getViewPerspectiveOriginal(a1);
}

void getViewPerspectiveHook::enableHook() {
	this->manualHook((void*)findSig("48 83 EC ? 48 8B ? 48 8D ? ? ? 41 B8 ? ? ? ? 48 8B ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 8B ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 83 C4 ? C3 8B 42 ? 48 83 C4 ? C3 E8 ? ? ? ? CC CC CC CC CC 48 83 EC ? 48 8B ? 48 8D ? ? ? 41 B8 ? ? ? ? 48 8B ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 8B ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 83 C4 ? C3 8B 42 ? 48 83 C4 ? C3 E8 ? ? ? ? CC CC CC CC CC 48 83 EC ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 48 8B ? 41 B8 ? ? ? ? 88 54 ? ? 48 8D ? ? ? 48 8B ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 8D ? ? ? E8 ? ? ? ? 48 8B ? ? ? 48 33 ? E8 ? ? ? ? 48 83 C4 ? C3 E8 ? ? ? ? CC CC CC CC CC CC CC CC 48 83 EC ? 48 8B ? 48 8D ? ? ? 41 B8 ? ? ? ? 48 8B ? ? FF 15 ? ? ? ? 48 8B ? 48 85 ? 74 ? 48 8B ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? E8 ? ? ? ? 48 83 C4 ? C3 0F B6 ? ? 48 83 C4 ? C3 E8 ? ? ? ? CC CC CC CC 40"), (void*)callback,  (void**)&getViewPerspectiveOriginal);
}
