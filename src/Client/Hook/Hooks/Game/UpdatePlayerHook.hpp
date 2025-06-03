#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"
#include "../../../../SDK/Client/Actor/Gamemode.hpp"

class UpdatePlayerHook : public Hook {

private:
    static void callback(void* a1, void* a2, void* a3);

public:
    typedef void(__thiscall *original)(void* a1, void* a2, void* a3);

    static inline original funcOriginal = nullptr;

    UpdatePlayerHook();

    void enableHook() override;
};