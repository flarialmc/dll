#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class HurtColorHook : public Hook {
private:

    static MCCColor *HurtColorCallback(void *a1, MCCColor *color, void *a3);

public:
    typedef MCCColor *(__thiscall *HurtColorOriginal)(void *a1, MCCColor *color, void *a3);

    static inline HurtColorOriginal funcOriginal = nullptr;

    HurtColorHook();

    void enableHook() override;
};


