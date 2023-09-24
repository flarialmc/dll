#include "getViewPerspective.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/SnapLook/SnapLook.hpp"
#include "RaknetTick.hpp"


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

	if (freemod != nullptr) {


		if (freemod->settings.getSettingByName<bool>("enabled")->value && RaknetTickHook::towriteip.find("hive") != std::string::npos || freemod->settings.getSettingByName<bool>("enabled")->value && RaknetTickHook::towriteip.find("galaxite") != std::string::npos) {
			FlarialGUI::Notify("Can't use freelook on " + RaknetTickHook::towriteip);
			freemod->settings.getSettingByName<bool>("enabled")->value = false;
		}
	}

	if (mod != nullptr && SDK::CurrentScreen == "hud_screen") {

		if (mod->settings.getSettingByName<bool>("enabled")->value) {
			return 2;
		}

		if (freemod != nullptr)
			if (freemod->settings.getSettingByName<bool>("enabled")->value) {
				return 1;
			}
	}
	return getViewPerspectiveOriginal(a1);
}

void getViewPerspectiveHook::enableHook() {
	this->manualHook((void*)findSig("48 83 EC ? 48 8B 01 48 8D 54 ? ? 41 B8 03 00 00 00"), (void*)callback, (void**)&getViewPerspectiveOriginal);
}
