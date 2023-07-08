#pragma once
#include <vector>

#include "Hooks/Hook.hpp"
#include "Hooks/Input/KeyHook.hpp"
#include "Hooks/Input/MouseHook.hpp"
#include "Hooks/Render/SwapchainHook.hpp"
#include "Hooks/Visual/getFovHook.hpp"
#include "Hooks/Game/OnSuspend.hpp"
#include "MinHook.h"

class HookManager
{
public:
    static std::vector<Hook *> hooks;
    static void initialize();
    static void terminate();

};
