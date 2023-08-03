#include "Hook.hpp"

Hook::Hook(std::string name, std::string signature)
{
    this->name = name;
    this->signature = signature;
}

Hook::~Hook()
{
}

void Hook::enableHook()
{
}

bool Hook::autoHook(void *callback_ptr, void **func_original)
{
    uintptr_t hook_addr = Memory::findSig(this->signature.c_str());
    return this->manualHook((void *)hook_addr, callback_ptr, func_original);
}

bool Hook::manualHook(void *hook_addr, void *callback_ptr, void **func_original)
{
    if (hook_addr == 0)
    {
        Logger::error(std::format("Failed to find address of {}", this->name));
        return false;
    }

    MH_CreateHook(hook_addr, callback_ptr, func_original);
    if (MH_EnableHook(hook_addr) != MH_OK)
    {
        Logger::error(std::format("Failed to find address of {}", this->name));
        return false;
    } else {
        Logger::info(std::format("[Hook] Successfully hooked {}", this->name));
    }

    return true;
}