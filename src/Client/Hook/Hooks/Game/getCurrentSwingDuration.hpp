#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"

class getCurrentSwingDuration : public Hook
{
private:

    static __int64 getCurrentSwingDurationCallback(__int64 a1) {

        __int64 thing = func_original(a1);

        return thing;
    }

public:
    typedef __int64(__thiscall* getCurrentSwingDurationOriginal)(__int64);
    static inline getCurrentSwingDurationOriginal func_original = nullptr;

    getCurrentSwingDuration() : Hook("getCurrentSwingDurationHook", "48 89 5C 24 ? 57 48 83 EC 20 48 8B 15 ? ? ? ? 48 8B F9 33 DB") {}

    void enableHook() override {
        this->autoHook(getCurrentSwingDurationCallback, (void**)&func_original);
    }
};


