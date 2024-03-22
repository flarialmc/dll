#pragma once

#include "MinHook.h"
#include <string>
#include <format>

#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Memory/Memory.hpp"

class Hook {
public:
    std::string signature;
    std::string name;

    Hook(std::string name, std::string signature);

    virtual ~Hook();

    virtual void enableHook();

    bool autoHook(void *callbackPtr, void **funcOriginal);

    bool manualHook(void *hookAddr, void *callbackPtr, void **funcOriginal);
};