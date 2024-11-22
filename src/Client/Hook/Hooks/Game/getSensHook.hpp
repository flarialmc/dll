#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Events/Input/SensitivityEvent.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include <cmath>

class getSensHook : public Hook {
private:
    // max sens = 0.858693;

    static float getSensCallback(unsigned int *a1, unsigned int a2) {

        float sensitivity = funcOriginal(a1, a2);

        auto event = nes::make_holder<SensitivityEvent>(sensitivity);
        eventMgr.trigger(event);

        return event->getSensitivity();
    }

public:
    typedef float(__thiscall *getSensOriginal)(unsigned int *, unsigned int);

    static inline getSensOriginal funcOriginal = nullptr;

    getSensHook() : Hook("getSensHook", GET_SIG_ADDRESS("Options::getSensitivity")) {}

    void enableHook() override {
        this->autoHook((void *) getSensCallback, (void **) &funcOriginal);
    }
};


