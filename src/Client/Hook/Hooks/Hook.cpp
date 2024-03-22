#include "Hook.hpp"
#include <libhat/Scanner.hpp>
#include <utility>

Hook::Hook(std::string name, std::string signature) {
    this->name = std::move(name);
    this->signature = std::move(signature);
}

Hook::~Hook() = default;

void Hook::enableHook() {
}

bool Hook::autoHook(void *callbackPtr, void **funcOriginal) {
    uintptr_t hookAddr = Memory::findSig(this->signature);
    return this->manualHook((void *) hookAddr, callbackPtr, funcOriginal);
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