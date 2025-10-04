#include "Hook.hpp"
#include <Utils/Logger/Logger.hpp>
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
        Logger::custom(fg(fmt::color::crimson), "Hook", "Failed to find address of {}", this->name);
        return false;
    }

    MH_CreateHook(hookAddr, callbackPtr, funcOriginal);
    if (MH_QueueEnableHook(hookAddr) != MH_OK) {
        Logger::custom(fg(fmt::color::crimson), "Hook", "Failed to find address of {}", this->name);
        return false;
    }

    Logger::custom(fg(fmt::color::deep_sky_blue), "Hook", "Hooked {}", this->name);
    return true;
}