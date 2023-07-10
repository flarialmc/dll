#include "Indetour.hpp"

Indetour::Indetour()
{
    // empty constructor (no need for anything)
}

bool Indetour::hook(void* function_address, void* hook_address)
{
    if (!function_address || !hook_address || this->is_hooked) { return false; }            // check if addresses are correct the nothing is hooked yet

#ifdef _WIN64
    byte shellcode[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // movabs rax, [address]
                         0x50,                                                              // push rax
                         0xC3 };                                                            // ret
    byte skip_bytes = 0x2;                                                                  // bytes to skip to address (x64)
#else
    byte shellcode[] = { 0xB8, 0x00, 0x00, 0x00, 0x00,                                      // mov eax, [address]
                         0x50,                                                              // push eax
                         0xC3 };                                                            // ret
    byte skip_bytes = 0x1;                                                                  // bytes to skip to address (x86)
#endif

    this->function_address = function_address;                                              // set the function address
    this->hook_address = hook_address;                                                      // set the hook address
    this->original_size = sizeof(shellcode);                                                // set the original function shellcode size
    this->original_shellcode = (byte*)malloc(sizeof(shellcode));                            // allocate memory for original function shellcode
    memcpy(this->original_shellcode, function_address, sizeof(shellcode));                  // save the original function shellcode

    Extra::memset_safe(function_address, 0x90, sizeof(shellcode));                         // overwrite original function with nop
    uintptr_t hook_fn = reinterpret_cast<uintptr_t>(hook_address);                          // recast hook address to uintptr_t
    memcpy((void*)((uintptr_t)shellcode + skip_bytes), &hook_fn, sizeof(uintptr_t));        // copy the hook address to shellcode

   Extra::wpm_safe(function_address, &shellcode, sizeof(shellcode));                      // overwrite the original function with new shellcode

    this->is_hooked = true;                                                                 // set the is_hooked flag to true

    return true;
}

bool Indetour::unhook()
{
    if (!this->is_hooked) { return false; }                                                 // check if the hook is indeed in place

    Extra::wpm_safe(function_address, this->original_shellcode, this->original_size);      // restore the original function shellcode

    free(this->original_shellcode);                                                         // free the allocated shellcode memory

    this->is_hooked = false;                                                                // set the is_hooked flag to false

    return true;
}