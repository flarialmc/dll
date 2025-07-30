#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class TintColorHook : public Hook {
private:

    static MCCColor *TintColorCallback(void *a1, MCCColor *color, void *a3);

public:
    typedef MCCColor *(__thiscall *TintColorOriginal)(void *a1, MCCColor *color, void *a3);

    static inline TintColorOriginal funcOriginal = nullptr;

    TintColorHook();

    void enableHook() override;
};


