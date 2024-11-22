#pragma once

#include "MinHook.h"
#include <string>
#include <format>

#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Memory/Memory.hpp"
#include "../../Events/EventManager.hpp"

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