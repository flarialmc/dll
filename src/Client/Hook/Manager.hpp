#pragma once

#include <vector>

#include "Hooks/Hook.hpp"
#include "Hooks/Input/KeyHook.hpp"
#include "Hooks/Input/MouseHook.hpp"
#include "Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "Hooks/Visual/getFovHook.hpp"
#include "Hooks/Game/OnSuspend.hpp"

class HookManager {
public:
    static std::vector<std::shared_ptr<Hook>> hooks;

    template<typename T, typename... ArgsT>
    static void addHook(ArgsT... args) {
        auto hookPtr = std::make_shared<T>(args...);
        hooks.emplace_back(hookPtr);
    }

    static void initialize();

    static void terminate();

};
