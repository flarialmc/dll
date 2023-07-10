#include "Extra.hpp"

void* Extra::get_module(const char* module_name)
{
    return GetModuleHandleA(module_name);                                        
}

void* Extra::get_module_export(const char* module_name, const char* function_name)
{
    void* module = get_module(module_name);                                      

    if (!module) { return 0; }                                                     

    return GetProcAddress(reinterpret_cast<HMODULE>(module), function_name);      
}

bool Extra::wpm_safe(void* address, void* buffer, size_t size)
{
    if (!address || !buffer || !size) { return false; }                       

    DWORD op;
    VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &op);            

    memcpy(address, buffer, size);                                              

    VirtualProtect(address, size, op, &op);                                      

    return true;
}

bool Extra::memset_safe(void* address, int buffer, size_t size)
{
    if (!address || !buffer || !size) { return false; }                           

    DWORD op;
    VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &op);                   

    memset(address, buffer, size);                                                 

    VirtualProtect(address, size, op, &op);                                      

    return true;
}