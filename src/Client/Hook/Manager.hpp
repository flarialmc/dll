#pragma once
#include <vector>

#include "Hooks/Hook.hpp"
#include "Hooks/Input/KeyHook.hpp"

#include "MinHook.h"

namespace HookManager
{
    inline std::vector<Hook *> hooks;
    void initialize();
    void terminate();
};
