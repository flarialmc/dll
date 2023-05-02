#include "Mem.h"
#include "../Utils/Logger.h"

bool Mem::MHInitialized = false;

HMODULE Mem::getDll() {

	MEMORY_BASIC_INFORMATION info;
	size_t len = VirtualQueryEx(GetCurrentProcess(), (void*)getDll, &info, sizeof(info));
	assert(len == sizeof(info));
	return len ? (HMODULE)info.AllocationBase : NULL;

};

uintptr_t Mem::findSig(const char* signature) {
	static auto pattern_to_byte = [](const char* pattern) {

		auto bytes = std::vector<std::optional<uint8_t>>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);
		bytes.reserve(strlen(pattern) / 2);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(std::nullopt);
			}
			else bytes.push_back((uint8_t)strtoul(current, &current, 16));
		}
		return bytes;

	};

	auto gameModule = (unsigned long long)(GetModuleHandleA("Minecraft.Windows.exe"));
	auto* const scanBytes = reinterpret_cast<uint8_t*>(gameModule);
	auto* const dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(gameModule);
	auto* const ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(scanBytes + dosHeader->e_lfanew);
	const auto sizeOfCode = ntHeaders->OptionalHeader.SizeOfImage;

	const auto pattern = pattern_to_byte(signature);
	const auto end = scanBytes + sizeOfCode;

	auto it = std::search(
		scanBytes, end,
		pattern.cbegin(), pattern.cend(),
		[](auto byte, auto opt) {
			return !opt.has_value() || *opt == byte;
		});

	auto ret = it != end ? (unsigned long long)it : 0u;
	if (!ret)
		return 0;

	return ret;
};

uintptr_t Mem::findMultiLvlPtr(unsigned long long baseAddr, std::vector<unsigned int> offsets, bool basePure) {

	auto ptr = (unsigned long long)NULL;
	auto i = 0;

	if (basePure) {
		ptr = baseAddr;
	}
	else {
		auto hwnd = GetModuleHandleA("Minecraft.Windows.exe");
		ptr = (unsigned long long)(hwnd) + baseAddr;
	};

	do {

		if (*(unsigned long long*)ptr + offsets[i] == offsets[i] || *(unsigned long long*)ptr + offsets[i] > 0xFFFFFFFFFFFF)
			break;

		ptr = *(unsigned long long*)ptr + offsets[i];

		if (ptr == NULL)
			break;

		i++;

	} while (i < offsets.size());

	return (i == offsets.size() ? ptr : NULL);

};

void Mem::hookFunc(void* pTarget, void* pDetour, void** ppOriginal, std::string name) {
	if (pTarget == NULL) {
		Logger::error(std::format("{} has invalid address", name));
		return;
	}

	if (!MHInitialized) {
		MHInitialized = true;

		if (MH_Initialize() != MH_OK) {
			Logger::fatal("Failed to initialize MinHook");
			return;
		}
	}

	if (MH_CreateHook(pTarget, pDetour, ppOriginal) != MH_OK) {
		Logger::error(std::format("Failed to hook {} function", name));
		return;
	}

	Logger::info(std::format("Successfully hooked {} function at {}", name, pTarget));
}