#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Module/Manager.hpp"

class TimeChangerHook : public Hook {
private:

    static float TimeChangerCallback(void *a1, void *a2, void *a3);

public:
    typedef float(__thiscall *TimeChangerOriginal)(void *a1, void *color, void *a3);

    static inline TimeChangerOriginal func_original = nullptr;

    TimeChangerHook();

    void enableHook() override;
};


