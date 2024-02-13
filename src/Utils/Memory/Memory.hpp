#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <type_traits>
#include <optional>
#include <future>
#include <execution>
#include <format>
#include "../Logger/Logger.hpp"
#include "MinHook.h"

#define in_range(x, a, b) (x >= a && x <= b)
#define get_bits(x) (in_range((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (in_range(x, '0', '9') ? x - '0' : 0))
#define get_byte(x) (get_bits(x[0]) << 4 | get_bits(x[1]))


template <typename Ret, typename Type> Ret& direct_access(Type* type, size_t offset) {
    union {
        size_t raw;
        Type* source;
        Ret* target;
    } u;
    u.source = type;
    u.raw += offset;
    return *u.target;
}

#define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name
#define DEF_FIELD_RW(type, name) __declspec(property(get = get##name, put = set##name)) type name

//fake class macro to avoid compile erros when using pragma once 

#define FK(typep) \
class typep;

#define FAKE_FIELD(type, name)                                                                                       \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name()

#define BUILD_ACCESS(ptr, type, name, offset)                                                                        \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name() const { return direct_access<type>(ptr, offset); }												     \
void set##name(type v) const { direct_access<type>(ptr, offset) = v; }



class Memory
{
public:

    template <unsigned int IIdx, typename TRet, typename... TArgs>
    static inline auto CallVFunc(void* thisptr, TArgs... argList) -> TRet {
        using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
    }

    template <typename TRet, typename... TArgs>
    static inline auto CallVFuncI(void* thisptr, int index, TArgs... argList) -> TRet {
        using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<Fn**>(thisptr))[index](thisptr, argList...);
    }

    static void hookFunc(void* pTarget, void* pDetour, void** ppOriginal, std::string name) {

        if (pTarget == NULL) {
            Logger::error(std::format("{} has invalid address", name));
            return;
        }

        if (MH_CreateHook(pTarget, pDetour, ppOriginal) != MH_OK) {
            Logger::error(std::format("Failed to hook {} function", name));
            return;
        }

        MH_EnableHook(pTarget);

        Logger::info(std::format("Successfully hooked {} function at {}", name, pTarget));
    }

    template <typename R, typename... Args>
    static inline R CallFunc(void* func, Args... args)
    {
        return ((R(*)(Args...))func)(args...);
    }

    template <unsigned int index>
    static void HookVFunc(uintptr_t sigOffset, void* pDetour, void** ppOriginal, std::string name) {
        uintptr_t** vTable = reinterpret_cast<uintptr_t**>(sigOffset + 3 + 7);

        hookFunc(vTable[index], pDetour, ppOriginal, name);
    }


    static auto findSig(const char* signature)
    {

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

        // ...

        auto gameModule = GetModuleHandleA("Minecraft.Windows.exe");
        auto* const scanBytes = reinterpret_cast<uint8_t*>(gameModule);
        auto* const dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(gameModule);
        auto* const ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(scanBytes + dosHeader->e_lfanew);
        const auto sizeOfCode = ntHeaders->OptionalHeader.SizeOfImage;

        const auto pattern = pattern_to_byte(signature);
        const auto end = scanBytes + sizeOfCode;

        auto it = std::search(std::execution::par, scanBytes, end, pattern.cbegin(), pattern.cend(),
                              [](auto byte, auto opt) {
                                  return !opt.has_value() || *opt == byte;
                              });

        auto ret = it != end ? (uintptr_t)it : 0u;
        return ret;
    }


    template<typename T>
    static void SafeRelease(T*& pPtr)
    {
        if (pPtr != nullptr)
        {
            pPtr->Release();
            pPtr = nullptr;
        }
    }

    static uintptr_t findDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
    {
        uintptr_t addr = ptr;

        for (int i = 0; i < offsets.size(); ++i)
        {
            addr = *(uintptr_t *)addr;
            addr += offsets[i];
        }
        return addr;
    }
    
    static void patchBytes(void* dst, void* src, unsigned int size) {
        DWORD oldprotect;
        VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memcpy(dst, src, size);
        VirtualProtect(dst, size, oldprotect, &oldprotect);
    }

    static inline uintptr_t offsetFromSig(uintptr_t sig, int offset) {
        return sig + offset + 4 + *reinterpret_cast<int*>(sig + offset);
    }

};
