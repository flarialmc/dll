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

        GammaEvent event(funcOriginal(a1));
        EventHandler::onGetGamma(event);

        return event.getGamma();
    }

public:
    typedef float(__thiscall *getGammaOriginal)(uintptr_t);

    static inline getGammaOriginal funcOriginal = nullptr;

    getGammaHook() : Hook("getGammaHook",
                          "48 83 EC ? 80 B9 ? ? ? ? ? 48 8D 54 24 ? 48 8B 01 48 8B 40 ? 74 ? 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 74 ? 48 8B 42 ? 48 8B 88 ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 83 C4 ? C3 F3 0F 10 42 ? 48 83 C4 ? C3 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 75 ? E8 ? ? ? ? CC E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC F3 0F 11 4C 24") {}

    void enableHook() override {
        this->autoHook((void *) getGammaCallback, (void **) &funcOriginal);
    }
};