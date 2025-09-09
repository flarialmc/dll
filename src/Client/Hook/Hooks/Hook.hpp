#pragma once

#include <minhook/MinHook.h>

class Hook {
public:
    uintptr_t address;
    std::string name;

    Hook(std::string name, uintptr_t address);

    virtual ~Hook();

    virtual void enableHook();

    bool autoHook(void *callbackPtr, void **funcOriginal);

    bool manualHook(void *hookAddr, void *callbackPtr, void **funcOriginal);
};