#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

class getTimeOfDayHook : public Hook {

private:
    static float callback(__int64 a1, int a2, float a3);

public:
    typedef float(__thiscall* original)(__int64 a1, int a2, float a3);

    static inline original funcOriginal = nullptr;

    getTimeOfDayHook();

    void enableHook() override;
};