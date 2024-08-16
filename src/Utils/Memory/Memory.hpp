#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <utility>
#include <vector>
#include <type_traits>
#include <optional>
#include <future>
#include <execution>
#include <format>
#include "../Logger/Logger.hpp"
#include "MinHook.h"
#include <libhat/Scanner.hpp>

#define in_range(x, a, b) (x >= a && x <= b)
#define get_bits(x) (in_range((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (in_range(x, '0', '9') ? x - '0' : 0))
#define get_byte(x) (get_bits(x[0]) << 4 | get_bits(x[1]))


template<typename Ret, typename Type>
Ret &direct_access(Type *type, size_t offset) {
    union {
        size_t raw;
        Type *source;
        Ret *target;
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
type get##name() const { return direct_access<type>(ptr, offset); }                                                     \
void set##name(type v) const { direct_access<type>(ptr, offset) = v; }


class Memory {
public:

    template<unsigned int IIdx, typename TRet, typename... TArgs>
    static inline auto CallVFunc(void *thisptr, TArgs... argList) -> TRet {
        using Fn = TRet(__thiscall *)(void *, decltype(argList)...);
        return (*static_cast<Fn **>(thisptr))[IIdx](thisptr, std::forward<TArgs>(argList)...);
    }

    template <typename TRet, typename... TArgs>
    static auto CallVFuncI(uint32_t index, void* thisptr, TArgs... argList) -> TRet {
        using Fn = TRet(__thiscall*)(void*, TArgs...);
        return (*static_cast<Fn**>(thisptr))[index](thisptr, std::forward<TArgs>(argList)...);
    }

    static void hookFunc(void *pTarget, void *pDetour, void **ppOriginal, std::string name) {

        if (pTarget == nullptr) {
            Logger::error(std::format("[vFunc Hook] {} has invalid address", name));
            return;
        }

        if (MH_CreateHook(pTarget, pDetour, ppOriginal) != MH_OK) {
            Logger::error(std::format("[vFunc Hook] Failed to hook {} function", name));
            return;
        }

        MH_EnableHook(pTarget);

        Logger::info(std::format("[vFunc Hook] Successfully hooked {} function at {}", name, pTarget));
    }

    template<typename R, typename... Args>
    static inline R CallFunc(void *func, Args... args) {
        return ((R(*)(Args...)) func)(args...);
    }

    template<unsigned int index>
    static void HookVFunc(uintptr_t sigOffset, void *pDetour, void **ppOriginal, std::string name) {
        auto **vTable = reinterpret_cast<uintptr_t **>(sigOffset + 3 + 7);

        hookFunc(vTable[index], pDetour, ppOriginal, std::move(name));
    }

    static uintptr_t findSig(std::string_view signature) {
        const auto parsed = hat::parse_signature(signature);
        if (!parsed.has_value()) {
            Logger::debug("[ Runtime Scanner ] Failed to parse signature: " + std::string(signature));
            return 0u;
        }

        const auto result = hat::find_pattern(parsed.value());

        if (!result.has_result()) {
            Logger::debug("[ Runtime Scanner ] Failed to find signature: " + std::string(signature));
            return 0u;
        }

        return reinterpret_cast<uintptr_t>(result.get());
    }


    template<typename T>
    static void SafeRelease(T *&pPtr) {
        if (pPtr != nullptr) {
            pPtr->Release();
            pPtr = nullptr;
        }
    }

    static uintptr_t findDMAAddy(uintptr_t ptr, const std::vector<unsigned int>& offsets) {
        uintptr_t addr = ptr;

        for (unsigned int offset : offsets) {
            addr = *(uintptr_t *) addr;
            addr += offset;
        }
        return addr;
    }

    static void nopBytes(void* dst, const unsigned int size) {
        if (dst == nullptr)
            return;

        DWORD oldprotect;
        VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memset(dst, 0x90, size);
        VirtualProtect(dst, size, oldprotect, &oldprotect);
    }

    static void copyBytes(void* src, void* dst, const unsigned int size) {
        if (src == nullptr || dst == nullptr)
            return;

        DWORD oldprotect;
        VirtualProtect(src, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memcpy(dst, src, size);
        VirtualProtect(src, size, oldprotect, &oldprotect);
    }

    static void patchBytes(void *dst, const void *src, const unsigned int size) {
        if (src == nullptr || dst == nullptr)
            return;

        DWORD oldprotect;
        VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memcpy(dst, src, size);
        VirtualProtect(dst, size, oldprotect, &oldprotect);
    }

    static inline uintptr_t offsetFromSig(uintptr_t sig, int offset) { // REL RIP ADDR RESOLVER
        // pointer is relative to the code it is in - it is how far to the left in bytes you need to move to get to the value it points to,
        // this function returns absolute address in memory ("(sig)A B C (+offset)? ? ? ?(+4)(from here + bytes to move to get to value pointer points to) D E F")
        // offset val = *reinterpret_cast<int *>(sig + offset)
        // base = sig + offset + 4
        return sig + offset + 4 + *reinterpret_cast<int *>(sig + offset);
    }

    template<typename Ret>
    static Ret getOffsetFromSig(const uintptr_t sig, const int offset) {
        return reinterpret_cast<Ret>(offsetFromSig(sig, offset));
    }

    static inline std::array<std::byte, 4> getRipRel(uintptr_t instructionAddress, uintptr_t targetAddress) {
        uintptr_t relAddress = targetAddress - (instructionAddress + 4); // 4 bytes for RIP-relative addressing
        std::array<std::byte, 4> relRipBytes{};

        for (size_t i = 0; i < 4; ++i) {
            relRipBytes[i] = static_cast<std::byte>((relAddress >> (i * 8)) & 0xFF);
        }

        return relRipBytes;
    }

    static inline uintptr_t GetAddressByIndex(uintptr_t vtable, int index) {
        return (*reinterpret_cast<uintptr_t*>(vtable + 8 * index));
    }
};
