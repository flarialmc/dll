#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class getCurrentSwingDuration : public Hook {
private:

    static __int64 getCurrentSwingDurationCallback(__int64 a1) {

        __int64 thing = funcOriginal(a1);

        return thing;
    }

public:
    typedef __int64(__thiscall *getCurrentSwingDurationOriginal)(__int64);

    static inline getCurrentSwingDurationOriginal funcOriginal = nullptr;

    getCurrentSwingDuration() : Hook("getCurrentSwingDurationHook",
                                     GET_SIG("getCurrentSwingDuration")) {}

    void enableHook() override {
        this->autoHook((void *) getCurrentSwingDurationCallback, (void **) &funcOriginal);
    }
};


