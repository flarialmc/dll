#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <type_traits>

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
  

    static uintptr_t findSig(const char *pattern)
    {
        MODULEINFO info;
        GetModuleInformation(GetCurrentProcess(), GetModuleHandleA("Minecraft.Windows.exe"), &info, sizeof(MODULEINFO));

        MEMORY_BASIC_INFORMATION mbi{0};
        uintptr_t protectFlags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

        uintptr_t firstMatch = 0;
        auto start = (uintptr_t)GetModuleHandleA("Minecraft.Windows.exe");
        auto end = start + info.SizeOfImage;
        auto pat = pattern;

        /*BYTE patByte = get_byte(pat);
        const char *oldPat = pat;*/

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
                    if (*(PBYTE)pat == '\?' || *(BYTE *)j == get_byte(pat))
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
    
};
