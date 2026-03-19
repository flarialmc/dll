#pragma once

#include <minhook/MinHook.h>

class Hook {
public:
    uintptr_t address;
    std::string name;

    Hook(std::string name, uintptr_t address);

    virtual ~Hook();

    virtual void enableHook();

    /// Hooks the stored address with the given callback; fails silently if address is 0.
    bool autoHook(void *callbackPtr, void **funcOriginal);

    /// Hooks a specified address with the callback and queues it for batch enabling.
    bool manualHook(void *hookAddr, void *callbackPtr, void **funcOriginal);
};