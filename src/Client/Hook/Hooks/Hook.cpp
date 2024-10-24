#include "Hook.hpp"
#include <libhat/Scanner.hpp>
#include <utility>

Hook::Hook(std::string name, uintptr_t address) {
    this->name = std::move(name);
    this->address = address;
}

Hook::~Hook() = default;

void Hook::enableHook() {
}

bool Hook::autoHook(void *callbackPtr, void **funcOriginal) {
    if(address == 0) return false;
    return this->manualHook((void *) address, callbackPtr, funcOriginal);
}

bool Hook::manualHook(void *hookAddr, void *callbackPtr, void **funcOriginal) {
    if (hookAddr == nullptr) {
        Logger::error(std::format("[Hook] Failed to find address of {}", this->name));
        return false;
    }

    MH_CreateHook(hookAddr, callbackPtr, funcOriginal);
    if (MH_EnableHook(hookAddr) != MH_OK) {
        Logger::error(std::format("[Hook] Failed to find address of {}", this->name));
        return false;
    } else {
        Logger::info(std::format("[Hook] Successfully hooked {}", this->name));
    }

    return true;
}