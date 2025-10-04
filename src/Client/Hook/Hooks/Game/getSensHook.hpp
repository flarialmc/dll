#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Events/Input/SensitivityEvent.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include <cmath>

class getSensHook : public Hook {
private:
    // max sens = 0.858693;

    static float getSensCallback(void* options, unsigned int a2, int a3, int a4, int a5) {

        float sensitivity = funcOriginal(options, a2, a3, a4, a5);

        auto event = nes::make_holder<SensitivityEvent>(sensitivity);
        eventMgr.trigger(event);

        sensitivity = event->getSensitivity();
        return sensitivity;
    }

public:
    typedef float(__thiscall *getSensOriginal)(void* options, unsigned int a2, int a3, int a4, int a5);

    static inline getSensOriginal funcOriginal = nullptr;

    getSensHook() : Hook("getSensHook", GET_SIG_ADDRESS("Options::getSensitivity")) {}

    void enableHook() override {
        this->autoHook((void *) getSensCallback, (void **) &funcOriginal);
    }
};


