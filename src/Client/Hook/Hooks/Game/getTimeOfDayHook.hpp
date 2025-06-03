#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

class getTimeOfDayHook : public Hook {

private:
    static int callback(void* a1);

public:
    typedef int(__thiscall* original)(void* a1);

    static inline original funcOriginal = nullptr;

    getTimeOfDayHook();

    void enableHook() override;
};