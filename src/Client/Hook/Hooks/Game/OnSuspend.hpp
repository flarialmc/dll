#pragma once
#include "../Hook.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
class OnSuspendHook : public Hook {

private:
    static void callback(void* a1, void* a2, void* a3, void* a4);

public:
    typedef void(__thiscall* original)(void*, void*, void*, void*);

    static inline original suspendOriginal = nullptr;

    OnSuspendHook() : Hook("OnSuspendHook", "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B E9 45 33 F6") {}

    void enableHook() override;
};