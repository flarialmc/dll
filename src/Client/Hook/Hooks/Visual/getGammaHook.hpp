 #pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"
#include "../../../Client.hpp"

class getGammaHook : public Hook {
private:

    static float getGammaCallback(uintptr_t a1) {

        if (!Options::isInitialized()) {
            Options::initialize(a1);
        }

        auto event = nes::make_holder<GammaEvent>(funcOriginal(a1));
        eventMgr.trigger(event);

        return event->getGamma();
    }

public:
    typedef float(__thiscall *getGammaOriginal)(uintptr_t);

    static inline getGammaOriginal funcOriginal = nullptr;

    getGammaHook() : Hook("getGammaHook",GET_SIG_ADDRESS("Options::getGamma")) {}

    void enableHook() override {
        this->autoHook((void *) getGammaCallback, (void **) &funcOriginal);
    }
};